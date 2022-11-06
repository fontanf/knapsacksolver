#include "knapsacksolver/tester.hpp"
#include "knapsacksolver/algorithms/dynamic_programming_bellman.hpp"
#include "knapsacksolver/algorithms/dynamic_programming_profits.hpp"

using namespace knapsacksolver;

Output dynamic_programming_bellman_array_test(Instance& instance) { return dynamic_programming_bellman_array(instance); }
Output dynamic_programming_profits_array_test(Instance& instance) { return dynamic_programming_profits_array(instance); }
Output dynamic_programming_profits_array_all_test(Instance& instance) { return dynamic_programming_profits_array_all(instance); }

std::vector<Output (*)(Instance&)> f_opt {
        dynamic_programming_bellman_array_test,
        dynamic_programming_profits_array_test,
};

std::vector<Output (*)(Instance&)> f_sopt {
        dynamic_programming_bellman_array_test,
        dynamic_programming_profits_array_all_test,
};

TEST(dynamic_programming_profits, TEST_SOPT)  { test(TEST, f_sopt, SOPT); }
TEST(dynamic_programming_profits, SMALL_SOPT) { test(SMALL, f_sopt, SOPT); }
TEST(dynamic_programming_profits, TEST_OPT)  { test(TEST, f_opt, OPT); }
TEST(dynamic_programming_profits, SMALL_OPT) { test(SMALL, f_opt, OPT); }

