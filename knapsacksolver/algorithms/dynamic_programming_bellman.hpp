#pragma once

#include "knapsacksolver/solution.hpp"

namespace knapsacksolver
{

Output dynamic_programming_bellman_rec(
        const Instance& instance,
        Info info = Info());

Output dynamic_programming_bellman_array(
        const Instance& instance,
        Info info = Info());

Output dynamic_programming_bellman_array_parallel(
        const Instance& instance,
        Info info = Info());

Output dynamic_programming_bellman_array_all(
        const Instance& instance,
        Info info = Info());

Output dynamic_programming_bellman_array_one(
        const Instance& instance,
        Info info = Info());

Output dynamic_programming_bellman_array_part(
        const Instance& instance,
        ItemIdx k = 64,
        Info info = Info());

Output dynamic_programming_bellman_array_rec(
        const Instance& instance,
        Info info = Info());

Output dynamic_programming_bellman_list(
        Instance& instance,
        bool sort = false,
        Info info = Info());

Output dynamic_programming_bellman_list_rec(
        const Instance& instance,
        Info info = Info());

}

