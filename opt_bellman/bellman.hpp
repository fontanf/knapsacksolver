#ifndef BELLMAN_HPP_0M1MVTFZ
#define BELLMAN_HPP_0M1MVTFZ

#include "../lib/instance.hpp"
#include "../lib/solution.hpp"

Profit   opt_bellman(const Instance& instance);
Solution sol_bellman(const Instance& instance);
Solution sol_bellman(const Instance& instance, ItemIdx alpha);

#endif /* end of include guard: BELLMAN_HPP_0M1MVTFZ */
