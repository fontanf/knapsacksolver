#pragma once

#include "knapsack/lib/instance.hpp"
#include "knapsack/lib/solution.hpp"

namespace knapsack
{

Profit    opt_dpprofits_array(const Instance& ins, Info& info);
Solution sopt_dpprofits_array_all(const Instance& ins, Info& info);

}

