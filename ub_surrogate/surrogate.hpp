#ifndef MARTELLO_HPP_YNPESKFM
#define MARTELLO_HPP_YNPESKFM

#include "../lib/instance.hpp"
#include "../lib/solution.hpp"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

Profit ub_surrogate(const Instance& instance, Profit lower_bound,
		boost::property_tree::ptree* pt = NULL, bool verbose = false);

#endif /* end of include guard: MARTELLO_HPP_YNPESKFM */
