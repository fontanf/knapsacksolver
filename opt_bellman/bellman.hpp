#ifndef BELLMAN_HPP_0M1MVTFZ
#define BELLMAN_HPP_0M1MVTFZ

#include "../lib/instance.hpp"
#include "../lib/solution.hpp"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

/**
 * Time:  O(nc)
 * Space: O(n+c)
 */
Profit opt_bellman(const Instance& instance,
		boost::property_tree::ptree* pt = NULL, bool verbose = false);

/**
 * Time:  O(nc)
 * Space: O(nc)
 */
Solution sopt_bellman_1(const Instance& instance,
		boost::property_tree::ptree* pt = NULL, bool verbose = false);
Solution sopt_bellman_1_it(const Instance& instance,
		boost::property_tree::ptree* pt = NULL, bool verbose = false);
Solution sopt_bellman_1_rec(const Instance& instance,
		boost::property_tree::ptree* pt = NULL, bool verbose = false);
Solution sopt_bellman_1_stack(const Instance& instance,
		boost::property_tree::ptree* pt = NULL, bool verbose = false);
Solution sopt_bellman_1_map(const Instance& instance,
		boost::property_tree::ptree* pt = NULL, bool verbose = false);

/**
 * Time:  O(n2c)
 * Space: O(n+c)
 */
Solution sopt_bellman_2(const Instance& instance,
		boost::property_tree::ptree* pt = NULL, bool verbose = false);

/**
 * Time:  O(nc)
 * Space: O(n+c)
 */
Solution sopt_bellman_3(const Instance& instance,
		boost::property_tree::ptree* pt = NULL, bool verbose = false);

#endif /* end of include guard: BELLMAN_HPP_0M1MVTFZ */
