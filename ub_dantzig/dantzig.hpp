#pragma once

#include "../lib/instance.hpp"
#include "../lib/solution.hpp"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

Profit ub_dantzig(const Instance& ins, Info* info = NULL);

////////////////////////////////////////////////////////////////////////////////

/**
 * Dantzig upper bound, using items j..n
 * Time  O(n)
 * Space O(1)
 * - rev: unfeasible solution. Remove items from j down to 1
 *     Require r <= 0 and the return profit is non-positive.
 * - except: using items n1..i1-1,i2+1..n2
 */
Profit ub_dantzig_from(const Instance& ins, ItemIdx j, const Solution& sol_curr);
Profit ub_dantzig_from(const Instance& ins, ItemIdx j, Profit p, Weight r);
Profit ub_dantzig_from_rev(const Instance& ins, ItemIdx j, const Solution& sol_curr);
Profit ub_dantzig_from_rev(const Instance& ins, ItemIdx j, Profit p, Weight r);
Profit ub_dantzig_from_to(const Instance& ins, ItemIdx i, ItemIdx l, Profit p, Weight r);
Profit ub_dantzig_skip(const Instance& ins, ItemIdx f, ItemIdx l, Profit p, Weight r);

/**
 * Dantzig upper bound but consider wether or not to pack to break item.
 * This bound dominates the classical Dantzig upper bound.
 * Time O(n)
 */
Profit ub_dantzig_2_from(const Instance& ins, ItemIdx j, const Solution& sol_curr);
Profit ub_dantzig_2_from(const Instance& ins, ItemIdx j, Profit p, Weight r);

