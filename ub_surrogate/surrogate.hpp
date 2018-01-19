#ifndef MARTELLO_HPP_YNPESKFM
#define MARTELLO_HPP_YNPESKFM

#include "../lib/instance.hpp"
#include "../lib/solution.hpp"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

struct SurrogateOut
{
	SurrogateOut(boost::property_tree::ptree* pt, bool verbose):
		pt(pt), verbose(verbose) {}
	boost::property_tree::ptree* pt;
	bool verbose;
	Profit ub = 0;
	ItemIdx bound = 0;
	Weight multiplier = 0;
};

SurrogateOut ub_surrogate(const Instance& instance, Profit lower_bound,
		boost::property_tree::ptree* pt = NULL, bool verbose = false);

#endif /* end of include guard: MARTELLO_HPP_YNPESKFM */
