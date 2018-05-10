#pragma once

#include "../lib/instance.hpp"
#include "../lib/solution.hpp"

namespace knapsack
{

Profit    opt_dpprofits_array(const Instance& ins, Info* info = NULL);
Solution sopt_dpprofits_array_all(const Instance& ins, Info* info = NULL);
Solution sopt_dpprofits_array_one(const Instance& ins, Info* info = NULL);
Solution sopt_dpprofits_array_part(const Instance& ins, ItemIdx k, Info* info = NULL);
Solution sopt_dpprofits_array_rec(const Instance& ins, Info* info = NULL);

}

