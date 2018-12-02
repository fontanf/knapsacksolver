#pragma once

#include "knapsack/lib/instance.hpp"
#include "knapsack/lib/solution.hpp"

namespace knapsack
{

Profit    opt_bellman_array(const Instance& ins, Info& info);
Solution sopt_bellman_array_all(const Instance& ins, Info& info);
Solution sopt_bellman_array_one(const Instance& ins, Info& info);
Solution sopt_bellman_array_part(const Instance& ins, Info& info, ItemIdx k=64);
Solution sopt_bellman_array_rec(const Instance& ins, Info& info);

Profit    opt_bellman_list(const Instance& ins, Info& info);
Solution sopt_bellman_list_all(const Instance& ins, Info& info);
Solution sopt_bellman_list_one(const Instance& ins, Info& info);
Solution sopt_bellman_list_part(const Instance& ins, Info& info, ItemPos k=64);
Solution sopt_bellman_list_rec(const Instance& ins, Info& info);

}

