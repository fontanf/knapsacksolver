#pragma once

#include "knapsack/lib/instance.hpp"
#include "knapsack/lib/solution.hpp"

namespace knapsack
{

knapsack::Output opt_dpprofits_array(const Instance& ins, Info info = Info());
knapsack::Output sopt_dpprofits_array_all(const Instance& ins, Info info = Info());

}

