#pragma once

#include "lib/instance.hpp"
#include "lib/solution.hpp"

namespace knapsack
{

Profit ub_0(const Instance& ins, ItemPos j, const Solution& sol_curr);
Profit ub_0(const Instance& ins, ItemPos j, Profit p, Weight r);
Profit ub_0_rev(const Instance& ins, ItemPos j, const Solution& sol_curr);
Profit ub_0_rev(const Instance& ins, ItemPos j, Profit p, Weight r);

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

#define DBG(x)
//#define DBG(x) x

inline Profit ub_0(const Instance& ins, ItemIdx j, const Solution& sol_curr)
{
    Profit u = ub_0(ins, j, sol_curr.profit(), sol_curr.remaining_capacity());
    return u;
}

inline Profit ub_0(const Instance& ins, ItemIdx j, Profit p, Weight r)
{
    if (j <= ins.last_item())
        p += (r * ins.max_efficiency_item().p) / ins.max_efficiency_item().w;
    return p;
}

inline Profit ub_0_rev(const Instance& ins, ItemIdx j, const Solution& sol_curr)
{
    return ub_0_rev(ins, j, sol_curr.profit(), sol_curr.remaining_capacity());
}

inline Profit ub_0_rev(const Instance& ins, ItemIdx j, Profit p, Weight r)
{
    if (j >= ins.first_item()) {
        p += (r * ins.max_efficiency_item().p + 1) / ins.max_efficiency_item().w - 1;
    } else {
        p = 0;
    }
    return p;
}

/******************************************************************************/

inline Profit ub_dembo(const Instance& ins, ItemIdx j, const Solution& sol_curr)
{
    Profit u = ub_dembo(ins, j, sol_curr.profit(), sol_curr.remaining_capacity());
    return u;
}

inline Profit ub_dembo(const Instance& ins, ItemIdx j, Profit p, Weight r)
{
    DBG(std::cout << " UBD j " << j << " L " << ins.last_item() << std::flush;)
    if (j <= ins.last_item())
        p += (r * ins.item(j).p) / ins.item(j).w;
    DBG(std::cout << " UBD" << std::flush;)
    return p;
}

inline Profit ub_dembo_rev(const Instance& ins, ItemIdx j, const Solution& sol_curr)
{
    return ub_dembo_rev(ins, j, sol_curr.profit(), sol_curr.remaining_capacity());
}

inline Profit ub_dembo_rev(const Instance& ins, ItemIdx j, Profit p, Weight r)
{
    DBG(std::cout << " UBDR j " << j << " F " << ins.first_item() << std::flush;)
    if (j >= ins.first_item()) {
        p += (r * ins.item(j).p + 1) / ins.item(j).w - 1;
    } else {
        p = 0;
    }
    DBG(std::cout << " UBDR" << std::flush;)
    return p;
}

#undef DBG

}
