#include "knapsack/lib/tester.hpp"
#include "knapsack/opt_minknap/minknap.hpp"
#include "knapsack/lb_greedy/greedy.hpp"

using namespace knapsack;

Profit opt_minknap(Instance& ins)      { Info info; return sopt_minknap(ins, MinknapParams(), info).profit(); }
Profit opt_greedy_test(Instance& ins)  { Info info; ins.sort_partially(info); return sol_greedy(ins, info).profit(); }

std::vector<Profit (*)(Instance&)> f = {
        opt_minknap,
        opt_greedy_test,
};

TEST(greedy, TEST)  { test(TEST, f, LB); }
TEST(greedy, SMALL) { test(SMALL, f, LB); }

