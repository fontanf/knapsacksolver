#include "knapsacksolver/tester.hpp"
#include "knapsacksolver/algorithms/minknap.hpp"
#include "knapsacksolver/algorithms/greedynlogn.hpp"

using namespace knapsacksolver;

Output minknap_test(Instance& instance)
{
    return minknap(instance);
}

Output forwardgreedynlogn_test(Instance& instance)
{
    Info info;
    instance.sort_partially(FFOT_DBG(info));
    return forwardgreedynlogn(instance, info);
}

Output backwardgreedynlogn_test(Instance& instance)
{
    Info info;
    instance.sort_partially(FFOT_DBG(info));
    return backwardgreedynlogn(instance, info);
}

Output greedynlogn_test(Instance& instance)
{
    Info info; instance.sort_partially(FFOT_DBG(info));
    return greedynlogn(instance, info);
}

std::vector<Output (*)(Instance&)> f = {
        minknap_test,
        forwardgreedynlogn_test,
        backwardgreedynlogn_test,
        greedynlogn_test,
};

TEST(greedynlogn, TEST)  { test(TEST, f, LB); }
TEST(greedynlogn, SMALL) { test(SMALL, f, LB); }

