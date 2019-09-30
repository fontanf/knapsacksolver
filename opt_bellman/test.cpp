#include "knapsack/lib/tester.hpp"
#include "knapsack/opt_bellman/bellman.hpp"

using namespace knapsack;

knapsack::Output opt_bellman_array_test(Instance& ins)        { return opt_bellman_array(ins); }
knapsack::Output sopt_bellmanpar_array_test(Instance& ins)    { return opt_bellmanpar_array(ins); }
knapsack::Output sopt_bellmanrec_test(Instance& ins)          { return sopt_bellmanrec(ins); }
knapsack::Output sopt_bellman_array_all_test(Instance& ins)   { return sopt_bellman_array_all(ins); }
knapsack::Output sopt_bellman_array_one_test(Instance& ins)   { return sopt_bellman_array_one(ins); }
knapsack::Output sopt_bellman_array_part1_test(Instance& ins) { return sopt_bellman_array_part(ins, 1); }
knapsack::Output sopt_bellman_array_part2_test(Instance& ins) { return sopt_bellman_array_part(ins, 2); }
knapsack::Output sopt_bellman_array_part3_test(Instance& ins) { return sopt_bellman_array_part(ins, 3); }
knapsack::Output sopt_bellman_array_rec_test(Instance& ins)   { return sopt_bellman_array_rec(ins); }
knapsack::Output sopt_bellman_list_test(Instance& ins)        { return opt_bellman_list(ins); }
knapsack::Output  opt_bellman_list_sort_test(Instance& ins)   { return opt_bellman_list(ins, true); }
knapsack::Output sopt_bellman_list_rec_test(Instance& ins)    { return sopt_bellman_list_rec(ins); }

std::vector<knapsack::Output (*)(Instance&)> f_sopt {
        opt_bellman_array_test,
        sopt_bellmanpar_array_test,
        sopt_bellmanrec_test,
        sopt_bellman_array_all_test,
        sopt_bellman_array_one_test,
        sopt_bellman_array_part1_test,
        sopt_bellman_array_part2_test,
        sopt_bellman_array_part3_test,
        sopt_bellman_array_rec_test,
        sopt_bellman_list_test,
        sopt_bellman_list_rec_test,
};

std::vector<knapsack::Output (*)(Instance&)> f_opt {
        opt_bellman_array_test,
        opt_bellman_list_sort_test,
};

TEST(bellman, TEST_SOPT)  { test(TEST, f_sopt, SOPT); }
TEST(bellman, SMALL_SOPT) { test(SMALL, f_sopt, SOPT); }
TEST(bellman, TEST_OPT)  { test(TEST, f_opt, OPT); }
TEST(bellman, SMALL_OPT) { test(SMALL, f_opt, OPT); }

