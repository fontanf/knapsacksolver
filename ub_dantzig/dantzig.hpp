#pragma once

#include "knapsack/lib/instance.hpp"
#include "knapsack/lib/solution.hpp"

namespace knapsack
{

Profit ub_dantzig(const Instance& ins, Info info = Info());

}

