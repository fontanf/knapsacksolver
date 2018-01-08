#ifndef BALKNAP_HPP_UHNG2R08
#define BALKNAP_HPP_UHNG2R08

#include "../lib/instance.hpp"
#include "../lib/solution.hpp"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

Profit opt_balknap(const Instance& instance, Profit lb = -1,
		boost::property_tree::ptree* pt = NULL, bool verbose = false);

Profit sopt_balknap(const Instance& instance, Solution& sol_curr,
		boost::property_tree::ptree* pt = NULL, bool verbose = false);

#endif /* end of include guard: BALKNAP_HPP_UHNG2R08 */
