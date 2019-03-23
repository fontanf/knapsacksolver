#include "knapsack/lib/tester.hpp"
#include "knapsack/opt_minknap/minknap.hpp"
#include "knapsack/opt_bab/bab.hpp"

using namespace knapsack;

Profit opt_bab_test(Instance& ins)     { return sopt_bab(ins).profit(); }

std::vector<Profit (*)(Instance&)> f = {
        opt_minknap,
        opt_bab_test,
};

TEST(bab, TEST)  { test(TEST, f); }
TEST(bab, SMALL) { test(SMALL, f); }

