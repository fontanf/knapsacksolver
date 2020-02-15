#include "knapsacksolver/tester.hpp"
#include "knapsacksolver/algorithms/minknap.hpp"
#include "knapsacksolver/algorithms/surrelax.hpp"
#include "knapsacksolver/algorithms/greedy.hpp"
#include "knapsacksolver/algorithms/greedynlogn.hpp"

using namespace knapsacksolver;

/*
Profit surrogate_test(Instance& ins)
{
    Info info;
    ins.sort_partially(info);
    Profit lb = greedy(ins).profit();
    return surrogate(ins, lb, info).ub;
}

std::vector<Profit (*)(Instance&)> f = {
        minknap,
        surrogate_test,
};

TEST(surrogate, TEST)  { test(TEST, f, UB); }
TEST(surrogate, SMALL) { test(SMALL, f, UB); }

*/
