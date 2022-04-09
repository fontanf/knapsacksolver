#include "knapsacksolver/tester.hpp"
#include "knapsacksolver/algorithms/minknap.hpp"
#include "knapsacksolver/algorithms/greedy.hpp"

using namespace knapsacksolver;

Output minknap_test(Instance& instance)
{
    return minknap(instance);
}

Output greedy_test(Instance& instance)
{
    Info info;
    instance.sort_partially(FFOT_DBG(info));
    return greedy(instance, info);
}

std::vector<Output (*)(Instance&)> f = {
        minknap_test,
        greedy_test,
};

TEST(greedy, TEST)  { test(TEST, f, LB); }
TEST(greedy, SMALL) { test(SMALL, f, LB); }

