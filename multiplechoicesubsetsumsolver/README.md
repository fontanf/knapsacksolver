# Multiple-Choice Subset Sum Solver

A solver for the Multiple-Choice Subset Sum Problem.

## Implemented algorithms

* Dynamic programming
  * Bellman
    * Array `-a dynamic_programming_bellman_array`
    * Word RAM (only optimal value) `-a dynamic_programming_bellman_word_ram`
    * Word RAM with recursive scheme `-a dynamic_programming_bellman_word_ram_rec`

## Usage (command line)

Download and uncompress the instances in the `data/` folder:

Compile:
```shell
bazel build -- //...
```

