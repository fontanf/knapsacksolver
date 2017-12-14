#ifndef DANTZIG_HPP_6HA0XAKO
#define DANTZIG_HPP_6HA0XAKO

#include "../lib/instance.hpp"
#include "../lib/solution.hpp"

Profit ub_dantzig(const Instance& instance);
Profit ub_dantzig_from(const Instance& instance, ItemIdx i, Weight c);
Profit ub_dantzig_without(const Instance& instance, ItemIdx i, Weight c);

#endif /* end of include guard: DANTZIG_HPP_6HA0XAKO */
