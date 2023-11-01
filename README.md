# KnapsackSolver

A solver for the 0-1 knapsack problem.

All classical and state-of-the-art algorithms are implemented. Most of them are described in the "Knapsack Problem" book (Kellerer et al., 2004).

![knapsack](knapsack.png?raw=true "knapsack")

[image source](https://commons.wikimedia.org/wiki/File:Knapsack.svg)

##### Table of Contents

 * [Implemented algorithms](#implemented-algorithms)
 * [Usage](#usage)
   * [Command line](#command-line)
   * [C++ library](#c-library)
   * [Python interface](#python-interface)
 * [Results](#results)
   * [Dynamic Programming: recursive vs iterative implementation](#dynamic-programming-recursive-vs-iterative-implementation)
   * [Dynamic Programming: cost of retrieving an optimal solution](#dynamic-programming-cost-of-retrieving-an-optimal-solution)
   * [Dynamic Programming: sequencial vs parallel implementation](#dynamic-programming-sequencial-vs-parallel-implementation)
   * [Normal dataset](#normal-dataset)
   * [Literature dataset](#literature-dataset)

## Implemented algorithms

### Knapsack

* Greedy
  * `O(n)` `-a greedy`
  * `O(n log n)`
    * Forward `-a greedy-nlogn-forward`
    * Backward `-a greedy-nlogn-backward`
    * Best of Forward and Backward `-a greedy-nlogn`

* Upper bounds
  * Dantzig upper bound `-a dantzig`
  * Surrogate relaxation
    * Only solve relaxation `-a surrogate-relaxation`
    * Solve relaxation and surrogate instance with `dynamic-programming-primal-dual`: `-a solve-surrogate-instance`

* Dynamic Programming
  * Bellman
    * Recursive `-a dynamic-programming-bellman-rec`
    * Array (only optimal value) `-a dynamic-programming-bellman-array`
    * Array + parallel (only optimal value) `-a dynamic-programming-bellman-array-parallel`
    * Array (all) `-a dynamic-programming-bellman-array-all`
    * Array (one) `-a dynamic-programming-bellman-array-one`
    * Array (partial solution) `-a dynamic-programming-bellman-array-part`
    * Array (recursive scheme) `-a dynamic-programming-bellman-array-rec`
    * List (only optimal value) `-a dynamic-programming-bellman-list`
    * List + sort (only optimal value) `-a dynamic-programming-bellman-list-sort`
    * List (recursive scheme) `-a dynamic-programming-bellman-list-rec`
  * By profits
    * Array (only optimal value) `-a dynamic-programming-profits-array`
    * Array (all) `-a dynamic-programming-profits-array-all`
  * Balancing (balknap)
    * List (partial solution) `-a "dynamic-programming-balancing -g -u t -n -1 -s -1 -k 64"`, `-a balknap`
  * Primal-dual (minknap, combo)
    * List (partial solution) `-a "dynamic-programming-primal-dual -c -g -p -1 -s -1 -k 64"`, `-a dynamic-programming-primal-dual-combo`, `-a combo`

* Branch-and-bound
  * Primal
    * `-a branch-and-bound`
    * `-a branch-and-bound-sort`
  * Primal-dual (expknap) `-a "branch-and-bound-primal-dual -c -g -n -1 -s -1"`,  `expknap`, `-a branch-and-bound-primal-dual-combo`

### Subset sum

* Dynamic programming
  * Bellman
    * Array `-a dynamic-programming-bellman-array`
    * List (only optimal value) `-a dynamic-programming-bellman-list`
    * Word RAM (only optimal value) `-a dynamic-programming-bellman-word-ram`
    * Word RAM with recursive scheme `-a dynamic-programming-bellman-word-ram-rec`
  * Balancing
    * Array (only optimal value) `-a dynamic-programming-balancing-array`

### Multiple-choice subset sum

* Dynamic programming
  * Bellman
    * Array `-a dynamic-programming-bellman-array`
    * Word RAM (only optimal value) `-a dynamic-programming-bellman-word-ram`
    * Word RAM with recursive scheme `-a dynamic-programming-bellman-word-ram-rec`

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
./bazel-bin/knapsacksolver/knapsack/main -v 1 --algorithm combo --input data/knapsack/largecoeff/knapPI_3_10000_10000000/knapPI_3_10000_10000000_50.csv --format pisinger
```
```
====================================
           KnapsackSolver           
====================================

Problem
--------
Knapsack problem

Instance
--------
Number of items:  10000
Capacity:         24552508933

Algorithm
---------
Dynamic programming - Primal-dual

Parameters
----------
Implementation:                  States
Method for retrieving solution:  Partial solution in states
Greedy:                          1
Pairing:                         10000
Surrogate relaxation:            2000
Combo core:                      1
Partial solution size:           64

    T (ms)             LB             UB       GAP                 Comment
    ------             --             --       ---                 -------
         0              0            inf       inf                        
       0.4    31604508753            inf       inf        initial solution
       0.4    31604508753    31605222696    713943     dantzig upper bound
       1.7    31604508786    31605222696    713910              it 18 (lb)
       1.7    31604508806    31605222696    713890              it 18 (lb)
       1.7    31604508812    31605222696    713884              it 18 (lb)
       1.7    31604508819    31605222696    713877              it 18 (lb)
       1.7    31604508824    31605222696    713872              it 18 (lb)
       1.7    31604508833    31605222696    713863              it 18 (lb)
       1.7    31604508933    31605222696    713763              it 18 (lb)
       6.6    31604508933    31604508933         0    surrogate relaxation

Final statistics
----------------
Value:                      31604508933
Bound:                      31604508933
Gap:                        0
Gap (%):                    0
Solution:                   1
Number of items:            7052 / 10000 (70.52%)
Weight:                     24552508933 / 24552508933 (100%)
Time (ms):                  7.4
Number of recursive calls:  2
```

```shell
./bazel-bin/knapsacksolver/knapsack/main -v 1 --algorithm combo --input data/knapsack/largecoeff/knapPI_5_10000_10000000/knapPI_5_10000_10000000_60.csv --format pisinger
```
```
====================================
           KnapsackSolver           
====================================

Problem
--------
Knapsack problem

Instance
--------
Number of items:  10000
Capacity:         29740614219

Algorithm
---------
Dynamic programming - Primal-dual

Parameters
----------
Implementation:                  States
Method for retrieving solution:  Partial solution in states
Greedy:                          1
Pairing:                         10000
Surrogate relaxation:            2000
Combo core:                      1
Partial solution size:           64

    T (ms)             LB             UB       GAP                 Comment
    ------             --             --       ---                 -------
         0              0            inf       inf                        
       0.5    37438591403            inf       inf        initial solution
       0.5    37438591403    37439459371    867968     dantzig upper bound
       0.5    37438593109    37439459371    866262              it 10 (lb)
         1    37438593616    37439459371    865755              it 16 (lb)
         1    37438593668    37439459371    865703              it 16 (lb)
       1.6    37438594077    37439459371    865294              it 18 (lb)
       1.6    37438594129    37439459371    865242              it 18 (lb)
       2.8    37438594909    37439459371    864462              it 20 (lb)
       2.8    37438595411    37439459371    863960              it 20 (lb)
       2.8    37438596238    37439459371    863133              it 20 (lb)
       2.8    37438597409    37439459371    861962              it 20 (lb)
       2.8    37438598429    37439459371    860942              it 20 (lb)
       2.8    37438601240    37439459371    858131              it 20 (lb)
       2.8    37438602260    37439459371    857111              it 20 (lb)
       2.8    37438603431    37439459371    855940              it 20 (lb)
       2.8    37438604258    37439459371    855113              it 20 (lb)
       2.8    37438611923    37439459371    847448              it 20 (lb)
       2.9    37438612943    37439459371    846428              it 20 (lb)
       2.9    37438614114    37439459371    845257              it 20 (lb)
       2.9    37438618772    37439459371    840599              it 20 (lb)
       2.9    37438619943    37439459371    839428              it 20 (lb)
       4.1    37438624079    37439459371    835292              it 21 (lb)
       4.1    37438624581    37439459371    834790              it 21 (lb)
       4.1    37438625752    37439459371    833619              it 21 (lb)
       4.1    37438626772    37439459371    832599              it 21 (lb)
       4.1    37438632601    37439459371    826770              it 21 (lb)
       4.1    37438643284    37439459371    816087              it 21 (lb)
      10.7    37438643342    37439459371    816029              it 25 (lb)
      10.7    37438644672    37439459371    814699              it 25 (lb)
      10.7    37438647021    37439459371    812350              it 25 (lb)
      12.2    37438647021    37439419666    772645    surrogate relaxation
      13.5    37439041506    37439419666    378160  surrogate ins res (lb)
      13.9    37439416751    37439419666      2915  surrogate ins res (lb)
        14    37439416751    37439416751         0  surrogate ins res (ub)

Final statistics
----------------
Value:                      37439416751
Bound:                      37439416751
Gap:                        0
Gap (%):                    0
Solution:                   1
Number of items:            7699 / 10000 (76.99%)
Weight:                     29740614093 / 29740614219 (100%)
Time (ms):                  16.8
Number of recursive calls:  1
```

```shell
./bazel-bin/knapsacksolver/knapsack/main -v 1 --algorithm combo --input data/knapsack/normal/knap_n100000_r100000000_x0.5 --format standard
```
```
====================================
           KnapsackSolver           
====================================

Problem
--------
Knapsack problem

Instance
--------
Number of items:  100000
Capacity:         2498481766944

Algorithm
---------
Dynamic programming - Primal-dual

Parameters
----------
Implementation:                  States
Method for retrieving solution:  Partial solution in states
Greedy:                          1
Pairing:                         10000
Surrogate relaxation:            2000
Combo core:                      1
Partial solution size:           64

    T (ms)             LB             UB       GAP                 Comment
    ------             --             --       ---                 -------
         0              0            inf       inf                        
       5.2  2698884873111            inf       inf        initial solution
       5.2  2698884873111  2698885010148    137037     dantzig upper bound
       7.8  2698884993895  2698885010148     16253              it 16 (lb)
      12.9  2698885006118  2698885010148      4030              it 20 (lb)
      30.3  2698885006118  2698885006118         0   tree search completed

Final statistics
----------------
Value:                      2698885006118
Bound:                      2698885006118
Gap:                        0
Gap (%):                    0
Solution:                   1
Number of items:            49971 / 100000 (49.971%)
Weight:                     2498481766106 / 2498481766944 (100%)
Time (ms):                  31.5
Number of recursive calls:  2
```

```shell
./bazel-bin/knapsacksolver/subsetsum/main -v 1 -i data/subsetsum/pthree/pthree_1000_1 -a dynamic-programming-bellman-word-ram-rec
```
```
====================================
           KnapsackSolver           
====================================

Problem
--------
Subset sum problem

Instance
--------
Number of items:  1000
Capacity:         250000

Algorithm
---------
Dynamic programming - Bellman

Parameters
----------
Implementation:                  Word RAM
Method for retrieving solution:  Recursive scheme

     T (s)            UB            LB           GAP   GAP (%)                 Comment
     -----            --            --           ---   -------                 -------
     0.000             0        250000        250000   -100.00                        
     0.036        250000        250000             0      0.00   tree search completed

Final statistics
----------------
Value:                        250000 / 250000 (100%)
Has solution:                 1
Bound:                        250000
Absolute optimality gap:      0
Relative optimality gap (%):  0
Number of items:              484 / 1000 (48.4%)
Time (s):                     0.0355509

Solution
--------
Number of items:  484 / 1000 (48.4%)
Weight:           250000 / 250000 (100%)
Feasible:         1
```

For more options, see:
```
./bazel-bin/knapsacksolver/knapsack/main --help
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
    name = "knapsacksolver",
    remote = "https://github.com/fontanf/knapsacksolver.git",
    commit = "c45720bd8f220d44863d0c8a6a4cc8aa6d5841c8",
)
```
And then in the `BUILD` file, add the dependency to the concerned rule:
```
        deps = [
                "@knapsacksolver//knapsacksolver/algorithms:dynamic_programming_primal_dual",
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
# or solution = knapsacksolver.solve(instance, algorithm = "dynamic-programming-primal-dual", verbose = True)
solution.number_of_items()
solution.profit()
solution.contains(0)
```

## Results

- Processor: Intel® Core™ i5-8500 CPU @ 3.00GHz × 6
- Time limit: 3000s for each cell.
- Lines correspond to different values of item numbers (50, 100...).
- Column correspond to distribution for the weights and profits (n, u, wc...) and their range (3 means 1000, 4 means 10000...).
- Given a number of items, a range, and a distribution for the weights and profits, a cell contains the mean time (in ms) to solve 100 instances with these parameters and a capacity from 1% to 100% of the sum of all weights of the instance.

Run benchmarks:
```shell
bazel run //knapsacksolver:bench -- -a dynamic-programming-bellman-array dynamic-programming-bellman-array-part dynamic-programming-bellman-array-rec bellmanpar-array dynamic-programming-bellman-array-all dynamic-programming-bellman-rec -d easy
bazel run //knapsacksolver:bench -- -a branch-and-bound-primal-dual branch-and-bound-primal-dual-combo dynamic-programming-balancing dynamic-programming-balancing-combo dynamic-programming-primal-dual dynamic-programming-primal-dual-combo -d normal
bazel run //knapsacksolver:bench -- -a branch-and-bound-primal-dual branch-and-bound-primal-dual-combo dynamic-programming-balancing dynamic-programming-primal-dual dynamic-programming-primal-dual-combo -d easy difficultlarge difficultsmall
bazel run //knapsacksolver:bench -- -a dynamic-programming-bellman-list-sort -d difficultsmall
```

Output files are created in `bazel-out/k8-opt/bin/knapsacksolver/bench.runfiles/__main__/`.

### Dynamic Programming: recursive vs iterative implementation

Except for very sparse instances (like SW), the iterative implementation is about 10 times faster.
- ![Recursive implementation](bench/dynamic_programming_bellman_rec_easy.csv)
- ![Iterative implementation](bench/dynamic_programming_bellman_array_all_easy.csv)

### Dynamic Programming: cost of retrieving an optimal solution

The recursive scheme is clearly the fastest to retrieve an optimal solution.
It still requires 2 times more time than the implementation returning only the optimal value.
- ![Only optimal value](bench/dynamic_programming_bellman_array_easy.csv)
- ![Keeping the whole array](bench/dynamic_programming_bellman_array_all_easy.csv)
- ![Storing partial solutions in states](bench/dynamic_programming_bellman_array_part_easy.csv)
- ![Recursive scheme](bench/dynamic_programming_bellman_array_rec_easy.csv)

### Dynamic Programming: sequencial vs parallel implementation

The parallel algorithm is implemented as follows: items are divided in two sets of same size. The all-capacities version of the knapsack problem is solved for both sets with the classical `bellman` recursion. Then, the optimal value is computed by merging the information from both arrays.
- ![Sequencial](bench/dynamic_programming_bellman_array_easy.csv)
- ![Parallel](bench/dynamic_programming_bellman_array_parallel_easy.csv)

### Normal dataset

```
n ∈ {100, 1000, 10000, 100000, 1000000}
r ∈ {1000, 10000, 100000, 1000000, 10000000, 100000000}
x ∈ {0.0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9}
wj ~ N(r / 2, r  / 10), 1 <= wj <= r
pj ~ N(wj,    wj / 10), 1 <= pj <= r
c = r * (1 - x) + ∑wj * x;
```

* [branch-and-bound-primal-dual](https://librallu.gitlab.io/splitted-cell-viz/?u=https://raw.githubusercontent.com/fontanf/knapsack/master/bench/branch_and_bound_primal_dual.json)
* [branch-and-bound-primal-dual-combo](https://librallu.gitlab.io/splitted-cell-viz/?u=https://raw.githubusercontent.com/fontanf/knapsack/master/bench/branch_and_bound_primal_dual_combo.json)
* [dynamic-programming-balancing](https://librallu.gitlab.io/splitted-cell-viz/?u=https://raw.githubusercontent.com/fontanf/knapsack/master/bench/dynamic_programming_balancing.json)
* [dynamic-programming-balancing-combo](https://librallu.gitlab.io/splitted-cell-viz/?u=https://raw.githubusercontent.com/fontanf/knapsack/master/bench/dynamic_programming_balancing_combo.json)
* [dynamic-programming-primal-dual](https://librallu.gitlab.io/splitted-cell-viz/?u=https://raw.githubusercontent.com/fontanf/knapsack/master/bench/dynamic_programming_primal_dual.json)
* [dynamic-programming-primal-dual-combo](https://librallu.gitlab.io/splitted-cell-viz/?u=https://raw.githubusercontent.com/fontanf/knapsack/master/bench/dynamic_programming_primal_dual_combo.json)

Remarks:
- The superiority of `dynamic-programming-primal-dual` is clear
- `combo` optimizations do not help:

### Literature dataset

| Algorithm                                | Instances                                                                                                                                                                                                                                |
| ---------------------------------------- | ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| `branch-and-bound-primal-dual`           | ![easy](bench/branch_and_bound_primal_dual_easy.csv),           ![difficult large](bench/branch_and_bound_primal_dual_difficultlarge.csv),           ![difficult small](bench/branch_and_bound_primal_dual_difficultsmall.csv)           |
| `branch-and-bound-primal-dual-combo`     | ![easy](bench/branch_and_bound_primal_dual_combo_easy.csv),     ![difficult large](bench/branch_and_bound_primal_dual_combo_difficultlarge.csv),     ![difficult small](bench/branch_and_bound_primal_dual_combo_difficultsmall.csv)     |
| `dynamic-programming-balancing`          | ![easy](bench/dynamic_programming_balancing_easy.csv),          ![difficult large](bench/dynamic_programming_balancing_difficultlarge.csv),          ![difficult small](bench/dynamic_programming_balancing_difficultsmall.csv)          |
| `dynamic-programming-primal-dual`        | ![easy](bench/dynamic_programming_primal_dual_easy.csv),        ![difficult large](bench/dynamic_programming_primal_dual_difficultlarge.csv),        ![difficult small](bench/dynamic_programming_primal_dual_difficultsmall.csv)        |
| `dynamic-programming-primal-dual-combo`  | ![easy](bench/dynamic_programming_primal_dual_combo_easy.csv),  ![difficult large](bench/dynamic_programming_primal_dual_combo_difficultlarge.csv),  ![difficult small](bench/dynamic_programming_primal_dual_combo_difficultsmall.csv)  |

Remarks:
- These subset sum, strongly correlated and inverse strongly correlated instances are easy to solve since the upper bound happens to always be optimal.
- With `combo` optimizations, `branch-and-bound-primal-dual` is able to solve strongly correlated and inverse strongly correlated instances. It also solves more almost strongly correlated instances.
- Performances of `dynamic-programming-primal-dual` and `combo` seems to match the results from the literature. Furthermore, taking advantage of parallelization, this implementation of `combo` is able to solve all instances of the `difficultlarge` dataset.
- `dynamic-programming-balancing` is very competitive on small instances (R = 1000). Compared to `dynamic-programming-primal-dual`:
  - it performs a lot better on `sc`, `isc`, `spanner`, `mstr` and `pceil` instances.
  - it performs slighlty worse on `u`, `wc` and `ss` instances.
  - it performs a lot worse on `asc` and `circle` instances.

