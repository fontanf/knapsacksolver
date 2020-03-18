# KnapsackSolver

A solver for the 0-1 Knapsack Problem.

All classical and state-of-the-art algorithms are implemented. Most of them are described in the "Knapsack Problem" book (Kellerer et al., 2004).

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
   * [Dynamic Programming: sequencial vs parallel implementation](#dynamic-programming-sequencial-vs-parallel-implementation)
   * [Normal dataset](#normal-dataset)
   * [Literature dataset](#literature-dataset)

## Usage

### Command line

This project uses Bazel https://bazel.build/

Compile:
```shell
bazel build -- //...
```

Download and uncompress the instances in the `data/` folder:
https://drive.google.com/file/d/1k2Mp3Z5sb5tVJO8I5-WiAglqawk-l1k3/view?usp=sharing

Solve:
```shell
./bazel-bin/knapsacksolver/main -v --algorithm combo --input data/largecoeff/knapPI_3_10000_10000000/knapPI_3_10000_10000000_50.csv --format pisinger
./bazel-bin/knapsacksolver/main -v --algorithm combo --input data/largecoeff/knapPI_5_10000_10000000/knapPI_5_10000_10000000_60.csv --format pisinger
./bazel-bin/knapsacksolver/main -v --algorithm combo --input data/normal/knap_n100000_r100000000_x0.5 --format standard
```

For more options, see:
```
./bazel-bin/knapsacksolver/main --help
```

Run tests:
```
bazel test -- //...
```

### C++ library

* Import this repository into your project. The include directory is the root.
If you use Bazel, just add the following lines to the `WORKSPACE` file:
```
git_repository(
    name = "knapsack",
    remote = "https://github.com/fontanf/knapsacksolver.git",
    commit = "c45720bd8f220d44863d0c8a6a4cc8aa6d5841c8",
)
```
And then in the `BUILD` file, add the dependency to the concerned rule:
```
        deps = [
                "@knapsacksolver//knapsacksolver/algorithms:minknap",
        ],
```
* Then an example of how to create an instance and solve it can be found here:
https://github.com/fontanf/generalizedassignmentsolver/blob/master/generalizedassignmentsolver/algorithms/lagrelax_lbfgs.cpp

## Implemented algorithms

### Lower bounds

- O(n) Greedy, run Forward and Backward Greedy algorithms and return the best solution `-a greedy` :heavy_check_mark:
- O(n log n) Greedy algorithms similar to the one described in "A fast algorithm for strongly correlated knapsack problems" (Pisinger, 1998)
  - Forward O(n log n) Greedy `-a greedynlogn_for` :heavy_check_mark:
  - Backward O(n log n) Greedy `-a greedynlogn_back` :heavy_check_mark:
  - Best between O(n) Greedy and Forward and Backward O(n log n) Greedy algorithms `-a greedynlogn` :heavy_check_mark:

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
  - Top-down (recursive): `-a bellman_rec` :heavy_check_mark:
  - Bottom-up (iterative), array: `-a bellman_array` :heavy_check_mark: `-a bellman_array_all` :heavy_check_mark: `-a bellman_array_one` :heavy_check_mark: `-a bellman_array_part` :heavy_check_mark: `-a bellman_array_rec` :heavy_check_mark:
  - Bottom-up (iterative), lists: `-a bellman_list` :heavy_check_mark: `-a bellman_list_sort` :heavy_check_mark: `-a bellman_list_rec` :heavy_check_mark:
  - Bottom-up (iterative), array, parallel: `-a bellmanpar_array` :heavy_check_mark:
- Dynamic programming by Profits `-a dpprofits_array` :heavy_check_mark: `-a dpprofits_array_all` :heavy_check_mark:
- Primal Branch-and-bound `-a branchandbound` :heavy_check_mark: `-a branchandbound_sort` :heavy_check_mark:

#### State of the art algorithms

Options
- `-c`: use `combo` core
- `-g`: `greedy` will be executed at the beginning of the algorithm
- `-n X`: `greedynlogn` will be executed at Xth node / if state number goes over X
- `-p X`: state pairing with items outside the core will be executed if state number goes over X
- `-s X`: surrogate relaxation and instance will be solved at Xth node / if state number goes over X
- `-k X`: partial solution size (1 <= X <= 64)

Algorithms:
- Primal-dual Branch-and-bound `-a "expknap -c -g -n -1 -s -1"`, `-a expknap_combo` :heavy_check_mark:
- Balanced Dynamic programming. The list implementation requires a map. Therefore, its asymptotical complexity is slightly greater than the one with an array. However, the possiblity of combining the dynamic programming with bouding makes it more performant. Still, two versions are implemented. Options `-u` can be set to `b` (partial sorting, Dembo Upper bound with break item) or `t` (complete sorting, better Upper Bound) `-a "balknap -g -u t -n -1 -s -1 -k 64"` :heavy_check_mark:
- Primal-dual Dynamic programming (only with list) `-a "minknap -c -g -p -1 -s -1 -k 64"`, `-a combo` :heavy_check_mark:

## Results

- Processor: Intel® Core™ i5-8500 CPU @ 3.00GHz × 6
- Time limit: 3000s for each cell.
- Lines correspond to different values of item numbers (50, 100...).
- Column correspond to distribution for the weights and profits (n, u, wc...) and their range (3 means 1000, 4 means 10000...).
- Given a number of items, a range, and a distribution for the weights and profits, a cell contains the mean time (in ms) to solve 100 instances with these parameters and a capacity from 1% to 100% of the sum of all weights of the instance.

Run benchmarks:
```shell
bazel run //knapsacksolver:bench -- -a bellman_array bellman_array_part bellman_array_rec bellmanpar_array bellman_array_all bellman_rec -d easy
bazel run //knapsacksolver:bench -- -a expknap expknap_combo balknap balknap_combo minknap combo -d normal
bazel run //knapsacksolver:bench -- -a expknap expknap_combo balknap minknap combo -d easy difficultlarge difficultsmall
bazel run //knapsacksolver:bench -- -a bellman_list_sort -d difficultsmall
```

Output files are created in `bazel-out/k8-opt/bin/knapsacksolver/bench.runfiles/__main__/`.

### Dynamic Programming: recursive vs iterative implementation

Except for very sparse instances (like SW), the iterative implementation is about 10 times faster.
- ![Recursive implementation](bench/bellman_rec_easy.csv)
- ![Iterative implementation](bench/bellman_array_all_easy.csv)

### Dynamic Programming: cost of retrieving an optimal solution

The recursive scheme is clearly the fastest to retrieve an optimal solution.
It still requires 2 times more time than the implementation returning only the optimal value.
- ![Only optimal value](bench/bellman_array_easy.csv)
- ![Keeping the whole array](bench/bellman_array_all_easy.csv)
- ![Storing partial solutions in states](bench/bellman_array_part_easy.csv)
- ![Recursive scheme](bench/bellman_array_rec_easy.csv)

### Dynamic Programming: sequencial vs parallel implementation

The parallel algorithm is implemented as follows: items are divided in two sets of same size. The all-capacities version of the knapsack problem is solved for both sets with the classical `bellman` recursion. Then, the optimal value is computed by merging the information from both arrays.
- ![Sequencial](bench/bellman_array_easy.csv)
- ![Parallel](bench/bellmanpar_array_easy.csv)

### Normal dataset

```
n ∈ {100, 1000, 10000, 100000, 1000000}
r ∈ {1000, 10000, 100000, 1000000, 10000000, 100000000}
x ∈ {0.0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9}
wj ~ N(r / 2, r  / 10), 1 <= wj <= r
pj ~ N(wj,    wj / 10), 1 <= pj <= r
c = r * (1 - x) + ∑wj * x;
```

* [expknap](https://librallu.gitlab.io/splitted-cell-viz/?u=https://raw.githubusercontent.com/fontanf/knapsack/master/bench/expknap.json)
* [expknap_combo](https://librallu.gitlab.io/splitted-cell-viz/?u=https://raw.githubusercontent.com/fontanf/knapsack/master/bench/expknap_combo.json)
* [balknap](https://librallu.gitlab.io/splitted-cell-viz/?u=https://raw.githubusercontent.com/fontanf/knapsack/master/bench/balknap.json)
* [balknap_combo](https://librallu.gitlab.io/splitted-cell-viz/?u=https://raw.githubusercontent.com/fontanf/knapsack/master/bench/balknap_combo.json)
* [minknap](https://librallu.gitlab.io/splitted-cell-viz/?u=https://raw.githubusercontent.com/fontanf/knapsack/master/bench/minknap.json)
* [combo](https://librallu.gitlab.io/splitted-cell-viz/?u=https://raw.githubusercontent.com/fontanf/knapsack/master/bench/combo.json)

Remarks:
- The superiority of `minknap` is clear
- `combo` optimizations do not help:

### Literature dataset

| Algorithm       | Instances                                                                                                                                                        |
| --------------- | ---------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| `expknap`       | ![easy](bench/expknap_easy.csv),        ![difficult large](bench/expknap_difficultlarge.csv),        ![difficult small](bench/expknap_difficultsmall.csv)        |
| `expknap_combo` | ![easy](bench/expknap_combo_easy.csv),  ![difficult large](bench/expknap_combo_difficultlarge.csv),  ![difficult small](bench/expknap_combo_difficultsmall.csv)  |
| `balknap`       | ![easy](bench/balknap_easy.csv),        ![difficult large](bench/balknap_difficultlarge.csv),        ![difficult small](bench/balknap_difficultsmall.csv)        |
| `minknap`       | ![easy](bench/minknap_easy.csv),        ![difficult large](bench/minknap_difficultlarge.csv),        ![difficult small](bench/minknap_difficultsmall.csv)        |
| `combo`         | ![easy](bench/combo_easy.csv),          ![difficult large](bench/combo_difficultlarge.csv),          ![difficult small](bench/combo_difficultsmall.csv)          |

Remarks:
- These subset sum, strongly correlated and inverse strongly correlated instances are easy to solve since the upper bound happens to always be optimal.
- With `combo` optimizations, `expknap` is able to solve strongly correlated and inverse strongly correlated instances. It also solves more almost strongly correlated instances.
- Performances of `minknap` and `combo` seems to match the results from the literature. Furthermore, taking advantage of parallelization, this implementation of `combo` is able to solve all instances of the `difficultlarge` dataset.
- `balknap` is very competitive on small instances (R = 1000). Compared to `minknap`:
  - it performs a lot better on `sc`, `isc`, `spanner`, `mstr` and `pceil` instances.
  - it performs slighlty worse on `u`, `wc` and `ss` instances.
  - it performs a lot worse on `asc` and `circle` instances.

