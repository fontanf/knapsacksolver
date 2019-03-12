#include "knapsack/lib/tester.hpp"
#include "knapsack/opt_minknap/minknap.hpp"
#include "knapsack/opt_bab/bab.hpp"

using namespace knapsack;

Profit opt_minknap_test(Instance& ins) { Logger logger; Info info(logger, true); return Minknap(ins, MinknapParams()).run(info).profit(); }
Profit opt_bab_test(Instance& ins)     { Logger logger; Info info(logger, true); return sopt_bab(ins, info).profit(); }

std::vector<Profit (*)(Instance&)> f = {
        opt_minknap_test,
        opt_bab_test,
};

TEST(bab, TEST)  { test(TEST, f); }
TEST(bab, SMALL) { test(SMALL, f); }

