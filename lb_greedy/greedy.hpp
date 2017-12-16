#ifndef GREEDY_HPP_D4Y7KHIE
#define GREEDY_HPP_D4Y7KHIE

#include "../lib/instance.hpp"
#include "../lib/solution.hpp"

/**
 * Greedy algorithm.
 * Require sorted or partially sorted input instance.
 * Time:  O(n)
 * Space: O(n)
 * Relative performance guarantee 1/2
 */
Solution  solution_greedy(const Instance& instance);
Profit lower_bound_greedy(const Instance& instance);

#endif /* end of include guard: GREEDY_HPP_D4Y7KHIE */
