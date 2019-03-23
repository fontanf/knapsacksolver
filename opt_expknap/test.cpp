#include "knapsack/lib/tester.hpp"
#include "knapsack/opt_minknap/minknap.hpp"
#include "knapsack/opt_expknap/expknap.hpp"

using namespace knapsack;

Profit opt_expknap_test(Instance& ins) { return Expknap(ins, ExpknapParams::pure()).run().profit(); }

std::vector<Profit (*)(Instance&)> f = {
        opt_minknap,
        opt_expknap_test,
};

TEST(expknap, TEST)  { test(TEST, f); }
TEST(expknap, SMALL) { test(SMALL, f); }

