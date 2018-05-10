#pragma once

#include "../lib/instance.hpp"
#include "../lib/solution.hpp"

namespace knapsack
{

Solution sol_forwardgreedynlogn(const Instance& ins, Info* info = NULL);
Solution sol_backwardgreedynlogn(const Instance& ins, Info* info = NULL);
Solution sol_bestgreedynlogn(const Instance& ins, Info* info = NULL);

}

