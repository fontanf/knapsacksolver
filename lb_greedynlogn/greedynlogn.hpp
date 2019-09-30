#pragma once

#include "knapsack/lib/instance.hpp"
#include "knapsack/lib/solution.hpp"

namespace knapsack
{

knapsack::Output sol_forwardgreedynlogn(const Instance& ins, Info info = Info());
knapsack::Output sol_backwardgreedynlogn(const Instance& ins, Info info = Info());
knapsack::Output sol_greedynlogn(const Instance& ins, Info info = Info());

}

