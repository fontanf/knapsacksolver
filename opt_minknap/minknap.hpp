#pragma once

#include "knapsack/lib/instance.hpp"
#include "knapsack/lib/solution.hpp"

namespace knapsack
{

struct MinknapParams
{
    StateIdx lb_greedy = 0;
    StateIdx lb_greedynlogn = -1;
    StateIdx lb_pairing = -1;
    StateIdx ub_surrogate = -1;
    StateIdx solve_sur = -1;
};

Profit opt_minknap_list(Instance& ins, Info& info,
        MinknapParams params = MinknapParams());
Solution sopt_minknap_list_part(Instance& ins, Info& info,
        MinknapParams params = MinknapParams(), ItemPos k = 64, Profit o = -1);

}

