#pragma once

#include "knapsacksolver/instance.hpp"
#include "knapsacksolver/solution.hpp"

namespace knapsacksolver
{

Output bellman_array(const Instance& ins, Info info = Info());
Output bellmanpar_array(const Instance& ins, Info info = Info());
Output bellmanrec(const Instance& ins, Info info = Info());
Output bellman_array_all(const Instance& ins, Info info = Info());
Output bellman_array_one(const Instance& ins, Info info = Info());
Output bellman_array_part(const Instance& ins, ItemIdx k=64, Info info = Info());
Output bellman_array_rec(const Instance& ins, Info info = Info());

Output bellman_list(Instance& ins, bool sort = false, Info info = Info());
Output bellman_list_rec(const Instance& ins, Info info = Info());

}

