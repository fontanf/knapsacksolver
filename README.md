# Knapsack

Classical and state of the art algorithm implementations for the Knapsack Problem. Most algorithms are detailed in the "Knapsack Problem" book (Kellerer et al., 2004).

Note: the code of `minknap` with all its optimizations is rather complex. I tried to test it to the best of my ability, however, some bugs might remain. If you wish to use it, do not hesitate to compare the returned values with a simpler algorithm like `opt_bellman_array` during development phase. If you find an instance (preferably a small one) for which two exact algorithms do not return the same value, please open an issue or contact me.

![knapsack](knapsack.png?raw=true "knapsack")

[image source](https://commons.wikimedia.org/wiki/File:Knapsack.svg)

##### Table of Contents

 * [Usage](#usage)
   * [Command line](#command-line)
   * [C++ library](#c-library)
 * [Implemented algorithms](#implemented-algorithms)
   * [Lower bounds](#lower-bounds)
   * [Upper bounds](#upper-bounds)
   * [Exact algorithms](#exact-algorithms)
     * [Classical algorithms](#classical-algorithms)
     * [State of the art algorithms](#state-of-the-art-algorithms)
 * [Results](#results)
   * [Dynamic Programming: recursive vs iterative implementation](#dynamic-programming-recursive-vs-iterative-implementation)
   * [Dynamic Programming: cost of retrieving an optimal solution](#dynamic-programming-cost-of-retrieving-an-optimal-solution)
   * [When cache makes parallel algorithms slower](#when-cache-makes-parallel-algorithms-slower)
   * [Normal dataset](#normal-dataset)
   * [Literature dataset](#literature-dataset)

## Usage

### Command line

This project uses Bazel https://bazel.build/

Compile:
```
bazel build -- //...
```

Generate an instance:
```
bazel build -- //lib:generator_main
./bazel-bin/lib/generator_main -n 1000 -t sc -r 1000 -o ./ins.txt
```

Examples:
- `-n 100 -t u/wc/sc/isc/asc/ss/sw -r 1000`
- `-n 100 --spanner -t u/wc/sc/isc/asc/ss/sw -r 1000 -v 2 -m 10`
- `-n 100 -t mstr -r 1000 -ka 300 -kb 200 -d 6`
- `-n 100 -t pceil -r 1000 -d 3`
- `-n 100 -t circle -r 1000 -d 0.66`

Solve:
```
./bazel-bin/lib/main -a bellman_array_part -i ins.txt -o out.ini -c sol.txt -v
```

Instances can be visualized with gnuplot:
```
./bazel-bin/lib/generator_main -n 1000 -t sw -r 1000 -o ./ins.txt -p ./ins.plot
gnuplot
gnuplot> set yrange[0:]; set xrange[0:]; plot 'ins.plot' u 1:2
```

Unit tests:
```
bazel test -- //...
```

### C++ library

* Import this repository into your project. The include directory is the root.
If you use Bazel, just add the following lines to the `WORKSPACE` file:
```
git_repository(
    name = "knapsack",
    remote = "https://github.com/fontanf/knapsack.git",
    commit = "c45720bd8f220d44863d0c8a6a4cc8aa6d5841c8",
)
```
And then in the `BUILD` file, add the dependency to the concerned rule:
```
        deps = [
                "@knapsack//opt_minknap:minknap",
        ],
```
* Then an example of how to create an instance and solve it can be found here:
https://github.com/fontanf/gap/blob/master/lb_lagrelax_lbfgs/lagrelax_lbfgs.cpp

## Implemented algorithms

### Lower bounds

- O(n) Greedy, run Forward and Backward Greedy algorithms and return the best solution `-a greedy` eavy_check_mark:
- O(n log n) Greedy algorithms similar to the one described in "A fast algorithm for strongly correlated knapsack problems" (Pisinger, 1998)
  - Forward O(n log n) Greedy `-a greedynlogn_for`
  - Backward O(n log n) Greedy `-a greedynlogn_back`
  - Best between O(n) Greedy and Forward and Backward O(n log n) Greedy algorithms `-a greedynlogn`

### Upper bounds

- Dantzig Upper bound `-a dantzig` :heavy_check_mark:
- Surrogate relaxation Upper bound
  - only solve relaxation: `-a surrelax` :heavy_check_mark:
  - solve relaxation and surrogate instance with `minknap`: `-a surrelax_minknap` :heavy_check_mark:


### Exact algorithms

#### Classical algorithms

- `array` and `list` refer to the type of memory used. Lists are slower but eliminate dominated states and allow the use of Upper bounds.
- `all`, `one`, `part` and `rec` refer to the method used to retrieve the optimal solution:
  - none: no solution retrieved, only the optimal value is returned
  - `all`: keep all states in memory and backtrack
  - `one`: keep only the last states in memory, retrieve the last item added and run the algorithm again to retrieve the complete optimal solution
  - `part`: keep a partial solution in each state and run the algorithm again while the global solution is not complete. `k` indicates the size of the partial solution (<=64).
  - `rec`: use the recursive scheme
- `sort` indicates that the items are fully sorted at the beginning, allowing the use of better Upper bounds.

Algorithms:
- Dynamic programming with Bellman recursion
  - Top-down (recursive): `-a bellmanrec_array` :heavy_check_mark:
  - Bottom-up (iterative), array: `-a bellman_array` :heavy_check_mark: `-a bellman_array_all` :heavy_check_mark: `-a bellman_array_one` :heavy_check_mark: `-a bellman_array_part` :heavy_check_mark: `-a bellman_array_rec` :heavy_check_mark:
  - Bottom-up (iterative), lists: `-a bellman_list` :heavy_check_mark: `-a bellman_list_sort` :heavy_check_mark: `-a bellman_list_rec` :heavy_check_mark:
  - Bottom-up (iterative), array, parallel: `-a bellmanpar_array` :heavy_check_mark:
- Dynamic programming by Profits `-a dpprofits_array` :heavy_check_mark: `-a dpprofits_array_all` :heavy_check_mark:
- Primal Branch-and-bound `-a bab` :heavy_check_mark: `-a bab_sort` :heavy_check_mark:

#### State of the art algorithms

Options
- `c 1`: use `combo` core
- `g X`: `greedy` will be executed at Xth node / if state number goes over X
- `gn X`: `greedynlogn` will be executed at Xth node / if state number goes over X
- `p X`: state pairing with items outside the core will be executed if state number goes over X
- `s X`: surrogate relaxation and instance will be solved at Xth node / if state number goes over X

Algorithms:
- Primal-dual Branch-and-bound `-a "expknap g 0 gn -1 s -1 c false"`, `-a expknap_combo` :heavy_check_mark:
- Balanced Dynamic programming. The list implementation requires a map. Therefore, its asymptotical complexity is slightly greater than the one with an array. However, the possiblity of combining the dynamic programming with bouding makes it more performant. Still, two versions are implemented. Options `u` can be set to `b` (partial sorting, Dembo Upper bound with break item) or `t` (complete sorting, better Upper Bound) `-a "balknap u t k 64 g 0 gn -1 s -1"` :heavy_check_mark:
- Primal-dual Dynamic programming (only with list) `-a "minknap k 64 g 0 p -1 s -1 c false"`, `-a combo` :heavy_check_mark:

## Results

- Processor: Intel® Core™ i5-8500 CPU @ 3.00GHz × 6
- Time limit: 3000s for each cell.
- Lines correspond to different values of item numbers (50, 100...).
- Column correspond to distribution for the weights and profits (n, u, wc...) and their range (3 means 1000, 4 means 10000...).
- Given a number of items, a range, and a distribution for the weights and profits, a cell contains the mean time (in ms) to solve 100 instances with these parameters and a capacity from 1% to 100% of the sum of all weights of the instance.

Bench:
```
bazel run //lib:bench -- -a minknap balknap -d normal easy difficultsmall difficultlarge
```

### Dynamic Programming: recursive vs iterative implementation

Except for very sparse instances (like SW), the iterative implementation is about 10 times faster.
- ![Recursive implementation](bench/bellmanrec_easy.csv)
- ![Iterative implementation](bench/bellman_array_all_easy.csv)

### Dynamic Programming: cost of retrieving an optimal solution

The recursive scheme is clearly the fastest to retrieve an optimal solution.
It still requires 2 times more time than the implementation returning only the optimal value.
- ![Only optimal value](bench/bellman_array_easy.csv)
- ![Keeping the whole array](bench/bellman_array_all_easy.csv)
- ![Storing partial solutions in states](bench/bellman_array_part_easy.csv)
- ![Recursive scheme](bench/bellman_array_rec_easy.csv)

### When cache makes parallel algorithms slower

The parallel algorithm is implemented as follows: items are divided in two sets of same size. The all-capacities version of the knapsack problem is solved for both sets with the classical `bellman` recursion. Then, the optimal value is computed by merging the information from both arrays.
- ![Sequencial](bench/bellman_array_easy.csv)
- ![Parallel](bench/bellmanpar_array_easy.csv)

### Normal dataset

```
n ∈ {100, 1000, 10000, 100000, 1000000}
r ∈ {1000, 10000, 100000, 1000000, 10000000, 100000000}
x ∈ {0.0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9}
wj ~ N(r / 2, r / 10)
pj ~ N(wj, wj / 10), 1 <= pj <= r
c = r * (1 - x) + ∑wj * x;
```

Each instance (except one) of the normal dataset happens to be solved exactly by `minknap` in less than a second:
* [bellman_list_rec](https://librallu.gitlab.io/splitted-cell-viz/?u=https://raw.githubusercontent.com/fontanf/knapsack/master/bench/bellman_list_rec.json)
* [expknap](https://librallu.gitlab.io/splitted-cell-viz/?u=https://raw.githubusercontent.com/fontanf/knapsack/master/bench/expknap.json)
* [expknap_combo](https://librallu.gitlab.io/splitted-cell-viz/?u=https://raw.githubusercontent.com/fontanf/knapsack/master/bench/expknap_combo.json)
* [balknap](https://librallu.gitlab.io/splitted-cell-viz/?u=https://raw.githubusercontent.com/fontanf/knapsack/master/bench/balknap.json)
* [balknap_combo](https://librallu.gitlab.io/splitted-cell-viz/?u=https://raw.githubusercontent.com/fontanf/knapsack/master/bench/balknap_combo.json)
* [minknap](https://librallu.gitlab.io/splitted-cell-viz/?u=https://raw.githubusercontent.com/fontanf/knapsack/master/bench/minknap.json)
* [combo](https://librallu.gitlab.io/splitted-cell-viz/?u=https://raw.githubusercontent.com/fontanf/knapsack/master/bench/combo.json)

### Literature dataset

| Algorithm                               | Instances                                                                                                                                                        |
| --------------------------------------- | ---------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| `expknap`                               | ![easy](bench/expknap_easy.csv),        ![difficult large](bench/expknap_difficultlarge.csv),        ![difficult small](bench/expknap_difficultsmall.csv)        |
| `expknap_combo`                         | ![easy](bench/expknap_combo_easy.csv),  ![difficult large](bench/expknap_combo_difficultlarge.csv),  ![difficult small](bench/expknap_combo_difficultsmall.csv)  |
| `balknap`                               | ![easy](bench/balknap_t_easy.csv),      ![difficult large](bench/balknap_t_difficultlarge.csv),      ![difficult small](bench/balknap_t_difficultsmall.csv)      |
| `minknap`                               | ![easy](bench/minknap_easy.csv),        ![difficult large](bench/minknap_difficultlarge.csv),        ![difficult small](bench/minknap_difficultsmall.csv)        |
| `combo`                                 | ![easy](bench/combo_easy.csv),          ![difficult large](bench/combo_difficultlarge.csv),          ![difficult small](bench/combo_difficultsmall.csv)          |

Remarks:
- Spanner instances are among the worst cases of the `minknap` recursion, since many items of the break solution won't be in an optimal solution. It is interesting to note that the `bellman` recursion performs better ![on those instances](bench/bellman_list_sort_difficultsmall.csv). However, the worst case of the `bellman` recursion is worse than the worst case of the `minknap` recursion.
- These subset sum, strongly correlated and inverse strongly correlated instances are easy to solve since the upper bound happens to always be optimal.
- With `combo` optimizations, `expknap` is able to solve strongly correlated and inverse strongly correlated instances. It also solves more almost strongly correlated instances.
- Performances of `minknap` and `combo` seems to match the results from the literature. Furthermore, taking advantage of parallelization, this implementation of `combo` is able to solve all instances of the `difficultlarge` dataset.
- `balknap` is very competitive on small instances (R = 1000). Compared to `minknap`:
  - it performs a lot better on `sc`, `isc`, `spanner`, `mstr` and `pceil` instances.
  - it performs slighlty worse on `u`, `wc` and `ss` instances.
  - it performs a lot worse on `asc` and `circle` instances.

