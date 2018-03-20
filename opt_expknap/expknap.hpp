#pragma once

#include "../lib/instance.hpp"
#include "../lib/solution.hpp"

struct ExpknapParams
{
    StateIdx lb_greedy = 0;
    StateIdx lb_greedynlogn = -1;
    StateIdx ub_surrogate = -1;
    //StateIdx solve_sur = -1;
};

Solution sopt_expknap(Instance& ins, ExpknapParams& params, Info* info = NULL);

