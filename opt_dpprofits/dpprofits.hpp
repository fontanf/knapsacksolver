#ifndef DPREACHING_HPP_PH02LVYK
#define DPREACHING_HPP_PH02LVYK

#include "../lib/instance.hpp"
#include "../lib/solution.hpp"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

Profit opt_dpprofits(const Instance& instance, Profit ub = -1, Info* info = NULL);

/**
 * Time:  O(nU)
 * Space: O(nU)
 */
Solution sopt_dpprofits_1(const Instance& instance, Profit ub = -1, Info* info = NULL);

#endif /* end of include guard: DPREACHING_HPP_PH02LVYK */
