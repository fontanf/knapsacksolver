#include "knapsacksolver/tester.hpp"
#include "knapsacksolver/algorithms/minknap.hpp"
#include "knapsacksolver/algorithms/greedy.hpp"

using namespace knapsacksolver;

Output minknap_test(Instance& ins)
{
    return minknap(ins);
}

Output greedy_test(Instance& ins)
{
    Info info;
    ins.sort_partially(info);
    return greedy(ins, info);
}

std::vector<Output (*)(Instance&)> f = {
        minknap_test,
        greedy_test,
};

TEST(greedy, TEST)  { test(TEST, f, LB); }
TEST(greedy, SMALL) { test(SMALL, f, LB); }

