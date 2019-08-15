#include "knapsack/lib/tester.hpp"
#include "knapsack/opt_minknap/minknap.hpp"
#include "knapsack/opt_bellman/bellman.hpp"
#include "knapsack/lib/generator.hpp"

using namespace knapsack;

Profit opt_bellman_array_test(Instance& ins) { return opt_bellman_array(ins); }

Profit opt_minknap_1_test(Instance& ins)
{
    Info info = Info()
        //.set_log2stderr(true)
        ;
    auto p = MinknapParams::pure();
    p.k = 1;
    return sopt_minknap(ins, p, info).profit();
}

Profit opt_minknap_2_test(Instance& ins)
{
    Info info = Info()
        //.set_log2stderr(true)
        ;
    auto p = MinknapParams::pure();
    p.k = 2;
    return sopt_minknap(ins, p, info).profit();
}

Profit opt_minknap_3_test(Instance& ins)
{
    Info info = Info()
        //.set_log2stderr(true)
        ;
    auto p = MinknapParams::pure();
    p.k = 3;
    return sopt_minknap(ins, p, info).profit();
}

Profit opt_minknap_combocore_k1_test(Instance& ins)
{
    Info info = Info()
        .set_verbose(true)
        .set_log2stderr(true)
        ;
    auto p = MinknapParams::pure();
    p.k = 1;
    p.combo_core = true;
    return sopt_minknap(ins, p, info).profit();
}

Profit opt_minknap_combocore_k2_test(Instance& ins)
{
    Info info = Info()
        .set_verbose(true)
        .set_log2stderr(true)
        ;
    auto p = MinknapParams::pure();
    p.k = 2;
    p.combo_core = true;
    return sopt_minknap(ins, p, info).profit();
}

Profit opt_minknap_combocore_k3_test(Instance& ins)
{
    Info info = Info()
        .set_verbose(true)
        .set_log2stderr(true)
        ;
    auto p = MinknapParams::pure();
    p.k = 3;
    p.combo_core = true;
    return sopt_minknap(ins, p, info).profit();
}

Profit opt_minknap_pairing_test(Instance& ins)
{
    Info info = Info()
        .set_verbose(true)
        .set_log2stderr(true)
        ;
    auto p = MinknapParams::pure();
    p.pairing = 10;
    return sopt_minknap(ins, p, info).profit();
}

std::vector<Profit (*)(Instance&)> f = {
        opt_bellman_array_test,
        opt_minknap_1_test,
        opt_minknap_2_test,
        opt_minknap_3_test,
        opt_minknap_combocore_k1_test,
        opt_minknap_combocore_k2_test,
        opt_minknap_combocore_k3_test,
        opt_minknap_pairing_test,
};

TEST(minknap, TEST)   { test(TEST, f); }
TEST(minknap, SMALL)  { test(SMALL, f); }
TEST(minknap, MEDIUM) { test(MEDIUM, f); }

