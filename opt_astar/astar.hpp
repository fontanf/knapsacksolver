#pragma once

#include "knapsack/lib/instance.hpp"
#include "knapsack/lib/solution.hpp"

namespace knapsack
{

Solution sopt_astar(Instance& ins, Info& info);

Solution sopt_astar_dp(Instance& ins, Info& info);

struct StarknapParams
{
    std::string upper_bound = "t";
    StateIdx lb_greedy = 0;
    StateIdx lb_greedynlogn = -1;
    StateIdx lb_pairing = -1;
    StateIdx ub_surrogate = -1;
    StateIdx solve_sur = -1;
};

Solution sopt_starknap(Instance& ins, Info& info, StarknapParams params = StarknapParams());

}

