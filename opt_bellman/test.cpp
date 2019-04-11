#include "knapsack/lib/tester.hpp"
#include "knapsack/opt_bellman/bellman.hpp"

using namespace knapsack;

Profit opt_bellman_array_test(Instance& ins)       { return opt_bellman_array(ins); }
Profit opt_bellmanpar_array_test(Instance& ins)    { return opt_bellmanpar_array(ins); }
Profit opt_bellmanrec_test(Instance& ins)          { return sopt_bellmanrec(ins).profit(); }
Profit opt_bellman_array_all_test(Instance& ins)   { return sopt_bellman_array_all(ins).profit(); }
Profit opt_bellman_array_one_test(Instance& ins)   { return sopt_bellman_array_one(ins).profit(); }
Profit opt_bellman_array_part1_test(Instance& ins) { return sopt_bellman_array_part(ins, 1).profit(); }
Profit opt_bellman_array_part2_test(Instance& ins) { return sopt_bellman_array_part(ins, 2).profit(); }
Profit opt_bellman_array_part3_test(Instance& ins) { return sopt_bellman_array_part(ins, 3).profit(); }
Profit opt_bellman_array_rec_test(Instance& ins)   { return sopt_bellman_array_rec(ins).profit(); }
Profit opt_bellman_list_test(Instance& ins)        { return opt_bellman_list(ins); }
Profit opt_bellman_list_sort_test(Instance& ins)   { return opt_bellman_list(ins, true); }
Profit opt_bellman_list_rec_test(Instance& ins)    { return sopt_bellman_list_rec(ins).profit(); }

std::vector<Profit (*)(Instance&)> f {
        opt_bellman_array_test,
        opt_bellmanpar_array_test,
        opt_bellmanrec_test,
        opt_bellman_array_all_test,
        opt_bellman_array_one_test,
        opt_bellman_array_part1_test,
        opt_bellman_array_part2_test,
        opt_bellman_array_part3_test,
        opt_bellman_array_rec_test,
        opt_bellman_list_test,
        opt_bellman_list_sort_test,
        opt_bellman_list_rec_test,
};

TEST(bellman, TEST)  { test(TEST, f); }
TEST(bellman, SMALL) { test(SMALL, f); }

