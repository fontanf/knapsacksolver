#include "knapsack/ub_dantzig/dantzig.hpp"

using namespace knapsack;

#define DBG(x)
//#define DBG(x) x

Profit knapsack::ub_dantzig(const Instance& ins, Info& info)
{
    DBG(std::cout << "UBDANTZIG..." << std::endl;)
    assert(ins.break_item_found());

    ItemPos b = ins.break_item();
    Weight  r = ins.break_capacity();
    Profit  p = ins.break_solution()->profit();
    DBG(std::cout << "P " << p << " B " << b << " " << ins.item(b) << " R " << r << std::endl;)
    if (b <= ins.last_item() && r > 0)
        p += (ins.item(b).p * r) / ins.item(b).w;

    DBG(std::cout << "UB " << p << std::endl;)
    assert(ins.check_ub(p));
    DBG(std::cout << "UBDANTZIG... END" << std::endl;)
    return algorithm_end(p, info);
}

#undef DBG
