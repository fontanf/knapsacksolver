#pragma once

#include "knapsacksolver/solution.hpp"

namespace knapsacksolver
{

Output bellman_array(const Instance& instance, Info info = Info());
Output bellmanpar_array(const Instance& instance, Info info = Info());
Output bellmanrec(const Instance& instance, Info info = Info());
Output bellman_array_all(const Instance& instance, Info info = Info());
Output bellman_array_one(const Instance& instance, Info info = Info());
Output bellman_array_part(const Instance& instance, ItemIdx k=64, Info info = Info());
Output bellman_array_rec(const Instance& instance, Info info = Info());

Output bellman_list(Instance& instance, bool sort = false, Info info = Info());
Output bellman_list_rec(const Instance& instance, Info info = Info());

}

