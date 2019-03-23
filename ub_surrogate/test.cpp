#include "knapsack/lib/tester.hpp"
#include "knapsack/opt_minknap/minknap.hpp"
#include "knapsack/ub_surrogate/surrogate.hpp"
#include "knapsack/lb_greedy/greedy.hpp"
#include "knapsack/lb_greedynlogn/greedynlogn.hpp"

using namespace knapsack;

Profit ub_surrogate_test(Instance& ins)
{
    Info info;
    ins.sort_partially(info);
    Profit lb = sol_greedy(ins).profit();
    return ub_surrogate(ins, lb, info).ub;
}

std::vector<Profit (*)(Instance&)> f = {
        opt_minknap,
        ub_surrogate_test,
};

TEST(surrogate, TEST)  { test(TEST, f, UB); }
TEST(surrogate, SMALL) { test(SMALL, f, UB); }

