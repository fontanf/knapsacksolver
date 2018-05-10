#pragma once

#include "../lib/instance.hpp"
#include "../lib/solution.hpp"

namespace knapsack
{

Profit ub_dantzig(const Instance& ins, Info* info = NULL);

}

