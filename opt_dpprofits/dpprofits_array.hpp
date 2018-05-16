#pragma once

#include "knapsack/lib/instance.hpp"
#include "knapsack/lib/solution.hpp"

namespace knapsack
{

Profit    opt_dpprofits_array(Instance& ins, Info* info = NULL);
Solution sopt_dpprofits_array_all(Instance& ins, Info* info = NULL);
Solution sopt_dpprofits_array_one(Instance& ins, Info* info = NULL);
Solution sopt_dpprofits_array_part(Instance& ins, ItemIdx k, Info* info = NULL);
Solution sopt_dpprofits_array_rec(Instance& ins, Info* info = NULL);

}

