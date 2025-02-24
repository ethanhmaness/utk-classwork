# -*- coding: utf-8 -*-
"""
Created on Tue Dec 10 00:07:29 2024

@author: djwou
"""
import ale_py
import gymnasium as gym
import torch
import torch.nn as nn
import torch.optim as optim
import torch.nn.functional as F
import numpy as np
import csv
import os
from gymnasium.wrappers import RecordVideo

# Ensure directory for videos exists
video_dir = "videos"
os.makedirs(video_dir, exist_ok=True)

############################
# Hyperparameters
############################
env_id = "ALE/Pong-v5"
learning_rate = 2.5e-4
gamma = 0.99
lam = 0.95
clip_range = 0.1
num_steps_per_update = 2048
mini_batch_size = 64
ppo_epochs = 4
total_episodes = 1000
test_interval = 50
render_mode = None  # Set to 'human' if you want to see training

# Set device to GPU if available
device = torch.device("cuda" if torch.cuda.is_available() else "cpu")
print(f"Using device: {device}")

############################
# Neural Network Models
############################
class CNNPolicy(nn.Module):
    def __init__(self, action_space):
        super(CNNPolicy, self).__init__()
        # Pong observation: (210,160,3)

        self.conv = nn.Sequential(
            nn.Conv2d(3, 32, kernel_size=8, stride=4),
            nn.ReLU(),
            nn.Conv2d(32, 64, kernel_size=4, stride=2),
            nn.ReLU(),
            nn.Conv2d(64, 64, kernel_size=3, stride=1),
            nn.ReLU()
        )

        # Compute the size of the linear layer
        test_inp = torch.zeros(1, 3, 210, 160)
        conv_out = self.conv(test_inp)
        conv_out_size = conv_out.numel()

        self.fc = nn.Sequential(
            nn.Linear(conv_out_size, 512),
            nn.ReLU()
        )

        self.pi = nn.Linear(512, action_space.n)
        self.vf = nn.Linear(512, 1)

    def forward(self, x):
        # Expect x in (B,H,W,C) from Gym, need (B,C,H,W)
        x = x.permute(0, 3, 1, 2).float() / 255.0
        x = x.to(device)
        conv_out = self.conv(x)
        flatten = torch.flatten(conv_out, 1)
        hidden = self.fc(flatten)
        logits = self.pi(hidden)
        value = self.vf(hidden)
        return logits, value

    def get_action_and_value(self, obs, action=None):
        obs = obs.to(device)
        logits, value = self.forward(obs)
        dist = torch.distributions.Categorical(logits=logits)
        if action is None:
            action = dist.sample()
        log_prob = dist.log_prob(action)
        return action, log_prob, dist.entropy().mean(), value


############################
# PPO Agent
############################
class PPOAgent:
    def __init__(self, env):
        self.env = env
        self.obs_shape = env.observation_space.shape
        self.action_space = env.action_space

        self.policy = CNNPolicy(self.action_space).to(device)
        self.optimizer = optim.Adam(self.policy.parameters(), lr=learning_rate)
    
    def collect_trajectories(self):
        obs_buf = []
        actions_buf = []
        rewards_buf = []
        dones_buf = []
        values_buf = []
        logprobs_buf = []

        obs = self.env.reset(seed=None)[0]

        for _ in range(num_steps_per_update):
            obs_tensor = torch.tensor(np.array([obs]), dtype=torch.float32).to(device)
            with torch.no_grad():
                action, logprob, _, value = self.policy.get_action_and_value(obs_tensor)

            action_cpu = action.item()
            next_obs, reward, done, trunc, info = self.env.step(action_cpu)
            obs_buf.append(obs)
            actions_buf.append(action_cpu)
            rewards_buf.append(reward)
            dones_buf.append(done)
            values_buf.append(value.item())
            logprobs_buf.append(logprob.item())

            obs = next_obs
            if done or trunc:
                obs = self.env.reset()[0]

        # Compute advantage targets
        obs_tensor = torch.tensor(np.array([obs]), dtype=torch.float32).to(device)
        with torch.no_grad():
            _, _, _, next_value = self.policy.get_action_and_value(obs_tensor)
        advantages, returns = self.compute_gae(rewards_buf, values_buf, dones_buf, next_value.item())
        return np.array(obs_buf), np.array(actions_buf), np.array(logprobs_buf), np.array(values_buf), advantages, returns

    def compute_gae(self, rewards, values, dones, next_value):
        values = np.append(values, next_value)
        advantages = np.zeros_like(rewards)
        gae = 0
        for t in reversed(range(len(rewards))):
            delta = rewards[t] + gamma * values[t+1] * (1 - dones[t]) - values[t]
            gae = delta + gamma * lam * (1 - dones[t]) * gae
            advantages[t] = gae
        returns = advantages + values[:-1]
        return advantages, returns

    def update(self, obs, actions, old_logprobs, values, advantages, returns):
        advantages = (advantages - advantages.mean()) / (advantages.std() + 1e-8)
        dataset = list(zip(obs, actions, old_logprobs, values, advantages, returns))
        entropy_total, policy_loss_total, value_loss_total = 0, 0, 0
        for _ in range(ppo_epochs):
            np.random.shuffle(dataset)
            for start in range(0, len(dataset), mini_batch_size):
                end = start + mini_batch_size
                batch = dataset[start:end]
                obs_b, actions_b, old_logprobs_b, values_b, advantages_b, returns_b = zip(*batch)
                
                obs_b = torch.tensor(np.array(obs_b), dtype=torch.float32).to(device)
                actions_b = torch.tensor(actions_b, dtype=torch.long).to(device)
                old_logprobs_b = torch.tensor(old_logprobs_b, dtype=torch.float32).to(device)
                values_b = torch.tensor(values_b, dtype=torch.float32).to(device)
                advantages_b = torch.tensor(advantages_b, dtype=torch.float32).to(device)
                returns_b = torch.tensor(returns_b, dtype=torch.float32).to(device)
    
                _, new_logprobs, entropy, new_values = self.policy.get_action_and_value(obs_b, actions_b)
                
                ratio = torch.exp(new_logprobs - old_logprobs_b)
                surr1 = ratio * advantages_b
                surr2 = torch.clamp(ratio, 1.0 - clip_range, 1.0 + clip_range) * advantages_b
                policy_loss = -torch.min(surr1, surr2).mean()
    
                value_loss = F.mse_loss(new_values.squeeze(-1), returns_b)
                loss = policy_loss + 0.5 * value_loss - 0.01 * entropy
    
                self.optimizer.zero_grad()
                loss.backward()
                self.optimizer.step()
    
                # Accumulate losses and entropy for logging
                entropy_total += entropy.item()
                policy_loss_total += policy_loss.item()
                value_loss_total += value_loss.item()
    
        # Return averaged losses and entropy
        num_batches = (ppo_epochs * len(dataset)) // mini_batch_size
        return entropy_total / num_batches, policy_loss_total / num_batches, value_loss_total / num_batches


############################
# Training Loop
############################
def run_episode(env, agent, record=False):
    obs = env.reset(seed=None)[0]
    done = False
    trunc = False
    ep_reward = 0
    while not done and not trunc:
        obs_tensor = torch.tensor(np.array([obs]), dtype=torch.float32).to(device)
        with torch.no_grad():
            action, _, _, _ = agent.policy.get_action_and_value(obs_tensor)
        next_obs, reward, done, trunc, info = env.step(action.item())
        ep_reward += reward
        obs = next_obs
    return ep_reward

def test_agent(agent, episode_num):
    # Create a RecordVideo env for testing
    test_env = RecordVideo(
        gym.make(env_id, render_mode='rgb_array', frameskip=4),
        video_folder="./videos",  # Use output_folder instead of video_dir
        episode_trigger=lambda x: True,  # Record every test episode
        name_prefix=f"test_episode_{episode_num}"
    )
    reward = run_episode(test_env, agent)
    test_env.close()
    print(f"Test Episode (episode {episode_num}): reward = {reward}")

def main():
    # Create training environment
    env = gym.make(env_id, frameskip=4, render_mode=render_mode)
    agent = PPOAgent(env)
    
    # Open CSV file for logging
    csv_file = open("training_stats.csv", mode="w", newline="")
    csv_writer = csv.writer(csv_file)
    csv_writer.writerow(["episode", "reward", "avg_reward", "max_reward", "min_reward", "entropy", "policy_loss", "value_loss"])
    reward_history = []
    max_reward = float('-inf')
    min_reward = float('inf')
    episode = 0
    while episode < total_episodes:
        # Collect experience
        obs, actions, old_logprobs, values, advantages, returns = agent.collect_trajectories()
        
        # Update policy
        entropy, policy_loss, value_loss = agent.update(obs, actions, old_logprobs, values, advantages, returns)
        
        # After updating, run a training episode to see performance
        ep_reward = run_episode(env, agent)
        episode += 1

        reward_history.append(ep_reward)
        max_reward = max(max_reward, ep_reward)
        min_reward = min(min_reward, ep_reward)
        avg_reward = np.mean(reward_history[-100:])

        # Log to CSV
        csv_writer.writerow([episode, ep_reward, avg_reward, max_reward, min_reward, entropy, policy_loss, value_loss])
        csv_file.flush()
   

        print(f"Episode {episode} reward: {ep_reward}, entropy: {entropy:.4f}, policy_loss: {policy_loss:.4f}, value_loss: {value_loss:.4f}")

        # Test and record video every test_interval episodes
        if episode % test_interval == 0:
            test_agent(agent, episode)

    csv_file.close()
    env.close()

if __name__ == "__main__":
    main()
