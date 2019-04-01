#include "knapsack/lib/tester.hpp"
#include "knapsack/opt_minknap/minknap.hpp"
#include "knapsack/opt_expknap/expknap.hpp"

using namespace knapsack;

Profit opt_expknap_test(Instance& ins)
{
    Info info = Info()
        .set_verbose(true)
        .set_log2stderr(true)
        ;
    return Expknap(ins, ExpknapParams::pure()).run(info).profit();
}

std::vector<Profit (*)(Instance&)> f = {
        opt_minknap,
        opt_expknap_test,
};

TEST(expknap, TEST)  { test(TEST, f); }
TEST(expknap, SMALL) { test(SMALL, f); }

