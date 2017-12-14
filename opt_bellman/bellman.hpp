#ifndef BELLMAN_HPP_0M1MVTFZ
#define BELLMAN_HPP_0M1MVTFZ

#include "../lib/instance.hpp"
#include "../lib/solution.hpp"

/**
 * Time:  O(nc)
 * Space: O(n+c)
 */
Profit opt_bellman(const Instance& instance);

/**
 * Time:  O(nc)
 * Space: O(nc)
 */
Solution sopt_bellman_1(const Instance& instance);

/**
 * Time:  O(n2c)
 * Space: O(n+c)
 */
Solution sopt_bellman_2(const Instance& instance);

#endif /* end of include guard: BELLMAN_HPP_0M1MVTFZ */
