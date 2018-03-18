#pragma once

#include "../lib/instance.hpp"
#include "../lib/solution.hpp"

struct BalknapParams
{
    std::string upper_bound = "b";
    StateIdx lb_greedy = 0;
    StateIdx lb_greedynlogn = -1;
    StateIdx lb_pairing = -1;
    StateIdx ub_surrogate = -1;
    StateIdx solve_sur = -1;
};

Profit opt_balknap_array(
        Instance& ins, BalknapParams params, Info* info = NULL);
Solution sopt_balknap_array_all(
        Instance& ins, BalknapParams params, Info* info = NULL);
Solution sopt_balknap_array_part(
        Instance& ins, BalknapParams params, ItemPos k, Info* info = NULL);

Profit opt_balknap_list(
        Instance& ins, BalknapParams params, Info* info = NULL);
Solution sopt_balknap_list_all(
        Instance& ins, BalknapParams params, Info* info = NULL);
Solution sopt_balknap_list_part(Instance& ins,
        BalknapParams params, ItemPos k, Info* info = NULL);

