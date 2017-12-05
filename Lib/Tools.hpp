#ifndef TOOLS_HPP_JW9XQ8FH
#define TOOLS_HPP_JW9XQ8FH

#include "Instance.hpp"

Profit lower_bound_greedy(const Instance& instance);
Solution solution_greedy(const Instance& instance);
Profit opt_bellman_dp(const Instance& instance);
Profit local_search(Solution& solution);

/**
 * Return false if lower_bound can not be reached with items 1..j
 */
bool upper_bound(
		const Instance& instance,
		ItemIdx j, Weight capacity, Profit lower_bound);

/**
 * Return false if lower_bound can not be reached with items 1..j-1,j+1..n
 */
bool upper_bound_without(
		const Instance& instance,
		ItemIdx j, Weight capacity, Profit lower_bound);

#endif /* end of include guard: TOOLS_HPP_JW9XQ8FH */
