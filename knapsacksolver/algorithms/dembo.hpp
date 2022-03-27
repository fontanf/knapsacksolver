#pragma once

#include "knapsacksolver/solution.hpp"

namespace knapsacksolver
{

Profit ub_0(const Instance& instance, ItemPos j, const Solution& sol_curr, ItemPos j_max);
Profit ub_0(const Instance& instance, ItemPos j, Profit p, Weight r, ItemPos j_max);

/**
 * u = p + r * pj / wj
 * Time  O(1)
 * Space O(1)
 * - rev: for unfeasible solution
 */
Profit ub_dembo(const Instance& instance, ItemPos j, const Solution& sol_curr);
Profit ub_dembo(const Instance& instance, ItemPos j, Profit p, Weight r);
Profit ub_dembo_rev(const Instance& instance, ItemPos j, const Solution& sol_curr);
Profit ub_dembo_rev(const Instance& instance, ItemPos j, Profit p, Weight r);


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

inline Profit ub_0(const Instance& instance, ItemPos j, const Solution& sol_curr, ItemPos j_max)
{
    Profit u = ub_0(instance, j, sol_curr.profit(), sol_curr.remaining_capacity(), j_max);
    return u;
}

inline Profit ub_0(const Instance& instance, ItemPos j, Profit p, Weight r, ItemPos j_max)
{
    if (j <= instance.last_item())
        p += (r * instance.item(j_max).p) / instance.item(j_max).w;
    return p;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

inline Profit ub_dembo(const Instance& instance, ItemPos j, const Solution& sol_curr)
{
    Profit u = ub_dembo(instance, j, sol_curr.profit(), sol_curr.remaining_capacity());
    return u;
}

inline Profit ub_dembo(const Instance& instance, ItemPos j, Profit p, Weight r)
{
    //if (j <= instance.last_item())
        //std::cout << " j " << j << " " << instance.item(j) << " l " << instance.last_item() << " p " << p << " r " << r;
    if (j <= instance.last_item())
        p += (r * instance.item(j).p) / instance.item(j).w;
    //std::cout << " p " << p << "  " << std::flush;
    return p;
}

inline Profit ub_dembo_rev(const Instance& instance, ItemPos j, const Solution& sol_curr)
{
    return ub_dembo_rev(instance, j, sol_curr.profit(), sol_curr.remaining_capacity());
}

inline Profit ub_dembo_rev(const Instance& instance, ItemPos j, Profit p, Weight r)
{
    if (j >= instance.first_item()) {
        p += (r * instance.item(j).p + 1) / instance.item(j).w - 1;
    } else {
        p = 0;
    }
    return p;
}

}
