# KnapsackSolver

A solver for the 0-1 knapsack problem

![knapsack](knapsack.png?raw=true "knapsack")

[image source](https://commons.wikimedia.org/wiki/File:Knapsack.svg)

<!--Here are some usage examples of this library:-->
<!--* [Solving a 0-1 knapsack subproblem inside an algorithm for a geometrical variable-sized bin packing problem](https://github.com/fontanf/packingsolver/blob/2cddb90686fb4a0e92f4ee1b4335ceaa2048d2f4/src/algorithms/dichotomic_search.hpp#L149)-->
<!--* Solving the pricing problem inside a column generation algorithm for the [cutting stock problem](https://github.com/fontanf/columngenerationsolver/blob/096802d9e20d2826aed5b44e3b68ac9df6b20da2/include/columngenerationsolver/examples/cutting_stock.hpp#L123), the [multiple knapsack problem](https://github.com/fontanf/columngenerationsolver/blob/096802d9e20d2826aed5b44e3b68ac9df6b20da2/include/columngenerationsolver/examples/multiple_knapsack.hpp#L154), or the [genralized assignment problem](https://github.com/fontanf/generalizedassignmentsolver/blob/68be0ab77efd897fd583f1031dd6cbc946b33f5a/src/algorithms/column_generation.cpp#L177)-->
<!--* [Solving a 0-1 knapsack subproblem inside an algorithm for the packing while travelling problem](https://github.com/fontanf/travellingthiefsolver/blob/ce1b6805e8aee8ee3300fbbc97dbc9153eecff01/src/packing_while_travelling/algorithms/sequential_value_correction.cpp#L19)-->

## Implemented algorithms

* Greedy
  * `O(n)` `-a greedy`

* Upper bounds
  * Dantzig upper bound `-a dantzig`

* Dynamic Programming
  * Bellman
    * Recursive `-a dynamic-programming-bellman-rec`
    * Array (only optimal value) `-a dynamic-programming-bellman-array`
    * Array + parallel (only optimal value) `-a dynamic-programming-bellman-array-parallel`
    * Array (all) `-a dynamic-programming-bellman-array-all`
    * Array (one) `-a dynamic-programming-bellman-array-one`
    * Array (partial solution) `-a dynamic-programming-bellman-array-part`
    * Array (recursive scheme) `-a dynamic-programming-bellman-array-rec`
    * List (only optimal value) `-a dynamic-programming-bellman-list --sort 0`
  * Primal-dual (minknap)
    * List (partial solution) `-a "dynamic-programming-primal-dual --partial-solution-size 64 --pairing 0"`

## Usage

### Command line

Compile:
```shell
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release --parallel
cmake --install build --config Release --prefix install
```

Download data:
```shell
python3 scripts/download_data.py
```

Solve:

```shell
./install/bin/knapsacksolver --verbosity-level 1 --algorithm dynamic-programming-primal-dual --input data/knapsack/largecoeff/knapPI_2_10000_10000000/knapPI_2_10000_10000000_50.csv --format pisinger
```
```
====================================
           KnapsackSolver           
====================================

Instance
--------
Number of items:      10000
Capacity:             24537085856
Highest item profit:  10972919
Highest item weight:  9999761
Total item profit:    49624323576
Total item weight:    49564913430
Weight ratio:         2.02

Algorithm
---------
Dynamic programming - primal-dual - partial

Parameters
----------
Time limit:                          inf
Messages
    Verbosity level:                 1
    Standard output:                 1
    File path:                       
    # streams:                       0
Logger
    Has logger:                      0
    Standard error:                  0
    File path:                       
Greedy:                              1
Pairing:                             0
Partial solution size:               64

    Time (s)  Sol.           Value           Bound             Gap Gap (%)                         Comment
    --------  ----           -----           -----             --- -------                         -------
       0.000     1               0     49624323576     4.96243e+10  100.00                                
       0.000     1     27018043776     49624323576     2.26063e+10   45.55                          greedy
       0.000     1     27018043776     27018127904           84128    0.00             dantzig upper bound
       0.000     1     27018043776     27018127851           84075    0.00                    it 1 (bound)
       0.000     1     27018043776     27018127670           83894    0.00                    it 3 (bound)
       0.000     1     27018043776     27018127668           83892    0.00                    it 5 (bound)
       0.001     1     27018043776     27018127663           83887    0.00                    it 7 (bound)
       0.001     0     27018069931     27018127663           57732    0.00                    it 8 (value)
       0.001     0     27018069931     27018127651           57720    0.00                    it 9 (bound)
       0.001     0     27018115040     27018127651           12611    0.00                   it 10 (value)
       0.001     0     27018115040     27018127648           12608    0.00                   it 11 (bound)
       0.001     0     27018115040     27018127635           12595    0.00                   it 13 (bound)
       0.002     0     27018115040     27018127634           12594    0.00                   it 15 (bound)
       0.002     0     27018115176     27018127634           12458    0.00                   it 16 (value)
       0.002     0     27018117035     27018127634           10599    0.00                   it 17 (value)
       0.002     0     27018117035     27018127633           10598    0.00                   it 17 (bound)
       0.003     0     27018119811     27018127633            7822    0.00                   it 18 (value)
       0.003     0     27018119811     27018127631            7820    0.00                   it 19 (bound)
       0.003     0     27018119811     27018127629            7818    0.00                   it 21 (bound)
       0.003     0     27018121352     27018127629            6277    0.00                   it 22 (value)
       0.004     0     27018121352     27018127623            6271    0.00                   it 25 (bound)
       0.004     0     27018121352     27018127620            6268    0.00                   it 27 (bound)
       0.004     0     27018121352     27018127615            6263    0.00                   it 29 (bound)
       0.004     0     27018121352     27018127611            6259    0.00                   it 33 (bound)
       0.004     0     27018121498     27018127611            6113    0.00                   it 34 (value)
       0.005     0     27018121498     27018127595            6097    0.00                   it 35 (bound)
       0.005     0     27018121498     27018127594            6096    0.00                   it 37 (bound)
       0.005     0     27018121498     27018127588            6090    0.00                   it 39 (bound)
       0.005     0     27018121498     27018127576            6078    0.00                   it 43 (bound)
       0.005     0     27018121498     27018127572            6074    0.00                   it 45 (bound)
       0.006     0     27018121498     27018127570            6072    0.00                   it 47 (bound)
       0.006     0     27018121498     27018127565            6067    0.00                   it 49 (bound)
       0.006     0     27018121498     27018127563            6065    0.00                   it 51 (bound)
       0.006     0     27018121498     27018127556            6058    0.00                   it 53 (bound)
       0.006     0     27018121498     27018127553            6055    0.00                   it 55 (bound)
       0.006     0     27018121498     27018127552            6054    0.00                   it 57 (bound)
       0.006     0     27018121928     27018127552            5624    0.00                   it 58 (value)
       0.006     0     27018121928     27018127548            5620    0.00                   it 61 (bound)
       0.006     0     27018121928     27018127539            5611    0.00                   it 63 (bound)
       0.006     0     27018121928     27018127536            5608    0.00                   it 64 (bound)
       0.006     0     27018121928     27018127533            5605    0.00                   it 65 (bound)
       0.006     0     27018121928     27018127527            5599    0.00                   it 66 (bound)
       0.006     0     27018121928     27018127512            5584    0.00                   it 67 (bound)
       0.006     0     27018121928     27018127511            5583    0.00                   it 68 (bound)
       0.006     0     27018121928     27018127499            5571    0.00                   it 69 (bound)
       0.006     0     27018121928     27018127489            5561    0.00                   it 70 (bound)
       0.006     0     27018121928     27018127442            5514    0.00                   it 71 (bound)
       0.006     0     27018121928     27018127379            5451    0.00                   it 72 (bound)
       0.006     0     27018121928     27018127109            5181    0.00                   it 73 (bound)
       0.006     0     27018121928     27018127103            5175    0.00                   it 74 (bound)
       0.007     0     27018122233     27018127103            4870    0.00                   it 75 (value)
       0.007     0     27018122468     27018127103            4635    0.00                   it 75 (value)
       0.007     0     27018122468     27018126889            4421    0.00                   it 75 (bound)
       0.007     0     27018122468     27018126854            4386    0.00                   it 76 (bound)
       0.007     0     27018122468     27018126307            3839    0.00                   it 77 (bound)
       0.007     0     27018122468     27018125684            3216    0.00                   it 78 (bound)
       0.007     0     27018122468     27018122468               0    0.00                   it 79 (bound)
       0.007     1     27018122468     27018122468               0    0.00        algorithm end (solution)

Final statistics
----------------
Value:                        27018122468
Has solution:                 1
Bound:                        27018122468
Absolute optimality gap:      0
Relative optimality gap (%):  0
Time (s):                     0.00667005
Number of recursive calls:    2

Solution
--------
Number of items:  4959 / 10000 (49.59%)
Weight:           24537085626 / 24537085856 (100%)
Profit:           27018122468
Feasible:         1
```

Run tests:
```
export KNAPSACK_DATA=$(pwd)/data/knapsack
cd build/test
ctest --parallel
```
