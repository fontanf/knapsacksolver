#include "knapsack/lib/tester.hpp"
#include "knapsack/opt_minknap/minknap.hpp"
#include "knapsack/opt_astar/astar.hpp"

using namespace knapsack;

Profit opt_minknap_test(Instance& ins)  { Logger logger; Info info(logger, true); return sopt_minknap(ins, info).profit(); }
Profit opt_astar_test(Instance& ins)    { Logger logger; Info info(logger, true); return sopt_astar(ins, info).profit(); }
Profit opt_astar_dp_test(Instance& ins) { Logger logger; Info info(logger, true); return sopt_astar_dp(ins, info).profit(); }

std::vector<Profit (*)(Instance&)> f = {
        opt_minknap_test,
        opt_astar_test,
        opt_astar_dp_test,
};

TEST(astar, TEST)  { test(TEST, f); }
TEST(astar, SMALL) { test(SMALL, f); }

