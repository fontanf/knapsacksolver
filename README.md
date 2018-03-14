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

* For Dynamic programming algorithms, option `-m` selects the type of memory used. Possible values are (if implemented) `array` or `list`. Lists are slower but eliminate dominated states and allow the use of Upper bounds.

* For Dynamic programming algorithms, option `-r` selects the method used to retrieve the optimal solution. Possible values (if implemented) are
  - `none`: no solution retrieved, only the optimal value is returned
  - `all`: keep all states in memory and backtrack
  - `one`: keep only the last states in memory, retrieve the last item added and run the algorithm again to retrieve the complete optimal solution
  - `part`: keep a partial solution in each state and run the algorithm again while the global solution is not complete
  - `rec`: use the recursive scheme

## Lower bounds

### O(n) Greedy algorithms

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

- Dantzig Upper bound `ub_dantzig/main`
- Surrogate relaxation `ub_surrogate/main`

## Exact algorithm without pre-processing or sorting

For those algorithms, if required, the Upper bound used is neccessarly `U0`.

- Dynamic programming with Bellman recursion `opt_bellman/main -m array -r all`
- Dynamic programming by Profits `opt_dpprofits/main -m array -r all`
- Primal Branch-and-bound `opt_bab/main`

## Exact algorithms with partial or complete sorting as pre-processing

Primal-dual Dynamic programming and Primal-dual Branch-and-bound allow to sort and reduce items 'by need' instead. This makes them more efficient on small or easy instances compared to other algorithms of the litterature which sort and reduce items in a pre-processing step. Balanced Dynamic programming may require only the partial sorting, however better bounds can be used if items are fully sorted.

- Balanced Dynamic programming `opt_balknap/main -m list -u t -r part` Since the list implementation require a map, its asymptotical complexity is slightly greater than the one with an array. On the other hand, better Upper bounds can be used. Therefore, both are implemented. Furthermore, options `-u` can be set to
  - b: partial sorting, Dembo Upper bound (with break item)
  - t: complete sorting, better Upper Bound
- Primal-dual Dynamic programming (`minknap`, `combo`)
- Primal-dual Branch-and-bound (`expknap`)

