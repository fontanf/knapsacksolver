# Knapsack

Implementations of classical algorithms for the Knapsack Problem. Most algorithms are detailed in the "Knapsack Problem" book (Pferschy, 2004).

DP with Bellman recursion *(See "Knapsack Problem", 2.3 Dynamic Programming, 3.3 Storage Reduction in Dynamic Programming, 3.4 Dynamic Programming with Lists, 3.5 Combining Dynamic Programming and Upper Bounds - Pferschy, 2004)*
- Only optimal value, Time O(nc), Space O(c) `opt_bellman/main`
- Version 1, Time O(nc), Space O(nc) `opt_bellman/main -a 1`
  - Iterative implementation (default) `opt_bellman/main -a 1it`
  - Implementation with a recursive function `opt_bellman/main -a 1rec`
  - Implementation with a stack simulating a recursive function `opt_bellman/main -a 1stack`
  - Recursive implementation using a map as memory `opt_bellman/main -a 1map`
- Version 2, Time O(n2c), Space O(c+n) `opt_bellman/main -a 2`
- Recursive scheme Time O(nc), Space O(n+c) `opt_bellman/main -a rec`
- With lists, only optimal value `opt_bellman/main -a list`
- With lists, recursive scheme `opt_bellman/main -a reclist`
- With Upper Bounds, only optimal value `opt_bellman/main -a ub`
- With Upper Bounds, recursive scheme `opt_bellman/main -a recub`

Branch-and-bound *(See "Knapsack Problem", 2.4 Branch-and-Bound - Pferschy, 2004)*
- Implementation with a recursive function (default) `opt_bab/main -a rec`
- Implementation with a stack simulating a recusrive function `opt_bab/main -a stack`
