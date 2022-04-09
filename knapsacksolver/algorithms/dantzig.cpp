#include "knapsacksolver/algorithms/dantzig.hpp"

using namespace knapsacksolver;

Profit knapsacksolver::ub_dantzig(const Instance& instance, Info info)
{
    FFOT_VER(info, "*** dantzig ***" << std::endl);
    assert(instance.sort_status() >= 1);

    ItemPos b = instance.break_item();
    Weight  r = instance.break_capacity();
    Profit  p = instance.break_solution()->profit();
    if (b <= instance.last_item() && r > 0)
        p += (instance.item(b).p * r) / instance.item(b).w;

    return algorithm_end(p, info);
}

