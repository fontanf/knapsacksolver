#pragma once

#include "../lib/instance.hpp"
#include "../lib/solution.hpp"

struct BalknapParams
{
    std::string upper_bound = "b";
    StateIdx gcd = -1;
    StateIdx greedy = 0;
    StateIdx greedynlogn = -1;
    StateIdx lb_pairing = -1;
    StateIdx surrogate = -1;
    StateIdx solve_sur = -1;
};

Profit opt_balknap_array(const Instance& instance,
        BalknapParams p, Info* info = NULL);
Solution sopt_balknap_array_all(const Instance& instance,
        BalknapParams p, Info* info = NULL);
Solution sopt_balknap_array_part(const Instance& instance,
        BalknapParams p, ItemPos k, Info* info = NULL);

Profit opt_balknap_list(const Instance& instance,
        BalknapParams p, Info* info = NULL);
Solution sopt_balknap_list_all(const Instance& instance,
        BalknapParams p, Info* info = NULL);
Solution sopt_balknap_list_part(const Instance& instance,
        BalknapParams p, ItemPos k, Info* info = NULL);

