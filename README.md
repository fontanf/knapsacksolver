# Knapsack Solver

A solver for the 0-1 Knapsack Problem.

All classical and state-of-the-art algorithms are implemented. Most of them are described in the "Knapsack Problem" book (Kellerer et al., 2004).

![knapsack](knapsack.png?raw=true "knapsack")

[image source](https://commons.wikimedia.org/wiki/File:Knapsack.svg)

##### Table of Contents

 * [Usage](#usage)
   * [Command line](#command-line)
   * [C++ library](#c-library)
   * [Python interface](#python-interface)
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
```
```
=====================================
           Knapsack Solver           
=====================================

Instance
--------
Number of items:  10000
Capacity:         24552508933

Algorithm
---------
Minknap

Parameters
----------
Greedy:                 1
Pairing:                10000
Surrogate relaxation:   2000
Combo core:             1
Partial solution size:  64

    T (ms)             LB             UB       GAP                 Comment
    ------             --             --       ---                 -------
         0              0            inf       inf                        
       0.3    31604508753            inf       inf        initial solution
       0.3    31604508753    31605222696    713943     dantzig upper bound
       1.9    31604508786    31605222696    713910              it 18 (lb)
       1.9    31604508806    31605222696    713890              it 18 (lb)
       1.9    31604508812    31605222696    713884              it 18 (lb)
       1.9    31604508819    31605222696    713877              it 18 (lb)
       1.9    31604508824    31605222696    713872              it 18 (lb)
       1.9    31604508833    31605222696    713863              it 18 (lb)
       1.9    31604508933    31605222696    713763              it 18 (lb)
       6.8    31604508933    31604508933         0    surrogate relaxation

Final statistics
----------------
Value:                      31604508933
Bound:                      31604508933
Gap:                        0
Gap (%):                    0
Solution:                   OK
Time (ms):                  11.8
Number of recursive calls:  2
```

```shell
./bazel-bin/knapsacksolver/main -v --algorithm combo --input data/largecoeff/knapPI_5_10000_10000000/knapPI_5_10000_10000000_60.csv --format pisinger
```
```
=====================================
           Knapsack Solver           
=====================================

Instance
--------
Number of items:  10000
Capacity:         29740614219

Algorithm
---------
Minknap

Parameters
----------
Greedy:                 1
Pairing:                10000
Surrogate relaxation:   2000
Combo core:             1
Partial solution size:  64

    T (ms)             LB             UB       GAP                 Comment
    ------             --             --       ---                 -------
         0              0            inf       inf                        
       0.4    37438591403            inf       inf        initial solution
       0.4    37438591403    37439459371    867968     dantzig upper bound
       0.4    37438593109    37439459371    866262              it 10 (lb)
       1.2    37438593616    37439459371    865755              it 16 (lb)
       1.2    37438593668    37439459371    865703              it 16 (lb)
       1.7    37438594077    37439459371    865294              it 18 (lb)
       1.7    37438594129    37439459371    865242              it 18 (lb)
       2.7    37438594909    37439459371    864462              it 20 (lb)
       2.7    37438595411    37439459371    863960              it 20 (lb)
       2.7    37438596238    37439459371    863133              it 20 (lb)
       2.8    37438597409    37439459371    861962              it 20 (lb)
       2.8    37438598429    37439459371    860942              it 20 (lb)
       2.8    37438601240    37439459371    858131              it 20 (lb)
       2.8    37438602260    37439459371    857111              it 20 (lb)
       2.8    37438603431    37439459371    855940              it 20 (lb)
       2.8    37438604258    37439459371    855113              it 20 (lb)
       2.8    37438611923    37439459371    847448              it 20 (lb)
       2.8    37438612943    37439459371    846428              it 20 (lb)
       2.8    37438614114    37439459371    845257              it 20 (lb)
       2.8    37438618772    37439459371    840599              it 20 (lb)
       2.8    37438619943    37439459371    839428              it 20 (lb)
       4.1    37438624079    37439459371    835292              it 21 (lb)
       4.1    37438624581    37439459371    834790              it 21 (lb)
       4.1    37438625752    37439459371    833619              it 21 (lb)
       4.1    37438626772    37439459371    832599              it 21 (lb)
       4.1    37438632601    37439459371    826770              it 21 (lb)
       4.2    37438643284    37439459371    816087              it 21 (lb)
      10.1    37438643342    37439459371    816029              it 25 (lb)
      10.1    37438644672    37439459371    814699              it 25 (lb)
      10.1    37438647021    37439459371    812350              it 25 (lb)
      12.4    37438647021    37439419666    772645    surrogate relaxation
      13.5    37439041506    37439419666    378160  surrogate ins res (lb)
      13.9    37439416751    37439419666      2915  surrogate ins res (lb)
      13.9    37439416751    37439416751         0  surrogate ins res (ub)

Final statistics
----------------
Value:                      37439416751
Bound:                      37439416751
Gap:                        0
Gap (%):                    0
Solution:                   OK
Time (ms):                  18
Number of recursive calls:  1
```

```shell
./bazel-bin/knapsacksolver/main -v 1 --algorithm combo --input data/knapsack/normal/knap_n100000_r100000000_x0.5 --format standard
```
```
=====================================
           Knapsack Solver           
=====================================

Instance
--------
Number of items:  100000
Capacity:         2498481766944

Algorithm
---------
Minknap

Parameters
----------
Greedy:                 1
Pairing:                10000
Surrogate relaxation:   2000
Combo core:             1
Partial solution size:  64

    T (ms)             LB             UB       GAP                 Comment
    ------             --             --       ---                 -------
       0.3              0            inf       inf                        
       5.5  2698884873111            inf       inf        initial solution
       5.5  2698884873111  2698885010148    137037     dantzig upper bound
      12.1  2698884993895  2698885010148     16253              it 16 (lb)
      16.6  2698885006118  2698885010148      4030              it 20 (lb)
      33.7  2698885006118  2698885006118         0   tree search completed

Final statistics
----------------
Value:                      2698885006118
Bound:                      2698885006118
Gap:                        0
Gap (%):                    0
Solution:                   OK
Time (ms):                  34.5
Number of recursive calls:  2
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

### Python interface

The Python library is generated at `bazel-bin/python/knapsacksolver.so` by the following command:
```shell
bazel build -- //python:knapsacksolver.so
```

Usage:
```python
import random
import knapsacksolver

# Create instance
instance = knapsacksolver.Instance()
n = 100
c = 0
for i in range(n):
    w = random.randint(1, 1000000)
    c += w
    instance.add_item(w, w + 10)

instance.set_capacity(c // 2)

# Solve
solution = knapsacksolver.solve(instance)
# or solution = knapsacksolver.solve(instance, algorithm = "minknap", verbose = True)
solution.number_of_items()
solution.profit()
solution.contains(0)
```

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

