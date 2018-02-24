#ifndef BELLMAN_HPP_0M1MVTFZ
#define BELLMAN_HPP_0M1MVTFZ

#include "../lib/instance.hpp"
#include "../lib/solution.hpp"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

/**
 * Time:  O(nc)
 * Space: O(n+c), exactly (c+1) * sizeof(Profit) + cst
 */
Profit opt_bellman(const Instance& instance, Info* info = NULL);

/**
 * Time:  O(nc)
 * Space: O(nc), exactly (c+1) * (n+1) * sizeof(Profit) + sizeof(Solution) + cst
 */
Solution sopt_bellman_1(const Instance& instance, Info* info = NULL);
Solution sopt_bellman_1_it(const Instance& instance, Info* info = NULL);
Solution sopt_bellman_1_rec(const Instance& instance, Info* info = NULL);
Solution sopt_bellman_1_stack(const Instance& instance, Info* info = NULL);
Solution sopt_bellman_1_map(const Instance& instance, Info* info = NULL);

/**
 * Time:  O(n2c)
 * Space: O(n+c), exactly (c+1) * sizeof(Profit) + sizeof(Solution) + cst
 */
Solution sopt_bellman_2(const Instance& instance, Info* info = NULL);

/**
 * Time:  O(nc)
 * Space: O(n+c), exactly 2 * (c+1) * sizeof(Profit) + sizeof(Solution) + cst
 */
Solution sopt_bellman_rec(const Instance& instance, Info* info = NULL);

/**
 * Time:  O(nc)
 * Space: O(c), at most (c+1) * (sizeof(Profit) + sizeof(Weight)) + cst
 * lb_type: "none", "greedy" or "best_greedy"
 * ub_type: "none", "trivial" or "dantzig"
 */
Profit    opt_bellman_list    (const Instance& instance, Profit lb,
        std::string lb_type = "none", std::string ub_type = "none", Info* info = NULL);
Solution sopt_bellman_rec_list(const Instance& instance, const Solution& sol,
        std::string lb_type = "none", std::string ub_type = "none", Info* info = NULL);

#endif /* end of include guard: BELLMAN_HPP_0M1MVTFZ */
