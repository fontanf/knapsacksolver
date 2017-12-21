#ifndef DANTZIG_HPP_6HA0XAKO
#define DANTZIG_HPP_6HA0XAKO

#include "../lib/instance.hpp"
#include "../lib/solution.hpp"

Profit ub_dantzig(const Instance& instance);

/**
 * Dantzig upper bound, using items j..n
 */
Profit ub_dantzig_from(const Instance& instance, ItemIdx j, Weight c);

/**
 * Dantzig upper bound, using items i1..i2
 */
Profit ub_dantzig_from_to(const Instance& instance, ItemIdx i1, ItemIdx i2, Weight c);

/**
 * Dantzig upper bound, using all items except j
 */
Profit ub_dantzig_without(const Instance& instance, ItemIdx j, Weight c);

/**
 * Dantzig upper bound, using items n1..i1-1,i2+1..n2
 */
Profit ub_dantzig_except(const Instance& instance,
		ItemIdx n1, ItemIdx i1, ItemIdx i2, ItemIdx n2, Weight c);

#endif /* end of include guard: DANTZIG_HPP_6HA0XAKO */
