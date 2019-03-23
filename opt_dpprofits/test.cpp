#include "knapsack/lib/tester.hpp"
#include "knapsack/opt_bellman/bellman.hpp"
#include "knapsack/opt_dpprofits/dpprofits.hpp"

using namespace knapsack;

Profit opt_bellman_array_test(Instance& ins)         { return opt_bellman_array(ins); }
Profit opt_dpprofits_array_test(Instance& ins)       { return opt_dpprofits_array(ins); }
Profit opt_dpprofits_array_all_test(Instance& ins)   { return sopt_dpprofits_array_all(ins).profit(); }

std::vector<Profit (*)(Instance&)> tested_functions()
{
    return {
        opt_bellman_array_test,
        opt_dpprofits_array_test,
        opt_dpprofits_array_all_test,
    };
}

TEST(dpprofits, TEST)  { test(TEST, tested_functions()); }
TEST(dpprofits, SMALL) { test(SMALL, tested_functions()); }

