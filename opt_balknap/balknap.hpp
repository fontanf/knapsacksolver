#pragma once

#include "knapsack/lib/instance.hpp"
#include "knapsack/lib/solution.hpp"

namespace knapsack
{

struct BalknapParams
{
    std::string upper_bound = "b";
    StateIdx lb_greedy = 0;
    StateIdx lb_greedynlogn = -1;
    StateIdx lb_pairing = -1;
    StateIdx ub_surrogate = -1;
    StateIdx solve_sur = -1;
};

Profit opt_balknap_array(Instance& ins, Info& info,
        BalknapParams params = BalknapParams());
Solution sopt_balknap_array_all(Instance& ins, Info& info,
        BalknapParams params = BalknapParams());
Solution sopt_balknap_array_part(Instance& ins, Info& info,
        BalknapParams params = BalknapParams(), ItemPos k = 64);

Profit opt_balknap_list(Instance& ins, Info& info,
        BalknapParams params = BalknapParams());
Solution sopt_balknap_list_all(Instance& ins, Info& info,
        BalknapParams params = BalknapParams());
Solution sopt_balknap_list_part(Instance& ins, Info& info,
        BalknapParams params = BalknapParams(), ItemPos k = 64, Profit ub = -1);

}

