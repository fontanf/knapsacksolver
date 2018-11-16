#pragma once

#include "knapsack/lib/instance.hpp"
#include "knapsack/lib/solution.hpp"

namespace knapsack
{

Solution sol_forwardgreedynlogn(const Instance& ins, Info& info);
Solution sol_backwardgreedynlogn(const Instance& ins, Info& info);
Solution sol_bestgreedynlogn(const Instance& ins, Info& info);

}

