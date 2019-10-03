#include "knapsack/lib/tester.hpp"
#include "knapsack/opt_bellman/bellman.hpp"
#include "knapsack/opt_dpprofits/dpprofits.hpp"

using namespace knapsack;

Output  opt_bellman_array_test(Instance& ins)       { return opt_bellman_array(ins); }
Output  opt_dpprofits_array_test(Instance& ins)     { return opt_dpprofits_array(ins); }
Output sopt_dpprofits_array_all_test(Instance& ins) { return sopt_dpprofits_array_all(ins); }

std::vector<Output (*)(Instance&)> f_opt {
        opt_bellman_array_test,
        opt_dpprofits_array_test,
};

std::vector<Output (*)(Instance&)> f_sopt {
        opt_bellman_array_test,
        sopt_dpprofits_array_all_test,
};

TEST(dpprofits, TEST_SOPT)  { test(TEST, f_sopt, SOPT); }
TEST(dpprofits, SMALL_SOPT) { test(SMALL, f_sopt, SOPT); }
TEST(dpprofits, TEST_OPT)  { test(TEST, f_opt, OPT); }
TEST(dpprofits, SMALL_OPT) { test(SMALL, f_opt, OPT); }

