#include "knapsack/lib/tester.hpp"
#include "knapsack/opt_minknap/minknap.hpp"
#include "knapsack/opt_expknap/expknap.hpp"

using namespace knapsack;

Profit opt_minknap_test(Instance& ins) { Logger logger; Info info(logger, true); return Minknap(ins, MinknapParams()).run(info).profit(); }
Profit opt_expknap_test(Instance& ins) { Logger logger; Info info(logger, true); return Expknap(ins, ExpknapParams::pure()).run(info).profit(); }

std::vector<Profit (*)(Instance&)> f = {
        opt_minknap_test,
        opt_expknap_test,
};

TEST(expknap, TEST)  { test(TEST, f); }
TEST(expknap, SMALL) { test(SMALL, f); }

