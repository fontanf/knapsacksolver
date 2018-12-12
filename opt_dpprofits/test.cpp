#include "knapsack/lib/tester.hpp"
#include "knapsack/opt_bellman/bellman.hpp"
#include "knapsack/opt_dpprofits/dpprofits.hpp"

using namespace knapsack;

Profit opt_bellman_array_test(Instance& ins)         { Logger logger; Info info(logger, true); return opt_bellman_array(ins, info); }
Profit opt_dpprofits_array_test(Instance& ins)       { Logger logger; Info info(logger, true); return opt_dpprofits_array(ins, info); }
Profit opt_dpprofits_array_all_test(Instance& ins)   { Logger logger; Info info(logger, true); return sopt_dpprofits_array_all(ins, info).profit(); }
Profit opt_dpprofits_array_one_test(Instance& ins)   { Logger logger; Info info(logger, true); return sopt_dpprofits_array_one(ins, info).profit(); }
Profit opt_dpprofits_array_part1_test(Instance& ins) { Logger logger; Info info(logger, true); return sopt_dpprofits_array_part(ins, info, 1).profit(); }
Profit opt_dpprofits_array_part2_test(Instance& ins) { Logger logger; Info info(logger, true); return sopt_dpprofits_array_part(ins, info, 2).profit(); }
Profit opt_dpprofits_array_part3_test(Instance& ins) { Logger logger; Info info(logger, true); return sopt_dpprofits_array_part(ins, info, 3).profit(); }
Profit opt_dpprofits_array_rec_test(Instance& ins)   { Logger logger; Info info(logger, true); return sopt_dpprofits_array_rec(ins, info).profit(); }
Profit opt_dpprofits_list_test(Instance& ins)        { Logger logger; Info info(logger, true); return opt_dpprofits_list(ins, info); }
Profit opt_dpprofits_list_all_test(Instance& ins)    { Logger logger; Info info(logger, true); return sopt_dpprofits_list_all(ins, info).profit(); }
Profit opt_dpprofits_list_one_test(Instance& ins)    { Logger logger; Info info(logger, true); return sopt_dpprofits_list_one(ins, info).profit(); }
Profit opt_dpprofits_list_part1_test(Instance& ins)  { Logger logger; Info info(logger, true); return sopt_dpprofits_list_part(ins, info, 1).profit(); }
Profit opt_dpprofits_list_part2_test(Instance& ins)  { Logger logger; Info info(logger, true); return sopt_dpprofits_list_part(ins, info, 2).profit(); }
Profit opt_dpprofits_list_part3_test(Instance& ins)  { Logger logger; Info info(logger, true); return sopt_dpprofits_list_part(ins, info, 3).profit(); }
Profit opt_dpprofits_list_rec_test(Instance& ins)    { Logger logger; Info info(logger, true); return sopt_dpprofits_list_rec(ins, info).profit(); }

std::vector<Profit (*)(Instance&)> tested_functions()
{
    return {
        opt_bellman_array_test,
        opt_dpprofits_array_test,
        opt_dpprofits_array_all_test,
        //opt_dpprofits_array_one_test,
        //opt_dpprofits_array_part1_test,
        //opt_dpprofits_array_part2_test,
        //opt_dpprofits_array_part3_test,
        //opt_dpprofits_array_rec_test,
        //opt_dpprofits_list_test,
        //opt_dpprofits_list_all_test,
        //opt_dpprofits_list_one_test,
        //opt_dpprofits_list_part1_test,
        //opt_dpprofits_list_part2_test,
        //opt_dpprofits_list_part3_test,
        //opt_dpprofits_list_rec_test,
    };
}

TEST(Bellman, TEST)  { test(TEST, tested_functions()); }
TEST(Bellman, SMALL) { test(SMALL, tested_functions()); }

