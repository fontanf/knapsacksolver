#pragma once

#include "knapsack/lib/instance.hpp"
#include "knapsack/lib/solution.hpp"

namespace knapsack
{

Profit    opt_dpprofits_array(const Instance& ins, Info& info);
Solution sopt_dpprofits_array_all(const Instance& ins, Info& info);
Solution sopt_dpprofits_array_one(const Instance& ins, Info& info);
Solution sopt_dpprofits_array_part(const Instance& ins, ItemIdx k, Info& info);
Solution sopt_dpprofits_array_rec(const Instance& ins, Info& info);

Profit    opt_dpprofits_list(const Instance& ins, Info& info);
Solution sopt_dpprofits_list_all(const Instance& ins, Info& info);
Solution sopt_dpprofits_list_one(const Instance& ins, Info& info);
Solution sopt_dpprofits_list_part(const Instance& ins, Info& info, ItemPos k=64);
Solution sopt_dpprofits_list_rec(const Instance& ins, Info& info);

}

