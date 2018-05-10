#pragma once

#include "../lib/instance.hpp"
#include "../lib/solution.hpp"

namespace knapsack
{

Profit    opt_bellman_list(const Instance& ins, Info* info = NULL);
Solution sopt_bellman_list_all(const Instance& ins, Info* info = NULL);
Solution sopt_bellman_list_one(const Instance& ins, Info* info = NULL);
Solution sopt_bellman_list_part(const Instance& ins, ItemPos k, Info* info = NULL);
Solution sopt_bellman_list_rec(const Instance& ins, Info* info = NULL);

}

