#pragma once

#include "knapsack/lib/instance.hpp"
#include "knapsack/lib/solution.hpp"

namespace knapsack
{

Solution sopt_bab(const Instance& ins, Info* info = NULL);

}

