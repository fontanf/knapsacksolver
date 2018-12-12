#include "knapsack/lib/tester.hpp"
#include "knapsack/opt_minknap/minknap.hpp"
#include "knapsack/opt_expknap/expknap.hpp"

using namespace knapsack;

Profit opt_minknap_test(Instance& ins) { Logger logger; Info info(logger, true); return sopt_minknap(ins, info).profit(); }
Profit opt_expknap_test(Instance& ins) { Logger logger; Info info(logger, true); return sopt_expknap(ins, info).profit(); }

std::vector<Profit (*)(Instance&)> f = {
        opt_minknap_test,
        opt_expknap_test,
};

TEST(Bellman, TEST)  { test(TEST, f); }
TEST(Bellman, SMALL) { test(SMALL, f); }

