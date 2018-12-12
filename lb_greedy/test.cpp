#include "knapsack/lib/tester.hpp"
#include "knapsack/opt_minknap/minknap.hpp"
#include "knapsack/lb_greedy/greedy.hpp"

using namespace knapsack;

Profit opt_minknap_test(Instance& ins) { Logger logger; Info info(logger, true); return sopt_minknap(ins, info).profit(); }
Profit opt_greedy_test(Instance& ins)  { Logger logger; Info info(logger, true); ins.sort_partially(); return sol_greedy(ins, info).profit(); }

std::vector<Profit (*)(Instance&)> f = {
        opt_minknap_test,
        opt_greedy_test,
};

TEST(Bellman, TEST)  { test(TEST, f, LB); }
TEST(Bellman, SMALL) { test(SMALL, f, LB); }

