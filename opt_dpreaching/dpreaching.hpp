#ifndef DPREACHING_HPP_PH02LVYK
#define DPREACHING_HPP_PH02LVYK

#include "../lib/instance.hpp"
#include "../lib/solution.hpp"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

Profit opt_dpreaching(const Instance& instance, Profit ub = -1,
		boost::property_tree::ptree* pt = NULL, bool verbose = false);

Solution sopt_dpreaching_1(const Instance& instance, Profit ub = -1,
		boost::property_tree::ptree* pt = NULL, bool verbose = false);

#endif /* end of include guard: DPREACHING_HPP_PH02LVYK */
