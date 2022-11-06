#include "knapsacksolver/tester.hpp"
#include "knapsacksolver/algorithms/dynamic_programming_primal_dual.hpp"
#include "knapsacksolver/algorithms/surrogate_relaxation.hpp"
#include "knapsacksolver/algorithms/greedy.hpp"
#include "knapsacksolver/algorithms/greedy_nlogn.hpp"

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
        dynamic_programming_primal_dual,
        surrogate_test,
};

TEST(surrogate, TEST)  { test(TEST, f, UB); }
TEST(surrogate, SMALL) { test(SMALL, f, UB); }

*/
