#pragma once

#include "knapsack/lib/instance.hpp"
#include "knapsack/lib/solution.hpp"

namespace knapsack
{

Output opt_dpprofits_array(const Instance& ins, Info info = Info());
Output sopt_dpprofits_array_all(const Instance& ins, Info info = Info());

}

