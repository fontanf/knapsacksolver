#include "knapsack/lib/tester.hpp"
#include "knapsack/opt_minknap/minknap.hpp"
#include "knapsack/opt_bellman/bellman.hpp"

using namespace knapsack;


Profit opt_bellman_array_test(Instance& ins) { Logger logger; Info info(logger, true); return opt_bellman_array(ins, info); }
Profit opt_minknap_1_test(Instance& ins)     { Logger logger; Info info(logger, true); return sopt_minknap(ins, info, MinknapParams(), 1).profit(); }
Profit opt_minknap_2_test(Instance& ins)     { Logger logger; Info info(logger, true); return sopt_minknap(ins, info, MinknapParams(), 2).profit(); }
Profit opt_minknap_3_test(Instance& ins)     { Logger logger; Info info(logger, true); return sopt_minknap(ins, info, MinknapParams(), 3).profit(); }

std::vector<Profit (*)(Instance&)> f = {
        opt_bellman_array_test,
        opt_minknap_1_test,
        opt_minknap_2_test,
        opt_minknap_3_test,
};

TEST(minknap, TEST)  { test(TEST, f); }
TEST(minknap, SMALL) { test(SMALL, f); }

