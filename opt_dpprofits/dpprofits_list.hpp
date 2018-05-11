#pragma once

#include "lib/instance.hpp"
#include "lib/solution.hpp"

namespace knapsack
{

Profit    opt_dpprofits_list(const Instance& ins, Info* info = NULL);
Solution sopt_dpprofits_list_all(const Instance& ins, Info* info = NULL);
Solution sopt_dpprofits_list_one(const Instance& ins, Info* info = NULL);
Solution sopt_dpprofits_list_part(const Instance& ins, ItemPos k, Info* info = NULL);
Solution sopt_dpprofits_list_rec(const Instance& ins, Info* info = NULL);

}

