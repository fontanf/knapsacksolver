WORK IN PROGRESS

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

Table of Contents
=================

  * [Variable reductions](#variable-reductions)
  * [Lower bounds](#lower-bounds)
     * [O(n) Greedy algorithms](#on-greedy-algorithms)
     * [O(n log n) Greedy algorithms](#on-log-n-greedy-algorithms)
  * [Upper bounds](#upper-bounds)
  * [DP with Bellman recursion](#dp-with-bellman-recursion)
     * [With arrays](#with-arrays)
     * [With lists](#with-lists)
  * [DP by profits](#dp-by-profits)
  * [Balanced Dynamic Programming](#balanced-dynamic-programming)
  * [Primal Branch-and-bound](#primal-branch-and-bound)
  * [Primal-Dual Branch-and-bound](#primal-dual-branch-and-bound)

## Variable reductions

*(See "Knapsack Problem", 5.1.3 Variable Reduction - Kellerer et al., 2004)*

The second reduction eliminates more items but is slower and require a complete
sorting of the items.

- Option `-r 1`: Partial sorting, Trivial upper bound
- Option `-r 2`: Complete sorting, Dantzig upper bound


## Lower bounds

### O(n) Greedy algorithms

*(See "Knapsack Problem", 2.5 Approximation Algorithms, 5.1.2 Lower Bounds for (KP) - Kellerer et al., 2004)*

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

### O(n log n) Greedy algorithms

Those Greedy are similar to the one described in "A fast algorithm for strongly
correlated knapsack problems" (Pisinger, 1998).

- Forward Greedy `lb_greedynlogn/main -a for`
- Backward Greedy `lb_greedynlogn/main -a back`
- Best Greedy `lb_greedynlogn/main -a best`

## Upper bounds

*(See "Knapsack Problem", 5.1.1 Upper Bounds for (KP) - Kellerer et al., 2004)*

- Dantzig Upper Bound `ub_dantzig/main`
- Surrogate relaxation `ub_surrogate/main`

## Exact algorithm without pre-processing or sorting

### Dynamic programming with Bellman recursion

The Bellman recursion is implemented both with array and list as memory. Use
option `-m array` of `-m list` to select which one to use (default `array`).

Option `-r` selects the methods used to retrieve the optimal solution. Possible
values are
- `none`: no solution retrieved, only the optimal value is returned
- `all`: keep all states in memory and backtrack
- `one`: keep only the last states in memory, retrieve the last item added and
run the algorithm again to retrieve the complete optimal solution
- `part`: keep a partial solution in each state and run the algorithm again
while the global solution is not complete
- `rec`: use the recursive scheme

Using Dynamic programming with lists allow using bound. Since the items are not
sorted, the `U0` bound is used.

### Dynamic programming by profits

*(See "Knapsack Problem", 2.3 - Kellerer et al., 2004)*

- Only optimal value, `opt_dpreaching/main -a opt`
- Optimal solution, `opt_bellman/main -a sopt`

### Primal Branch-and-bound

*(See "Knapsack Problem", 2.4 Branch-and-Bound - Kellerer et al., 2004)*

Use option `-u` (possible values: `trivial`, `dantzig` or `dantzig_2`) to select
which upper bound is used.

- Implementation with a recursive function (default) `opt_bab/main -a rec`
- Implementation with a stack simulating a recusrive function `opt_bab/main -a stack`

## Exact algorithms with partial or complete sorting as pre-processing

### Balanced Dynamic programming

*(See "Knapsack Problem", 5.3.1 Balanced Dynamic Programming - Kellerer et al., 2004)*

Use option `-u` (possible values: `dembo`, `trivial` or `dantzig`) to select which upper bound is used.

- With tables, Only optimal value, `opt_balknap/main -a opt`
- With tables, Optimal solution, `opt_balknap/main -a sopt`
- With lists (maps), only optimal value, `opt_balknap/main -a opt_list`
- With lists (maps), optimal solution, `opt_balknap/main -a sopt_list`

### Primal-dual Dynamic programming (`minknap`, `combo`)

### Primal-dual Branch-and-bound (`expknap`)

*(See "Knapsack Problem", 5.1.4 Branch-and-Bound Implementations - Kellerer et al., 2004)*

Use option `-u` (possible values: `trivial` or `dantzig`) to select which upper bound is used.

- `opt_babprimaldual/main`

