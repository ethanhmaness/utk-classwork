# -*- coding: UTF-8 -*-

import argparse
import numpy as np
import torch
import gymnasium as gym
import pybullet_envs_gymnasium
from policy_gradient import PolicyGradient
from ppo import PPO
from config import get_config
import random

import pdb

import logging
logging.getLogger("matplotlib.font_manager").setLevel(logging.ERROR)

parser = argparse.ArgumentParser()
parser.add_argument('-e', "--env-name", required=True, type=str, choices=["cartpole", "pendulum", "cheetah"])
parser.add_argument('-b', "--baseline", dest="use_baseline", action="store_true")
parser.add_argument('-nb', "--no-baseline", dest="use_baseline", action="store_false")
parser.add_argument('-ppo', "--ppo", dest="ppo", action="store_true")
parser.add_argument('-s', "--seed", type=int, default=1)

parser.set_defaults(use_baseline=True)


if __name__ == "__main__":
  args = parser.parse_args()

  torch.random.manual_seed(args.seed)
  np.random.seed(args.seed)
  random.seed(args.seed)

  config = get_config(args.env_name, args.use_baseline, args.ppo, args.seed)
  env = gym.make(config.env_name)
  # train model
  if not args.ppo:
    model = PolicyGradient(env, config, args.seed)
  else:
    model = PPO(env, config, args.seed)
  model.run()
