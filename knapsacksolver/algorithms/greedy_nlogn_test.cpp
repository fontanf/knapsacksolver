#include "knapsacksolver/tester.hpp"
#include "knapsacksolver/algorithms/dynamic_programming_primal_dual.hpp"
#include "knapsacksolver/algorithms/greedy_nlogn.hpp"

using namespace knapsacksolver;

Output dynamic_programming_primal_dual_test(Instance& instance)
{
    return dynamic_programming_primal_dual(instance);
}

Output greedy_nlogn_forward_test(Instance& instance)
{
    Info info;
    instance.sort_partially(FFOT_DBG(info));
    return greedy_nlogn_forward(instance, info);
}

Output greedy_nlogn_backward_test(Instance& instance)
{
    Info info;
    instance.sort_partially(FFOT_DBG(info));
    return greedy_nlogn_backward(instance, info);
}

Output greedy_nlogn_test(Instance& instance)
{
    Info info; instance.sort_partially(FFOT_DBG(info));
    return greedy_nlogn(instance, info);
}

std::vector<Output (*)(Instance&)> f = {
        dynamic_programming_primal_dual_test,
        greedy_nlogn_forward_test,
        greedy_nlogn_backward_test,
        greedy_nlogn_test,
};

TEST(greedy_nlogn, TEST)  { test(TEST, f, LB); }
TEST(greedy_nlogn, SMALL) { test(SMALL, f, LB); }

