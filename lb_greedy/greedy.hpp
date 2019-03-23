#pragma once

#include "knapsack/lib/instance.hpp"
#include "knapsack/lib/solution.hpp"

namespace knapsack
{

Solution sol_greedy(const Instance& ins, Info info = Info());

}

