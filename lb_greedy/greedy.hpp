#ifndef GREEDY_HPP_D4Y7KHIE
#define GREEDY_HPP_D4Y7KHIE

#include "../lib/instance.hpp"
#include "../lib/solution.hpp"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

/**
 * Extended greedy algorithm.
 * Require sorted or partially sorted input instance.
 * Time:  O(n)
 * Space: O(n)
 * Relative performance guarantee 1/2
 */
Solution sol_extgreedy(const Instance& instance,
		boost::property_tree::ptree* pt = NULL, bool verbose = false);
Profit lb_extgreedy(const Instance& instance);

/**
 * Greedy algorithm with items n1..n2
 */
Profit lb_greedy_from_to(const Instance& instance,
		ItemIdx n1, ItemIdx n2, Weight c);

/**
 * Greedy algorithm with items first..i1-1,i2+1..last
 */
Profit lb_greedy_except(const Instance& instance,
		ItemIdx first, ItemIdx i1, ItemIdx i2, ItemIdx last, Weight c);

#endif /* end of include guard: GREEDY_HPP_D4Y7KHIE */
