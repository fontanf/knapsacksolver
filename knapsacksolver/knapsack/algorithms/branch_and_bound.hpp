#pragma once

#include "knapsacksolver/knapsack/solution.hpp"

namespace knapsacksolver
{
namespace knapsack
{

Output branch_and_bound(
        Instance& instance,
        bool sort = false,
        Info info = Info());

}
}

