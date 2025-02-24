import subprocess

for env in ["cartpole", "pendulum", "cheetah"]:
    for method in ["no-baseline", "baseline", "ppo"]:
        seeds = [1,2,3] if env != "cheetah" else [1]
        for seed in seeds:
            subprocess.run(["python", "main.py", "--env-name", env, "--seed", str(seed), f"--{method}"])
