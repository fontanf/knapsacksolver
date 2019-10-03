#pragma once

#include "knapsack/lib/instance.hpp"
#include "knapsack/lib/solution.hpp"

namespace knapsack
{

Output sol_forwardgreedynlogn(const Instance& ins, Info info = Info());
Output sol_backwardgreedynlogn(const Instance& ins, Info info = Info());
Output sol_greedynlogn(const Instance& ins, Info info = Info());

}

