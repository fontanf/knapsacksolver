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

## Notes

* If the split item is searched at the beginning, then the Bellman algorithms are more or less dominated by the Primal-dual Dynamic programming algorithms, as well as the Primal Branch-and-bound is more or less dominated by the Primal-dual Branch-and-bound. Therefore, as soon as a pre-processing step requires at leat a partial sorting, it is more consistent to use those algorithms. This explains why variable reductions, initial lower bound or better upper bounds are not implemented for the Bellman Dynamic Programming algoorithms, the Dynamic Programming by Profits algorithms or the Primal Branch-and-bound algorithm.

* On the other hand, the Primal-Dual algorithms require to find the split item at the beginning.


## Lower bounds

### O(n) Greedy algorithms

*(See "Knapsack Problem", 2.5 Approximation Algorithms, 5.1.2 Lower Bounds for (KP) - Kellerer et al., 2004)*

The *Max Greedy* algorithm correspond to packing the item with the largest weight first and then filling the remaining capacity with the greedy algorithm. Thus, if used with Greedy, the algorithm has a relative performance guarantee of 1/2.

The *Best Greedy* algorithm runs all the other greedy algorithms and keeps the best solution found.

- Greedy `lb_greedy/main`
- Max Greedy `lb_greedy/main -a max`
- Forward Greedy `lb_greedy/main -a for`
- Backward Greedy `lb_greedy/main -a back`
- Best Greedy `lb_greedy/main -a best`

### O(n log n) Greedy algorithms

Those Greedy are similar to the one described in "A fast algorithm for strongly correlated knapsack problems" (Pisinger, 1998).

- Forward Greedy `lb_greedynlogn/main -a for`
- Backward Greedy `lb_greedynlogn/main -a back`
- Best Greedy `lb_greedynlogn/main -a best`

## Upper bounds

*(See "Knapsack Problem", 5.1.1 Upper Bounds for (KP) - Kellerer et al., 2004)*

- Dantzig Upper bound `ub_dantzig/main`
- Surrogate relaxation `ub_surrogate/main`

## Exact algorithm without pre-processing or sorting

### Dynamic programming with Bellman recursion

```
opt_bellman/main -m array -r all
```

The Bellman recursion is implemented both with array and list as memory. Use option `-m array` of `-m list` to select which one to use (default `array`).

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

### Dynamic programming by Profits

```
opt_dpprofits/main -m array -r all
```

### Primal Branch-and-bound

*(See "Knapsack Problem", 2.4 Branch-and-Bound - Kellerer et al., 2004)*

```
opt_bab/main
```

## Exact algorithms with partial or complete sorting as pre-processing

### Balanced Dynamic programming

*(See "Knapsack Problem", 5.3.1 Balanced Dynamic Programming - Kellerer et al., 2004)*

```
opt_balknap/main -m list -u t -r part
```

Since the list implementation require a map, its asymptotical complexity is slightly greater than the one with an array. On the other hand, better Upper bounds can be used. Therefore, both are implemented.

Options `-u` can be set to
- b: partial sorting, Dembo Upper bound (with break item)
- t: complete sorting, better Upper Bound

### Primal-dual Dynamic programming (`minknap`, `combo`)

### Primal-dual Branch-and-bound (`expknap`)

*(See "Knapsack Problem", 5.1.4 Branch-and-Bound Implementations - Kellerer et al., 2004)*

