#pragma once

#include "knapsacksolver/solution.hpp"

namespace knapsacksolver
{

Output greedy_nlogn_forward(
        const Instance& instance,
        Info info = Info());

Output greedy_nlogn_backward(
        const Instance& instance,
        Info info = Info());

Output greedy_nlogn(
        const Instance& instance,
        Info info = Info());

}

