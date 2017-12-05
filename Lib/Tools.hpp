#ifndef TOOLS_HPP_JW9XQ8FH
#define TOOLS_HPP_JW9XQ8FH

#include "PartialInstance.hpp"

Profit lower_bound_greedy(const PartialInstance& instance);
Solution solution_greedy(const PartialInstance& instance);
Profit opt_bellman_dp(const PartialInstance& instance);
Profit local_search(Solution& solution);

/**
 * Return false if lower_bound can not be reached with items 1..j
 */
bool upper_bound(
		const PartialInstance& instance,
		ItemIdx j, Weight capacity, Profit lower_bound);

/**
 * Return false if lower_bound can not be reached with items 1..j-1,j+1..n
 */
bool upper_bound_without(
		const PartialInstance& instance,
		ItemIdx j, Weight capacity, Profit lower_bound);

/**
 * Return v such that
 * - v[i-1] = '1' => item i is     in any optimal solution
 * - v[i-1] = '0' => item i is not in any optimal solution
 */
std::vector<char> reduce(const PartialInstance& instance, Profit opt);

#endif /* end of include guard: TOOLS_HPP_JW9XQ8FH */
