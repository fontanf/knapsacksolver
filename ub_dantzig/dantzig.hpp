#ifndef DANTZIG_HPP_6HA0XAKO
#define DANTZIG_HPP_6HA0XAKO

#include "../lib/instance.hpp"
#include "../lib/solution.hpp"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

Profit ub_dantzig(const Instance& instance, Info* info = NULL);

////////////////////////////////////////////////////////////////////////////////

/**
 * u = p + r * pj / wj
 * Time  O(1)
 * Space O(1)
 * - rev: for unfeasible solution
 */
Profit ub_trivial_from(const Instance& instance, ItemPos j, const Solution& sol_curr);
Profit ub_trivial_from(const Instance& instance, ItemPos j, Profit p, Weight r);
Profit ub_trivial_from_rev(const Instance& instance, ItemPos j, const Solution& sol_curr);
Profit ub_trivial_from_rev(const Instance& instance, ItemPos j, Profit p, Weight r);

/**
 * Dantzig upper bound, using items j..n
 * Time  O(n)
 * Space O(1)
 * - rev: unfeasible solution. Remove items from j down to 1
 *     Require r <= 0 and the return profit is non-positive.
 * - except: using items n1..i1-1,i2+1..n2
 */
Profit ub_dantzig_from(const Instance& instance, ItemIdx j, const Solution& sol_curr);
Profit ub_dantzig_from(const Instance& instance, ItemIdx j, Profit p, Weight r);
Profit ub_dantzig_from_rev(const Instance& instance, ItemIdx j, const Solution& sol_curr);
Profit ub_dantzig_from_rev(const Instance& instance, ItemIdx j, Profit p, Weight r);
Profit ub_dantzig_except(const Instance& instance,
        ItemIdx n1, ItemIdx i1, ItemIdx i2, ItemIdx n2, Weight c);

/**
 * Dantzig upper bound but consider wether or not to pack to break item.
 * This bound dominates the classical Dantzig upper bound.
 * Time O(n)
 */
Profit ub_dantzig_2_from(const Instance& instance, ItemIdx j, const Solution& sol_curr);
Profit ub_dantzig_2_from(const Instance& instance, ItemIdx j, Profit p, Weight r);

#endif /* end of include guard: DANTZIG_HPP_6HA0XAKO */
