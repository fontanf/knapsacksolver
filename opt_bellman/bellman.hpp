#pragma once

#include "knapsack/lib/instance.hpp"
#include "knapsack/lib/solution.hpp"

namespace knapsack
{

Output opt_bellman_array(const Instance& ins, Info info = Info());
Output opt_bellmanpar_array(const Instance& ins, Info info = Info());
Output sopt_bellmanrec(const Instance& ins, Info info = Info());
Output sopt_bellman_array_all(const Instance& ins, Info info = Info());
Output sopt_bellman_array_one(const Instance& ins, Info info = Info());
Output sopt_bellman_array_part(const Instance& ins, ItemIdx k=64, Info info = Info());
Output sopt_bellman_array_rec(const Instance& ins, Info info = Info());

Output opt_bellman_list(Instance& ins, bool sort = false, Info info = Info());
Output sopt_bellman_list_rec(const Instance& ins, Info info = Info());

}

