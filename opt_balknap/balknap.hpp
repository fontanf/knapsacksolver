#pragma once

#include "knapsack/lib/instance.hpp"
#include "knapsack/lib/solution.hpp"

namespace knapsack
{

struct BalknapParams
{
    char ub_type        = 't';
    Cpt cpt_greedy      = 0;
    Cpt cpt_greedynlogn = -1;
    Cpt cpt_surrogate   = 2000;
    Cpt cpt_solve_sur   = 2000;
    Cpt cpt_pairing     = 10000;
};

Solution sopt_balknap(Instance& ins, Info& info,
        BalknapParams params = BalknapParams(), ItemPos k = 64, Profit ub = -1);

}

