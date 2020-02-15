#include "knapsacksolver/tester.hpp"
#include "knapsacksolver/algorithms/minknap.hpp"
#include "knapsacksolver/algorithms/greedynlogn.hpp"

using namespace knapsacksolver;

Output minknap_test(Instance& ins)
{
    return minknap(ins);
}

Output forwardgreedynlogn_test(Instance& ins)
{
    Info info;
    ins.sort_partially(info);
    return forwardgreedynlogn(ins, info);
}

Output backwardgreedynlogn_test(Instance& ins)
{
    Info info;
    ins.sort_partially(info);
    return backwardgreedynlogn(ins, info);
}

Output greedynlogn_test(Instance& ins)
{
    Info info; ins.sort_partially(info);
    return greedynlogn(ins, info);
}

std::vector<Output (*)(Instance&)> f = {
        minknap_test,
        forwardgreedynlogn_test,
        backwardgreedynlogn_test,
        greedynlogn_test,
};

TEST(greedynlogn, TEST)  { test(TEST, f, LB); }
TEST(greedynlogn, SMALL) { test(SMALL, f, LB); }

