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

- O(n) Greedy `lb_greedy/main` run Forward and Backward Greedy algorithms and return the best solution :heavy_check_mark:
- O(n log n) Greedy `lb_greedynlogn/main`  run a Complete Greedy (continue filling the knapsack after the break item) and a Forward and Backward Greedy algorithm similar to the one described in "A fast algorithm for strongly correlated knapsack problems" (Pisinger, 1998), and return the best solution found :heavy_check_mark:

## Upper bounds

- Dantzig Upper bound `ub_dantzig/main` :heavy_check_mark:
- Surrogate relaxation Upper bound `ub_surrogate/main` :heavy_check_mark:

## Exact algorithms

### Exact algorithms without pre-processing or sorting

For those algorithms, if required, the Upper bound used is neccessarly `U0`.

- Dynamic programming with Bellman recursion `opt_bellman/main -m array -r all`
  - `-m array`:` -r none` :heavy_check_mark: `-r all` :heavy_check_mark: `-r one` :heavy_check_mark: `-r part` :heavy_check_mark: `-r rec` :heavy_check_mark:
  - `-m list`: `-r none` :heavy_check_mark: `-r all` :x: `-r one` :x: `-r part` :x: `-r rec` :heavy_check_mark:
- Dynamic programming by Profits `opt_dpprofits/main -m array -r all`
  - `-m array`: `-r none` :heavy_check_mark: - `-r all` :heavy_check_mark: - `-r one` :x: - `-r part` :x: - `-r rec` :x:
  - `-m list`: `-r none` :x: `-r all` :x: `-r one` :x: `-r part` :x: `-r rec` :x:
- Primal Branch-and-bound `opt_bab/main` :heavy_check_mark:

### Exact algorithms with partial or complete sorting as pre-processing

Primal-dual Dynamic programming and Primal-dual Branch-and-bound allow to sort and reduce items 'by need' instead. This makes them more efficient on small or easy instances compared to other algorithms of the litterature which sort and reduce items in a pre-processing step. Balanced Dynamic programming may require only the partial sorting, however better bounds can be used if items are fully sorted.

Furthermore, some `combo` improvements may here be used with the other algorithms.
- Option `-n`: use `combo` core
- Option `-g X`: `greedynlogn` will be executed at Xth node / if state number goes over X
- Option `-p X`: state pairing with items outside the core will be executed if state number goes over X
- Option `-s X`: surrogate relaxation will be solved at Xth node / if state number goes over X
- Option `-k X`: surrogate instance will be solved at Xth node / if state number goes over X

Algorithms:
- Balanced Dynamic programming `opt_balknap/main -m list -u t -r part` Since the list implementation requires a map, its asymptotical complexity is slightly greater than the one with an array. On the other hand, better Upper bounds can be used. Therefore, both are implemented and with the list implementation, options `-u` can be set to `b` (partial sorting, Dembo Upper bound with break item) or `t` (complete sorting, better Upper Bound)
  - `-m array`: `-r none` :heavy_check_mark: `-r all` :heavy_check_mark: `-r part` :x:
  - `-m list -r none` :heavy_check_mark:
  - `-m list -r all` :heavy_check_mark: `-r part` :heavy_check_mark: (with options `-n` :x: `-g` :heavy_check_mark: `-p` :x: `-s` :heavy_check_mark: `-k` :heavy_check_mark:)
- Primal-dual Dynamic programming (only with list) (`minknap`, `combo`)
  - `-r none` :x: (with options `-n` :x: `-g` :x: `-p` :x: `-s` :x: `-k` :x:)
  - `-r part` :x: (with options `-n` :x: `-g` :x: `-p` :x: `-s` :x: `-k` :x:)

- Primal-dual Branch-and-bound (`expknap`) `opt_expknap/main` :heavy_check_mark: (with options `-n` :x: `-g` :heavy_check_mark: `-s`  :heavy_check_mark: `-k` :heavy_check_mark:)

