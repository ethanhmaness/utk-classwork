import os
import csv
import gymnasium as gym
import torch
import torch.nn as nn
import torch.optim as optim
import torch.nn.functional as F
import numpy as np
import ale_py
from gymnasium.wrappers import GrayscaleObservation, ResizeObservation
import optuna
from gymnasium.vector import AsyncVectorEnv
from functools import partial
from gymnasium.wrappers import GrayscaleObservation, ResizeObservation

# ------------------------------------------
# Changes from original:
# - Using vectorized env
# - Preprocessing observations (grayscale, resize)
# - Removed video recording during training
# - Reduced total_episodes for faster runs
# - Added Optuna objective function
# - Parameterized hyperparameters
# ------------------------------------------

device = torch.device("cuda" if torch.cuda.is_available() else "cpu")
print(f"Using device: {device}")

def make_env(env_id):
    def _make():
        env = gym.make(env_id, frameskip=4)
        env = GrayscaleObservation(env, keep_dim=False)
        env = ResizeObservation(env, (84, 84))
        return env
    return _make

class CNNPolicy(nn.Module):
    def __init__(self, action_space):
        super(CNNPolicy, self).__init__()
        self.conv = nn.Sequential(
            nn.Conv2d(1, 32, kernel_size=8, stride=4),
            nn.ReLU(),
            nn.Conv2d(32, 64, kernel_size=4, stride=2),
            nn.ReLU(),
            nn.Conv2d(64, 64, kernel_size=3, stride=1),
            nn.ReLU()
        )

        test_inp = torch.zeros(1, 1, 84, 84)
        conv_out = self.conv(test_inp)
        conv_out_size = conv_out.numel()

        self.fc = nn.Sequential(
            nn.Linear(conv_out_size, 512),
            nn.ReLU()
        )
        self.pi = nn.Linear(512, action_space.n)
        self.vf = nn.Linear(512, 1)

    def forward(self, x):
        # Expect x in (B,C,H,W) with grayscale
        x = x.float() / 255.0
        x = x.to(device)
        conv_out = self.conv(x)
        flatten = torch.flatten(conv_out, 1)
        hidden = self.fc(flatten)
        logits = self.pi(hidden)
        value = self.vf(hidden)
        return logits, value

    def get_action_and_value(self, obs, action=None):
        logits, value = self.forward(obs)
        dist = torch.distributions.Categorical(logits=logits)
        if action is None:
            action = dist.sample()
        log_prob = dist.log_prob(action)
        return action, log_prob, dist.entropy().mean(), value


