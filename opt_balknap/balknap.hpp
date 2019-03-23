#pragma once

#include "knapsack/lib/instance.hpp"
#include "knapsack/lib/solution.hpp"

namespace knapsack
{

struct BalknapParams
{
    ItemPos k           = 64;
    char ub_type        = 't';
    Cpt cpt_greedy      = 0;
    Cpt cpt_greedynlogn = -1;
    Cpt cpt_surrogate   = 2000;

    static BalknapParams pure()
    {
        return {
            .k = 64,
            .ub_type = 'b',
            .cpt_greedy = -1,
            .cpt_greedynlogn = -1,
            .cpt_surrogate = -1,
        };
    }

    static BalknapParams fontan()
    {
        return {
            .k = 64,
            .ub_type = 't',
            .cpt_greedy = 0,
            .cpt_greedynlogn = 5000,
            .cpt_surrogate = 2000,
        };
    }
};

Solution sopt_balknap(Instance& ins,
        BalknapParams params = BalknapParams(), Profit ub = -1, Info info = Info());

}

