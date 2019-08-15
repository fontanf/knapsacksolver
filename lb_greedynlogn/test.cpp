#include "knapsack/lib/tester.hpp"
#include "knapsack/opt_minknap/minknap.hpp"
#include "knapsack/lb_greedynlogn/greedynlogn.hpp"

using namespace knapsack;

Profit opt_minknap(Instance& ins)                  { Info info; return sopt_minknap(ins, MinknapParams(), info).profit(); }
Profit opt_forwardgreedynlogn_test(Instance& ins)  { Info info; ins.sort_partially(info); return sol_forwardgreedynlogn(ins, info).profit(); }
Profit opt_backwardgreedynlogn_test(Instance& ins) { Info info; ins.sort_partially(info); return sol_backwardgreedynlogn(ins, info).profit(); }
Profit opt_greedynlogn_test(Instance& ins)         { Info info; ins.sort_partially(info); return sol_greedynlogn(ins, info).profit(); }

std::vector<Profit (*)(Instance&)> f = {
        opt_minknap,
        opt_forwardgreedynlogn_test,
        opt_backwardgreedynlogn_test,
        opt_greedynlogn_test,
};

TEST(greedynlogn, TEST)  { test(TEST, f, LB); }
TEST(greedynlogn, SMALL) { test(SMALL, f, LB); }