class PPOAgent:
    def __init__(self, env, learning_rate, gamma, lam, clip_range, num_steps_per_update, mini_batch_size, ppo_epochs):
        self.env = env
        self.obs_shape = env.single_observation_space.shape
        self.action_space = env.single_action_space
        self.gamma = gamma
        self.lam = lam
        self.clip_range = clip_range
        self.num_steps_per_update = num_steps_per_update
        self.mini_batch_size = mini_batch_size
        self.ppo_epochs = ppo_epochs

        self.policy = CNNPolicy(self.action_space).to(device)
        self.optimizer = optim.Adam(self.policy.parameters(), lr=learning_rate)

    def collect_trajectories(self):
        num_envs = self.env.num_envs
        obs_buf = []
        actions_buf = []
        rewards_buf = []
        dones_buf = []
        values_buf = []
        logprobs_buf = []

        obs = self.env.reset(seed=None)[0] 
        for _ in range(self.num_steps_per_update):
            obs_tensor = torch.tensor(obs, dtype=torch.uint8).unsqueeze(1).to(device)  
            with torch.no_grad():
                action, logprob, _, value = self.policy.get_action_and_value(obs_tensor)

            action_cpu = action.cpu().numpy()
            next_obs, reward, done, trunc, info = self.env.step(action_cpu)
            # Store transitions
            obs_buf.append(obs)
            actions_buf.append(action_cpu)
            rewards_buf.append(reward)
            dones_buf.append(done)

            values_buf.append(value.cpu().numpy().squeeze(-1))
            logprobs_buf.append(logprob.cpu().numpy())

            obs = next_obs
        # Compute advantage targets
        obs_tensor = torch.tensor(obs, dtype=torch.uint8).unsqueeze(1).to(device)
        with torch.no_grad():
            _, _, _, next_value = self.policy.get_action_and_value(obs_tensor)
        next_value = next_value.cpu().numpy().squeeze(-1)

        # Convert to np arrays
        obs_buf = np.array(obs_buf) 
        actions_buf = np.array(actions_buf) 
        rewards_buf = np.array(rewards_buf) 
        dones_buf = np.array(dones_buf)     
        values_buf = np.array(values_buf) 
        logprobs_buf = np.array(logprobs_buf) 

        advantages, returns = self.compute_gae(rewards_buf, values_buf, dones_buf, next_value)

        obs_buf = obs_buf.reshape(-1, *obs_buf.shape[2:])
        actions_buf = actions_buf.flatten()
        logprobs_buf = logprobs_buf.flatten()
        values_buf = values_buf.flatten()
        advantages = advantages.flatten()
        returns = returns.flatten()

        return obs_buf, actions_buf, logprobs_buf, values_buf, advantages, returns

    def compute_gae(self, rewards, values, dones, next_value):
        T, N = rewards.shape
        values = np.concatenate([values, next_value[None, :]], axis=0) 
        advantages = np.zeros((T, N), dtype=np.float32)
        gae = np.zeros(N, dtype=np.float32)
        for t in reversed(range(T)):
            mask = 1.0 - dones[t]
            delta = rewards[t] + self.gamma * values[t+1] * mask - values[t]
            gae = delta + self.gamma * self.lam * mask * gae
            advantages[t] = gae
        returns = advantages + values[:-1]
        return advantages, returns

    def update(self, obs, actions, old_logprobs, values, advantages, returns):
        advantages = (advantages - advantages.mean()) / (advantages.std() + 1e-8)
        dataset = list(zip(obs, actions, old_logprobs, values, advantages, returns))
        entropy_total, policy_loss_total, value_loss_total = 0, 0, 0
        for _ in range(self.ppo_epochs):
            np.random.shuffle(dataset)
            for start in range(0, len(dataset), self.mini_batch_size):
                end = start + self.mini_batch_size
                batch = dataset[start:end]
                if len(batch) < self.mini_batch_size:
                    continue
                obs_b, actions_b, old_logprobs_b, values_b, advantages_b, returns_b = zip(*batch)

                obs_b = torch.tensor(np.array(obs_b), dtype=torch.uint8).unsqueeze(1).to(device)
                actions_b = torch.tensor(actions_b, dtype=torch.long).to(device)
                old_logprobs_b = torch.tensor(old_logprobs_b, dtype=torch.float32).to(device)
                values_b = torch.tensor(values_b, dtype=torch.float32).to(device)
                advantages_b = torch.tensor(advantages_b, dtype=torch.float32).to(device)
                returns_b = torch.tensor(returns_b, dtype=torch.float32).to(device)

                _, new_logprobs, entropy, new_values = self.policy.get_action_and_value(obs_b, actions_b)
                ratio = torch.exp(new_logprobs - old_logprobs_b)
                surr1 = ratio * advantages_b
                surr2 = torch.clamp(ratio, 1.0 - self.clip_range, 1.0 + self.clip_range) * advantages_b
                policy_loss = -torch.min(surr1, surr2).mean()

                value_loss = F.mse_loss(new_values.squeeze(-1), returns_b)
                loss = policy_loss + 0.5 * value_loss - 0.01 * entropy

                self.optimizer.zero_grad()
                loss.backward()
                self.optimizer.step()

                entropy_total += entropy.item()
                policy_loss_total += policy_loss.item()
                value_loss_total += value_loss.item()

        num_batches = (self.ppo_epochs * (len(dataset) // self.mini_batch_size))
        return entropy_total / num_batches, policy_loss_total / num_batches, value_loss_total / num_batches

def evaluate_agent(env_id, agent, num_episodes=5):
    # Evaluate the agent on a single environment (non-vectorized)
    eval_env = gym.make(env_id, frameskip=4)
    eval_env = GrayscaleObservation(eval_env, keep_dim=False)
    eval_env = ResizeObservation(eval_env, (84, 84))
    reward_sum = 0
    for _ in range(num_episodes):
        obs = eval_env.reset(seed=None)[0]
        done = False
        trunc = False
        ep_reward = 0
        while not done and not trunc:
            obs_tensor = torch.tensor(np.array([obs]), dtype=torch.uint8).unsqueeze(1).to(device)
            with torch.no_grad():
                action, _, _, _ = agent.policy.get_action_and_value(obs_tensor)
            obs, reward, done, trunc, info = eval_env.step(action.item())
            ep_reward += reward
        reward_sum += ep_reward
    eval_env.close()
    return reward_sum / num_episodes


def objective(trial):
    # Hyperparameter search space
    learning_rate = trial.suggest_loguniform('learning_rate', 1e-5, 1e-3)
    clip_range = trial.suggest_uniform('clip_range', 0.1, 0.3)
    gamma = trial.suggest_uniform('gamma', 0.95, 0.999)
    lam = trial.suggest_uniform('lam', 0.9, 0.97)
    ppo_epochs = trial.suggest_int('ppo_epochs', 1, 10)
    mini_batch_size = trial.suggest_categorical('mini_batch_size', [32, 64, 128])
    num_steps_per_update = trial.suggest_categorical('num_steps_per_update', [512, 1024, 2048])

    # Create vectorized environment for training
    env_id = "ALE/Pong-v5"
    num_envs = 8
    env = AsyncVectorEnv([make_env(env_id) for _ in range(num_envs)])
    
    agent = PPOAgent(env, learning_rate, gamma, lam, clip_range, num_steps_per_update, mini_batch_size, ppo_epochs)

    # Reduced training episodes for speed
    total_episodes = 100  

    reward_history = []
    for episode in range(total_episodes):
        obs, actions, old_logprobs, values, advantages, returns = agent.collect_trajectories()
        entropy, policy_loss, value_loss = agent.update(obs, actions, old_logprobs, values, advantages, returns)
        if (episode + 1) % 10 == 0:
            avg_reward = evaluate_agent(env_id, agent, num_episodes=1)
            reward_history.append(avg_reward)

    env.close()

    # If we never evaluated during training, do a final evaluation:
    if len(reward_history) == 0:
        avg_reward = evaluate_agent(env_id, agent, num_episodes=5)
    else:
        avg_reward = np.mean(reward_history[-5:])  

    return avg_reward


if __name__ == "__main__":
    study = optuna.create_study(direction='maximize')
    study.optimize(objective, n_trials=10)  

    print("Best trial:")
    trial = study.best_trial
    print(trial.params)
