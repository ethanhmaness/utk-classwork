import sys
import numpy as np

# Calculates n!
def factorial(n: int):
    if n == 0:
        return 1
    for i in range(1,n):
        n *= i
    return n

# Calculates n! / (k! * (n-k)!)
def n_choose_k(n: int, k: int):
    return factorial(n) // (factorial(k) * factorial(n-k))

# Returns an array representing the binomial distribution B(n, p) 
# So binomial_pmf(n,p)[i] equals the probablity X = i for a random variable X ~ B(n, p) 
def binomial_pmf(n: int, p: float):
    return [n_choose_k(n, i) * p**i * (1-p)**(n-i) for i in range(n+1)]


# Read command line args
N_MIN = int(sys.argv[1])
N_MAX = int(sys.argv[2])
P_BIRTH = float(sys.argv[3])
R_OVER = float(sys.argv[4])
DISCOUNT = float(sys.argv[5])

# Have the fail state be represented by the index one past the last non-fail state
FAIL_STATE = 2 * N_MAX + 1

# Get the probabilities for each possible number of births for a given state/population
# n_births_probabilities(p)[i] is the probability of p existing monkeys having a total of i births in a year.
def n_births_probabilities(population: int):
    return binomial_pmf(population//2, P_BIRTH)

# Get the probabilities of transitioning to each state for a given start state and action
# transition_probabilities(s, n)[i] is the probability of moving to state i from state s on action n
def transition_probabilities(state: int, n_release: int):
    result = [0.0] * (FAIL_STATE + 1)
    if state >= FAIL_STATE:
        return result
    new_pop = state - n_release
    b_probs = n_births_probabilities(new_pop)
    for i in range(len(b_probs)):
        if new_pop + i >= FAIL_STATE:
            result[FAIL_STATE] += b_probs[i]
        else:
            result[new_pop + i] = b_probs[i]
    return result

# Get the reward for taking an action from a given state
def reward(state: int):
    if state >= FAIL_STATE:
        return R_OVER
    if state < N_MIN or state > N_MAX:
        return 0
    return state

# Get the expected return for an action and state based on a given value function
def expected_return(state: int, n_release: int, vfunc: list[float]):
    t_prob = transition_probabilities(state, n_release)
    return reward(state) + DISCOUNT * np.sum([t_prob[new_state] * vfunc[new_state] for new_state in range(len(t_prob))])

# Get the next step in value iteration for a given value function
# Value function should have 2*N_MAX + 2 elements, corresponding to states 0 thru 2*N_MAX and the fail state.
def iterate_vfunc(vfunc):
    new_vfunc = []
    for state in range(FAIL_STATE + 1):
        new_vfunc.append(np.max([expected_return(state, n_release, vfunc) for n_release in range(state + 1)]))
    return new_vfunc

# Determine the optimal policy for a given value function
# optimal_policy(vfunc)[i] is the optimal action (number of monkeys to release) for state i
def optimal_policy(vfunc):
    policy = []
    for state in range(FAIL_STATE + 1):
        returns = []
        for n_release in range(state + 1):
            returns.append(expected_return(state, n_release, vfunc))
        policy.append(np.argmax(returns))
    return policy

# Initialize all reward values to 0
vfunc = [0]*(FAIL_STATE + 1)
# Perform value iteration for a bunch of steps
for i in range(9001):
    vfunc = iterate_vfunc(vfunc)
# Get policy from converged optimal value function
policy = optimal_policy(vfunc)

print(vfunc)

# # Print results
# print("Value function:")
# for i in range(FAIL_STATE):
#     print(f"{i} monkeys: {vfunc[i]}")
# print("")
# print("Policy:")
# for i in range(FAIL_STATE):
#     print(f"{i} Monkeys: Release {policy[i]}")
