#pragma once

#include "knapsack/lib/instance.hpp"
#include "knapsack/lib/solution.hpp"

namespace knapsack
{

Profit    opt_bellman_list(Instance& ins, Info* info = NULL);
Solution sopt_bellman_list_all(Instance& ins, Info* info = NULL);
Solution sopt_bellman_list_one(Instance& ins, Info* info = NULL);
Solution sopt_bellman_list_part(Instance& ins, ItemPos k, Info* info = NULL);
Solution sopt_bellman_list_rec(Instance& ins, Info* info = NULL);

}

