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

Profit opt_expknap_combocore_test(Instance& ins)
{
    Info info = Info()
        .set_verbose(true)
        .set_log2stderr(true)
        ;
    ExpknapParams p = ExpknapParams::pure();
    p.combo_core = true;
    return Expknap(ins, p).run(info).profit();
}

std::vector<Profit (*)(Instance&)> f = {
        opt_minknap,
        opt_expknap_test,
        opt_expknap_combocore_test,
};

TEST(expknap, TEST)  { test(TEST, f); }
TEST(expknap, SMALL) { test(SMALL, f); }

