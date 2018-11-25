#include "knapsack/ub_dantzig/dantzig.hpp"

using namespace knapsack;

Profit knapsack::ub_dantzig(const Instance& ins, Info& info)
{
    info.verbose("*** dantzig ***\n");
    assert(ins.break_item_found());

    ItemPos b = ins.break_item();
    Weight  r = ins.break_capacity();
    Profit  p = ins.break_solution()->profit();
    if (b <= ins.last_item() && r > 0)
        p += (ins.item(b).p * r) / ins.item(b).w;

    return algorithm_end(p, info);
}

