#include "knapsacksolver/tester.hpp"
#include "knapsacksolver/algorithms/bellman.hpp"
#include "knapsacksolver/algorithms/dpprofits.hpp"

using namespace knapsacksolver;

Output bellman_array_test(Instance& ins)       { return bellman_array(ins); }
Output dpprofits_array_test(Instance& ins)     { return dpprofits_array(ins); }
Output dpprofits_array_all_test(Instance& ins) { return dpprofits_array_all(ins); }

std::vector<Output (*)(Instance&)> f_opt {
        bellman_array_test,
        dpprofits_array_test,
};

std::vector<Output (*)(Instance&)> f_sopt {
        bellman_array_test,
        dpprofits_array_all_test,
};

TEST(dpprofits, TEST_SOPT)  { test(TEST, f_sopt, SOPT); }
TEST(dpprofits, SMALL_SOPT) { test(SMALL, f_sopt, SOPT); }
TEST(dpprofits, TEST_OPT)  { test(TEST, f_opt, OPT); }
TEST(dpprofits, SMALL_OPT) { test(SMALL, f_opt, OPT); }

