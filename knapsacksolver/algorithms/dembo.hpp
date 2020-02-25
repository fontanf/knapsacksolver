#pragma once

#include "knapsacksolver/solution.hpp"

namespace knapsacksolver
{

Profit ub_0(const Instance& ins, ItemPos j, const Solution& sol_curr, ItemPos j_max);
Profit ub_0(const Instance& ins, ItemPos j, Profit p, Weight r, ItemPos j_max);

/**
 * u = p + r * pj / wj
 * Time  O(1)
 * Space O(1)
 * - rev: for unfeasible solution
 */
Profit ub_dembo(const Instance& ins, ItemPos j, const Solution& sol_curr);
Profit ub_dembo(const Instance& ins, ItemPos j, Profit p, Weight r);
Profit ub_dembo_rev(const Instance& ins, ItemPos j, const Solution& sol_curr);
Profit ub_dembo_rev(const Instance& ins, ItemPos j, Profit p, Weight r);


/******************************************************************************/

inline Profit ub_0(const Instance& ins, ItemPos j, const Solution& sol_curr, ItemPos j_max)
{
    Profit u = ub_0(ins, j, sol_curr.profit(), sol_curr.remaining_capacity(), j_max);
    return u;
}

inline Profit ub_0(const Instance& ins, ItemPos j, Profit p, Weight r, ItemPos j_max)
{
    if (j <= ins.last_item())
        p += (r * ins.item(j_max).p) / ins.item(j_max).w;
    return p;
}

/******************************************************************************/

inline Profit ub_dembo(const Instance& ins, ItemPos j, const Solution& sol_curr)
{
    Profit u = ub_dembo(ins, j, sol_curr.profit(), sol_curr.remaining_capacity());
    return u;
}

inline Profit ub_dembo(const Instance& ins, ItemPos j, Profit p, Weight r)
{
    //if (j <= ins.last_item())
        //std::cout << " j " << j << " " << ins.item(j) << " l " << ins.last_item() << " p " << p << " r " << r;
    if (j <= ins.last_item())
        p += (r * ins.item(j).p) / ins.item(j).w;
    //std::cout << " p " << p << "  " << std::flush;
    return p;
}

inline Profit ub_dembo_rev(const Instance& ins, ItemPos j, const Solution& sol_curr)
{
    return ub_dembo_rev(ins, j, sol_curr.profit(), sol_curr.remaining_capacity());
}

inline Profit ub_dembo_rev(const Instance& ins, ItemPos j, Profit p, Weight r)
{
    if (j >= ins.first_item()) {
        p += (r * ins.item(j).p + 1) / ins.item(j).w - 1;
    } else {
        p = 0;
    }
    return p;
}

}
