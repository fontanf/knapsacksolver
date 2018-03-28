#pragma once

#include "../lib/instance.hpp"
#include "../lib/solution.hpp"

struct MinknapParams
{
    StateIdx lb_greedy = 0;
    StateIdx lb_greedynlogn = -1;
    StateIdx lb_pairing = -1;
    StateIdx ub_surrogate = -1;
    StateIdx solve_sur = -1;
};

Profit opt_minknap_list(Instance& ins, MinknapParams params, Info* info = NULL);
Solution sopt_minknap_list_part(Instance& ins,
        MinknapParams params, ItemPos k, Info* info = NULL, Profit ub = -1);

