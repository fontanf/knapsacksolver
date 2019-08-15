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
    return Minknap(ins, p).run(info).profit();
}

Profit opt_minknap_2_test(Instance& ins)
{
    Info info = Info()
        //.set_log2stderr(true)
        ;
    auto p = MinknapParams::pure();
    p.k = 2;
    return Minknap(ins, p).run(info).profit();
}

Profit opt_minknap_3_test(Instance& ins)
{
    Info info = Info()
        //.set_log2stderr(true)
        ;
    auto p = MinknapParams::pure();
    p.k = 3;
    return Minknap(ins, p).run(info).profit();
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
    return Minknap(ins, p).run(info).profit();
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
    return Minknap(ins, p).run(info).profit();
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
    return Minknap(ins, p).run(info).profit();
}

Profit opt_minknap_pairing_test(Instance& ins)
{
    Info info = Info()
        .set_verbose(true)
        .set_log2stderr(true)
        ;
    auto p = MinknapParams::pure();
    p.pairing = 10;
    return Minknap(ins, p).run(info).profit();
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

TEST(minknap, TEST1)
{
    Generator data;
    data.n = 500;
    data.t = "asc";
    data.r = 10000;
    data.h = 31;
    data.s = 10531;
    Instance ins = data.generate();
    Info info = Info()
        //.set_verbose(true)
        //.set_log2stderr(true)
        ;
    Profit opt = Minknap(ins, MinknapParams::combo()).run(info).profit();
    EXPECT_EQ(opt, 1083713);
}

TEST(minknap, TEST2)
{
    Generator data;
    data.n = 1000;
    data.t = "wc";
    data.r = 10000;
    data.h = 6;
    data.s = 11006;
    Instance ins = data.generate();
    Info info = Info()
        //.set_verbose(true)
        ;
    Profit opt = Minknap(ins, MinknapParams::pure()).run(info).profit();
    EXPECT_EQ(opt, 398832);
}

