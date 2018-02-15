#ifndef DANTZIG_HPP_6HA0XAKO
#define DANTZIG_HPP_6HA0XAKO

#include "../lib/instance.hpp"
#include "../lib/solution.hpp"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

Profit ub_dantzig(const Instance& instance,
        boost::property_tree::ptree* pt = NULL, bool verbose = false);

/**
 * Dantzig upper bound, using items j..n
 */
Profit ub_dantzig_from(const Instance& instance, ItemIdx j, Weight c);

/**
 * Dantzig upper bound for unfeasible solution. Remove items from j down to 1
 * Require r <= 0 and the return profit is non-positive.
 */
Profit ub_dantzig_rev_from(const Instance& instance, ItemIdx j, Weight r);

/**
 * Dantzig upper bound, using items i1..i2
 */
Profit ub_dantzig_from_to(const Instance& instance, ItemIdx i1, ItemIdx i2, Weight c);

/**
 * Dantzig upper bound, using items n1..i1-1,i2+1..n2
 */
Profit ub_dantzig_except(const Instance& instance,
		ItemIdx n1, ItemIdx i1, ItemIdx i2, ItemIdx n2, Weight c);

#endif /* end of include guard: DANTZIG_HPP_6HA0XAKO */
