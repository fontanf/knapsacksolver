#include "knapsacksolver/tester.hpp"
#include "knapsacksolver/algorithms/dynamic_programming_primal_dual.hpp"
#include "knapsacksolver/algorithms/greedy.hpp"

using namespace knapsacksolver;

Output dynamic_programming_primal_dual_test(Instance& instance)
{
    return dynamic_programming_primal_dual(instance);
}

Output greedy_test(Instance& instance)
{
    Info info;
    instance.sort_partially(FFOT_DBG(info));
    return greedy(instance, info);
}

std::vector<Output (*)(Instance&)> f = {
        dynamic_programming_primal_dual_test,
        greedy_test,
};

TEST(greedy, TEST)  { test(TEST, f, LB); }
TEST(greedy, SMALL) { test(SMALL, f, LB); }

