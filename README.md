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

- Ext-Greedy `lb_greedy/main` *(See "Knapsack Problem", 2.5 Approximation Algorithms - Kellerer et al., 2004)*
- Local search `lb_ls/main`

## Upper bounds

- Dantzig Upper Bound `ub_dantzig/main` *(See "Knapsack Problem", 5.1.1 Upper Bounds for (KP) - Kellerer et al., 2004)*
- Surrogate relaxation `ub_surrogate/main` *(See "Knapsack Problem", 5.1.1 Upper Bounds for (KP) - Kellerer et al., 2004)*

## DP with Bellman recursion

### With arrays *(See "Knapsack Problem", 2.3, 3.3 - Kellerer et al., 2004)*

- Only optimal value, Time O(nc), Space O(c) `opt_bellman/main`
- Version 1, Time O(nc), Space O(nc) `opt_bellman/main -a 1`
  - Iterative implementation (default) `opt_bellman/main -a 1it`
  - Implementation with a recursive function `opt_bellman/main -a 1rec`
  - Implementation with a stack simulating a recursive function `opt_bellman/main -a 1stack`
  - Recursive implementation using a map as memory `opt_bellman/main -a 1map`
- Version 2, Time O(n2c), Space O(c+n) `opt_bellman/main -a 2`
- Recursive scheme Time O(nc), Space O(n+c) `opt_bellman/main -a rec`

### With lists *(See "Knapsack Problem", 3.4, 3.5 - Kellerer et al., 2004)*

- Only optimal value `opt_bellman/main -a list`
- Recursive scheme `opt_bellman/main -a reclist`
- With Upper Bounds, only optimal value `opt_bellman/main -a ub`
- With Upper Bounds, recursive scheme `opt_bellman/main -a recub`

## DP by profits *(See "Knapsack Problem", 2.3 - Kellerer et al., 2004)*

- Only optimal value `opt_dpreaching/main`
- With solution `opt_bellman/main -a 1`

## Balanced Dynamic Programming *(See "Knapsack Problem", 5.3.1 - Kellerer et al., 2004)*

- Only optimal value, partial sorting, Dembo and Hammer UB, `opt_balknap/main -a opt`
- Optimal solution, partial sorting, Dembo and Hammer UB, `opt_balknap/main -a sopt`
- With lists (maps), only optimal value, partial sorting, Dembo and Hammer UB, `opt_balknap/main -a opt_list_partsorted`
- With lists (maps), only optimal value, complete sorting, Dantzig UB, `opt_balknap/main -a opt_list_sorted`
- With lists (maps), optimal solution, partial sorting, Dembo and Hammer UB, `opt_balknap/main -a sopt_list_partsorted`
- With lists (maps), optimal solution, complete sorting, Dantzig UB, `opt_balknap/main -a sopt_list_sorted`

## Primal Branch-and-bound *(See "Knapsack Problem", 2.4 Branch-and-Bound - Kellerer et al., 2004)*

- Complete sorting, Dantizg UB
  - Implementation with a recursive function (default) `opt_bab/main -a rec`
  - Implementation with a stack simulating a recusrive function `opt_bab/main -a stack`

## Primal-Dual Branch-and-bound *(See "Knapsack Problem", 5.1.4 Branch-and-Bound Implementations - Kellerer et al., 2004)*

- Partial sorting, Dembo and Hammer UB, `opt_babprimaldual/main -a partsorted`
- Complete sorting, Dantzig UB, `opt_babprimaldual/main -a sorted`

