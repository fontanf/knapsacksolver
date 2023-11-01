#pragma once

#include "knapsacksolver/knapsack/solution.hpp"

namespace knapsacksolver
{
namespace knapsack
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
}

