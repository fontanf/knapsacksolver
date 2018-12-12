#include "knapsack/opt_dpprofits/dpprofits.hpp"

using namespace knapsack;

Profit knapsack::opt_dpprofits_list(const Instance& ins, Info& info)
{
    VER(info, "*** dpprofits (list) ***" << std::endl);
    VER(info, "Not yet implemented." << std::endl);
    (void)ins;
    return algorithm_end(0, info);
}

/******************************************************************************/

Solution knapsack::sopt_dpprofits_list_all(const Instance& ins, Info& info)
{
    VER(info, "*** dpprofits (list, all) ***" << std::endl);
    Solution sol(ins);
    VER(info, "Not yet implemented." << std::endl);
    return algorithm_end(sol, info);
}

/******************************************************************************/

Solution knapsack::sopt_dpprofits_list_one(const Instance& ins, Info& info)
{
    VER(info, "*** dpprofits (list, one) ***" << std::endl);
    Solution sol(ins);
    VER(info, "Not yet implemented." << std::endl);
    return algorithm_end(sol, info);
}

/******************************************************************************/

Solution knapsack::sopt_dpprofits_list_part(const Instance& ins, Info& info, ItemPos k)
{
    VER(info, "*** dpprofits (list, part " << k << ") ***" << std::endl);
    Solution sol(ins);
    VER(info, "Not yet implemented." << std::endl);
    return algorithm_end(sol, info);
}

/******************************************************************************/

Solution knapsack::sopt_dpprofits_list_rec(const Instance& ins, Info& info)
{
    VER(info, "*** dpprofits (list, rec) ***" << std::endl);
    Solution sol(ins);
    VER(info, "Not yet implemented." << std::endl);
    return algorithm_end(sol, info);
}

