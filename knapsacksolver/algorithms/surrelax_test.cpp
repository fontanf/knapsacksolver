#include "knapsacksolver/tester.hpp"
#include "knapsacksolver/algorithms/minknap.hpp"
#include "knapsacksolver/algorithms/surrelax.hpp"
#include "knapsacksolver/algorithms/greedy.hpp"
#include "knapsacksolver/algorithms/greedynlogn.hpp"

using namespace knapsacksolver;

/*
Profit surrogate_test(Instance& instance)
{
    Info info;
    instance.sort_partially(info);
    Profit lb = greedy(instance).profit();
    return surrogate(instance, lb, info).ub;
}

std::vector<Profit (*)(Instance&)> f = {
        minknap,
        surrogate_test,
};

TEST(surrogate, TEST)  { test(TEST, f, UB); }
TEST(surrogate, SMALL) { test(SMALL, f, UB); }

*/
