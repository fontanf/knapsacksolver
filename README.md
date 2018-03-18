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

- Greedy `lb_greedy/main` :heavy_check_mark:
- Max Greedy `lb_greedy/main -a max` :heavy_check_mark:
- Forward Greedy `lb_greedy/main -a for` :heavy_check_mark:
- Backward Greedy `lb_greedy/main -a back` :heavy_check_mark:
- Best Greedy `lb_greedy/main -a best` :heavy_check_mark:

### O(n log n) Greedy algorithms

Those Greedy are similar to the one described in "A fast algorithm for strongly correlated knapsack problems" (Pisinger, 1998).

- Forward Greedy `lb_greedynlogn/main -a for` :heavy_check_mark:
- Backward Greedy `lb_greedynlogn/main -a back` :heavy_check_mark:
- Best Greedy `lb_greedynlogn/main -a best` :heavy_check_mark:

## Upper bounds

- Dantzig Upper bound `ub_dantzig/main` :heavy_check_mark:
- Surrogate relaxation `ub_surrogate/main` :heavy_check_mark:

## Exact algorithm without pre-processing or sorting

For those algorithms, if required, the Upper bound used is neccessarly `U0`.

- Dynamic programming with Bellman recursion `opt_bellman/main -m array -r all`
  - `-m array -r none` :heavy_check_mark:
  - `-m array -r all` :heavy_check_mark:
  - `-m array -r one` :heavy_check_mark:
  - `-m array -r part` :heavy_check_mark:
  - `-m array -r rec` :heavy_check_mark:
  - `-m list -r none` :heavy_check_mark:
  - `-m list -r all` :x:
  - `-m list -r one` :x:
  - `-m list -r part` :x:
  - `-m list -r rec` :heavy_check_mark:
- Dynamic programming by Profits `opt_dpprofits/main -m array -r all`
  - `-m array -r none` :heavy_check_mark:
  - `-m array -r all` :heavy_check_mark:
  - `-m array -r one` :x:
  - `-m array -r part` :x:
  - `-m array -r rec` :x:
  - `-m list -r none` :x:
  - `-m list -r all` :x:
  - `-m list -r one` :x:
  - `-m list -r part` :x:
  - `-m list -r rec` :x:
- Primal Branch-and-bound `opt_bab/main` :heavy_check_mark:

## Exact algorithms with partial or complete sorting as pre-processing

Primal-dual Dynamic programming and Primal-dual Branch-and-bound allow to sort and reduce items 'by need' instead. This makes them more efficient on small or easy instances compared to other algorithms of the litterature which sort and reduce items in a pre-processing step. Balanced Dynamic programming may require only the partial sorting, however better bounds can be used if items are fully sorted.

- Balanced Dynamic programming `opt_balknap/main -m list -u t -r part` Since the list implementation requires a map, its asymptotical complexity is slightly greater than the one with an array. On the other hand, better Upper bounds can be used. Therefore, both are implemented and with the list implementation, options `-u` can be set to `b` (partial sorting, Dembo Upper bound with break item) or `t` (complete sorting, better Upper Bound)
  - `-m array -r none` :heavy_check_mark:
  - `-m array -r all` :heavy_check_mark:
  - `-m array -r part` :x:
  - `-m list -r none` :heavy_check_mark:
  - `-m list -r all` :heavy_check_mark:
  - `-m list -r part` :x:
- Primal-dual Dynamic programming (`minknap`, `combo`)
  - `-m list -r part` :x:
- Primal-dual Branch-and-bound (`expknap`) :x:

