#include "knapsack/lib/tester.hpp"
#include "knapsack/opt_bellman/bellman.hpp"

using namespace knapsack;

Profit opt_bellman_array_test(Instance& ins)       { Logger logger; Info info(logger, true); return opt_bellman_array(ins, info); }
Profit opt_bellman_array_all_test(Instance& ins)   { Logger logger; Info info(logger, true); return sopt_bellman_array_all(ins, info).profit(); }
Profit opt_bellman_array_one_test(Instance& ins)   { Logger logger; Info info(logger, true); return sopt_bellman_array_one(ins, info).profit(); }
Profit opt_bellman_array_part1_test(Instance& ins) { Logger logger; Info info(logger, true); return sopt_bellman_array_part(ins, info, 1).profit(); }
Profit opt_bellman_array_part2_test(Instance& ins) { Logger logger; Info info(logger, true); return sopt_bellman_array_part(ins, info, 2).profit(); }
Profit opt_bellman_array_part3_test(Instance& ins) { Logger logger; Info info(logger, true); return sopt_bellman_array_part(ins, info, 3).profit(); }
Profit opt_bellman_array_rec_test(Instance& ins)   { Logger logger; Info info(logger, true); return sopt_bellman_array_rec(ins, info).profit(); }
Profit opt_bellman_list_test(Instance& ins)        { Logger logger; Info info(logger, true); return opt_bellman_list(ins, info); }
Profit opt_bellman_list_all_test(Instance& ins)    { Logger logger; Info info(logger, true); return sopt_bellman_list_all(ins, info).profit(); }
Profit opt_bellman_list_one_test(Instance& ins)    { Logger logger; Info info(logger, true); return sopt_bellman_list_one(ins, info).profit(); }
Profit opt_bellman_list_part1_test(Instance& ins)  { Logger logger; Info info(logger, true); return sopt_bellman_list_part(ins, info, 1).profit(); }
Profit opt_bellman_list_part2_test(Instance& ins)  { Logger logger; Info info(logger, true); return sopt_bellman_list_part(ins, info, 2).profit(); }
Profit opt_bellman_list_part3_test(Instance& ins)  { Logger logger; Info info(logger, true); return sopt_bellman_list_part(ins, info, 3).profit(); }
Profit opt_bellman_list_rec_test(Instance& ins)    { Logger logger; Info info(logger, true); return sopt_bellman_list_rec(ins, info).profit(); }

std::vector<Profit (*)(Instance&)> f {
        opt_bellman_array_test,
        opt_bellman_array_all_test,
        opt_bellman_array_one_test,
        opt_bellman_array_part1_test,
        opt_bellman_array_part2_test,
        opt_bellman_array_part3_test,
        opt_bellman_array_rec_test,
        opt_bellman_list_test,
        //opt_bellman_list_all_test,
        //opt_bellman_list_one_test,
        //opt_bellman_list_part1_test,
        //opt_bellman_list_part2_test,
        //opt_bellman_list_part3_test,
        opt_bellman_list_rec_test,
};

TEST(bellman, TEST)  { test(TEST, f); }
TEST(bellman, SMALL) { test(SMALL, f); }

