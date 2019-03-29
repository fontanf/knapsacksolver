WORK IN PROGRESS

# Knapsack

Algorithm implementations for the Knapsack Problem. Most algorithms are detailed in the "Knapsack Problem" book (Kellerer et al., 2004).

This project uses Bazel https://bazel.build/

Compile:
```
bazel build --cxxopt='-std=c++14' --compilation_mode=opt -- //lib:main
```

Generate an instance:
```
bazel build --compilation_mode=opt -- //lib:generator_main
./bazel-bin/lib/generator_main -n 1000 -t sc -r 1000 -o ./ins.txt
```

Solve:
```
./bazel-bin/lib/main -a bellman_array_part -i ins.txt -o out.ini -c sol.txt -v
```

Instances can be visualized with gnuplot:
```
./bazel-bin/lib/generator_main -n 1000 -t sw -r 1000 -o ./ins.txt -p ./ins.plot
gnuplot
gnuplot> set yrange[0:]
gnuplot> set xrange[0:]
gnuplot> plot 'ins.plot' u 1:2
```


## Notes

* If the split item is searched at the beginning, then the Bellman algorithms are more or less dominated by the Primal-dual Dynamic programming algorithms, as well as the Primal Branch-and-bound is more or less dominated by the Primal-dual Branch-and-bound. Therefore, as soon as a pre-processing step requires at leat a partial sorting, it is more consistent to use those algorithms. That's why variable reductions, initial lower bound or better upper bounds are not implemented for the Bellman Dynamic Programming algoorithms, the Dynamic Programming by Profits algorithms or the Primal Branch-and-bound algorithm.

* On the other hand, the Primal-Dual algorithms require to find the split item at the beginning.

* For Dynamic programming algorithms, option `-m` selects the type of memory used. Possible values are (if implemented) `array` or `list`. Lists are slower but eliminate dominated states and allow the use of Upper bounds.

* For Dynamic programming algorithms, option `-r` selects the method used to retrieve the optimal solution. Possible values (if implemented) are
  - `none`: no solution retrieved, only the optimal value is returned
  - `all`: keep all states in memory and backtrack
  - `one`: keep only the last states in memory, retrieve the last item added and run the algorithm again to retrieve the complete optimal solution
  - `part`: keep a partial solution in each state and run the algorithm again while the global solution is not complete
  - `rec`: use the recursive scheme

## Lower bounds

- O(n) Greedy `-a greedy` run Forward and Backward Greedy algorithms and return the best solution :heavy_check_mark:
- O(n log n) Greedy `-a greedynlogn`  run a Complete Greedy (continue filling the knapsack after the break item) and a Forward and Backward Greedy algorithm similar to the one described in "A fast algorithm for strongly correlated knapsack problems" (Pisinger, 1998), and return the best solution found :heavy_check_mark:

## Upper bounds

- Dantzig Upper bound `-a dantzig` :heavy_check_mark:
- Surrogate relaxation Upper bound `-a surrelax` :heavy_check_mark:

## Exact algorithms

### Exact algorithms without pre-processing or sorting

For those algorithms, if required, the Upper bound used is neccessarly `U0`.

- Dynamic programming with Bellman recursion `-a bellman_array` :heavy_check_mark: `-a bellman_array_all` :heavy_check_mark: `-a bellman_array_one` :heavy_check_mark: `-a bellman_array_part` :heavy_check_mark: `-a bellman_array_rec` :heavy_check_mark: `-a bellman_list` :heavy_check_mark: `-a bellman_list_rec` :heavy_check_mark:
- Dynamic programming by Profits `-a dpprofits_array` :heavy_check_mark: `-a dpprofits_array_all` :heavy_check_mark:
- Primal Branch-and-bound `-a bab` :heavy_check_mark:

### Exact algorithms with partial or complete sorting as pre-processing

Primal-dual Dynamic programming and Primal-dual Branch-and-bound allow to sort and reduce items 'by need' instead. This makes them more efficient on small or easy instances compared to other algorithms of the litterature which sort and reduce items in a pre-processing step. Balanced Dynamic programming may require only the partial sorting, however better bounds can be used if items are fully sorted.

Furthermore, some `combo` improvements may here be used with the other algorithms.
- Option `-n`: use `combo` core
- Option `-g X`: `greedynlogn` will be executed at Xth node / if state number goes over X
- Option `-p X`: state pairing with items outside the core will be executed if state number goes over X
- Option `-s X`: surrogate relaxation and instance will be solved at Xth node / if state number goes over X

Algorithms:
- Balanced Dynamic programming `-a balknap -u t`. The list implementation requires a map. Therefore, its asymptotical complexity is slightly greater than the one with an array. However, the possiblity of combining the dynamic programming with bouding makes it more performant. Two versions are still implemented. Options `-u` can be set to `b` (partial sorting, Dembo Upper bound with break item) or `t` (complete sorting, better Upper Bound) :heavy_check_mark: (with options `-n` :x: `-g` :heavy_check_mark: `-s` :heavy_check_mark:)
- Primal-dual Dynamic programming (only with list) (`minknap`, `combo`) :heavy_check_mark: (with options `-n` :heavy_check_mark: `-g` :x: `-p` :x: `-s` :x:)
- Primal-dual Branch-and-bound (`expknap`) `opt_expknap/main` :heavy_check_mark: (with options `-n` :x: `-g` :heavy_check_mark: `-s`  :heavy_check_mark:)

## 

### Dynamic Programming: recursive vs iterative implementation

- ![Recursive implementation](bench/bellmanrec_easy.csv)
- ![Iterative implementation](bench/bellman_array_all_easy.csv)

### Dynamic Programming: cost of retrieving an optimal solution

- ![Only optimal value](bench/bellman_array_easy.csv)
- ![Keeping the whole array](bench/bellman_array_all_easy.csv)
- ![Storing partial solutions in states](bench/bellman_array_part_easy.csv)
- ![Recursive scheme](bench/bellman_array_rec_easy.csv)


