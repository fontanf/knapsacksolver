#include "knapsack/opt_dpprofits/dpprofits.hpp"

using namespace knapsack;

Profit knapsack::opt_dpprofits_list(const Instance& ins, Info& info)
{
    info.verbose("*** dpprofits (list) ***\n");
    info.verbose("Not yet implemented.\n");
    (void)ins;
    return algorithm_end(0, info);
}

/******************************************************************************/

Solution knapsack::sopt_dpprofits_list_all(const Instance& ins, Info& info)
{
    info.verbose("*** dpprofits (list, all) ***\n");
    Solution sol(ins);
    info.verbose("Not yet implemented.\n");
    return algorithm_end(sol, info);
}

/******************************************************************************/

Solution knapsack::sopt_dpprofits_list_one(const Instance& ins, Info& info)
{
    info.verbose("*** dpprofits (list, one) ***\n");
    Solution sol(ins);
    info.verbose("Not yet implemented.\n");
    return algorithm_end(sol, info);
}

/******************************************************************************/

Solution knapsack::sopt_dpprofits_list_part(const Instance& ins, Info& info, ItemPos k)
{
    info.verbose("*** dpprofits (list, part " + std::to_string(k) + ") ***\n");
    Solution sol(ins);
    info.verbose("Not yet implemented.\n");
    return algorithm_end(sol, info);
}

/******************************************************************************/

Solution knapsack::sopt_dpprofits_list_rec(const Instance& ins, Info& info)
{
    info.verbose("*** dpprofits (list, rec) ***\n");
    Solution sol(ins);
    info.verbose("Not yet implemented.\n");
    return algorithm_end(sol, info);
}

