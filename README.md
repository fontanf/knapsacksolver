*Note: Rewritinig in progress*

# Knapsack

Implementations of classical algorithms for the Knapsack Problem. Most algorithms are detailed in the "Knapsack Problem" book (Kellerer et al., 2004).

This project uses Bazel https://bazel.build/

Compile:
```
bazel build --compilation_mode=opt -- //...
```

Execute:
```
bazel-bin/lb_greedy/main -i ...
```

## Lower bounds

*(See "Knapsack Problem",
2.5 Approximation Algorithms,
5.1.2 Lower Bounds for (KP) - Kellerer et al., 2004)*

The *Max Greedy* algorithm correspond to packing the item with the largest
weight first and then filling the remaining capacity with the greedy algorithm.
Thus, if used with Greedy, the algorithm has a relative performance guarantee of
1/2.

The *Best Greedy* algorithm runs all the other greedy algorithms and keeps the
best solution found.

- Greedy `lb_greedy/main`
- Max Greedy `lb_greedy/main -a max`
- Forward Greedy `lb_greedy/main -a for`
- Backward Greedy `lb_greedy/main -a back`
- Best Greedy `lb_greedy/main -a best`

## Upper bounds

*(See "Knapsack Problem", 5.1.1 Upper Bounds for (KP) - Kellerer et al., 2004)*

- Dantzig Upper Bound `ub_dantzig/main`
- Surrogate relaxation `ub_surrogate/main`

## DP with Bellman recursion

### With arrays

*(See "Knapsack Problem", 2.3, 3.3 - Kellerer et al., 2004)*

- Only optimal value, Time O(nc), Space O(c) `opt_bellman/main -a opt`
- Version 1, Time O(nc), Space O(nc) `opt_bellman/main -a sopt_1`
  - Iterative implementation (default) `opt_bellman/main -a sopt_1it`
  - Implementation with a recursive function `opt_bellman/main -a sopt_1rec`
  - Implementation with a stack simulating a recursive function `opt_bellman/main -a sopt_1stack`
  - Recursive implementation using a map as memory `opt_bellman/main -a sopt_1map`
- Version 2, Time O(n2c), Space O(c+n) `opt_bellman/main -a sopt_2`
- Recursive scheme Time O(nc), Space O(n+c) `opt_bellman/main -a sopt_rec`

### With lists

*(See "Knapsack Problem", 3.4, 3.5 - Kellerer et al., 2004)*

Options `-u` can be set to `trivial` or `dantzig` to combine Dynamic Programming
with upper bounds. In this case, option `-l` can be set to `none` or `greedy` to
select which lower bound to use.

- Only optimal value `opt_bellman/main -a opt_list`
- Recursive scheme `opt_bellman/main -a sopt_list_rec`

## DP by profits

*(See "Knapsack Problem", 2.3 - Kellerer et al., 2004)*

- Only optimal value, `opt_dpreaching/main -a opt`
- Optimal solution, `opt_bellman/main -a sopt`

## Balanced Dynamic Programming

*(See "Knapsack Problem", 5.3.1 - Kellerer et al., 2004)*

- Only optimal value, partial sorting, Dembo and Hammer UB, `opt_balknap/main -a opt`
- Optimal solution, partial sorting, Dembo and Hammer UB, `opt_balknap/main -a sopt`
- With lists (maps), only optimal value, partial sorting, Dembo and Hammer UB, `opt_balknap/main -a opt_list_partsorted`
- With lists (maps), only optimal value, complete sorting, Dantzig UB, `opt_balknap/main -a opt_list_sorted`
- With lists (maps), optimal solution, partial sorting, Dembo and Hammer UB, `opt_balknap/main -a sopt_list_partsorted`
- With lists (maps), optimal solution, complete sorting, Dantzig UB, `opt_balknap/main -a sopt_list_sorted`

## Primal Branch-and-bound

*(See "Knapsack Problem", 2.4 Branch-and-Bound - Kellerer et al., 2004)*

Set option `-u` (possible values `trivial`, `dantzig`, `dantzig_2`) to select
which upper bound is used.

- Implementation with a recursive function (default) `opt_bab/main -a rec`
- Implementation with a stack simulating a recusrive function `opt_bab/main -a stack`

## Primal-Dual Branch-and-bound

*(See "Knapsack Problem", 5.1.4 Branch-and-Bound Implementations - Kellerer et al., 2004)*

- Partial sorting, Dembo and Hammer UB, `opt_babprimaldual/main -a partsorted`
- Complete sorting, Dantzig UB, `opt_babprimaldual/main -a sorted`

