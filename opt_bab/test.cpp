#include "knapsack/lib/tester.hpp"
#include "knapsack/opt_minknap/minknap.hpp"
#include "knapsack/opt_bab/bab.hpp"

using namespace knapsack;

Profit opt_minknap(Instance& ins)       { Info info; return sopt_minknap(ins, MinknapParams(), info).profit(); }
Profit opt_bab_test(Instance& ins)      { Info info = Info().set_verbose(false); return sopt_bab(ins, false, info).profit(); }
Profit opt_bab_sort_test(Instance& ins) { Info info = Info().set_verbose(false); return sopt_bab(ins, true, info).profit(); }

std::vector<Profit (*)(Instance&)> f = {
        opt_minknap,
        opt_bab_test,
        opt_bab_sort_test,
};

TEST(bab, TEST)  { test(TEST, f); }
TEST(bab, SMALL) { test(SMALL, f); }

