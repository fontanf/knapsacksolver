# Subset Sum Solver

A solver for the Subset Sum Problem.

## Implemented algorithms

* Dynamic programming
  * Bellman
    * Array `-a dynamic_programming_bellman_array`
    * List (only optimal value) `-a dynamic_programming_bellman_list`
    * Word RAM (only optimal value) `-a dynamic_programming_bellman_word_ram`
    * Word RAM with recursive scheme `-a dynamic_programming_bellman_word_ram_rec`
  * Balancing
    * Array (only optimal value) `-a dynamic_programming_balancing_array`
<!---
  * Primal-dual
    * Array `-a dynamic_programming_primal_dual_array`
    * Word RAM (only optimal value) `-a dynamic_programming_primal_dual_word_ram`
--->

## Usage (command line)

Download and uncompress the instances in the `data/` folder:

Compile:
```shell
bazel build -- //...
```

Examples:

```shell
./bazel-bin/subsetsumsolver/main -v 1 -i data/subsetsum/pthree/pthree_1000_1 -a dynamic_programming_bellman_word_ram_rec
```
```
=============================
      Subset Sum Solver      
=============================

Instance
--------
Number of items:          1000
Capacity:                 250000

Algorithm
---------
Dynamic Programming - Bellman

Parameters
----------
Implementation:                  Word RAM
Method for retrieving solution:  Recursive scheme

     T (s)            UB            LB           GAP   GAP (%)                 Comment
     -----            --            --           ---   -------                 -------
     0.000             0        250000        250000      1.00                        
     0.008        250000        250000             0      0.00   tree search completed

Final statistics
----------------
Value:                    250000
Has solution:             1
Bound:                    250000
Gap:                      0
Gap (%):                  0
Time (s):                 0.0076
```

