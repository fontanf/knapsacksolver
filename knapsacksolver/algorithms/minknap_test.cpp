#include "knapsacksolver/tester.hpp"
#include "knapsacksolver/algorithms/minknap.hpp"
#include "knapsacksolver/algorithms/bellman.hpp"
#include "knapsacksolver/generator.hpp"

using namespace knapsacksolver;

Output bellman_array_test(Instance& ins)
{
    Info info = Info()
        //.set_verbose(true)
        //.set_log2stderr(true)
        ;
    return bellman_array(ins, info);
}

Output minknap_1_test(Instance& ins)
{
    Info info = Info()
        //.set_verbose(true)
        //.set_log2stderr(true)
        ;
    auto p = MinknapOptionalParameters().set_pure();
    p.info = info;
    p.partial_solution_size = 1;
    return minknap(ins, p);
}

Output minknap_2_test(Instance& ins)
{
    Info info = Info()
        //.set_verbose(true)
        //.set_log2stderr(true)
        ;
    auto p = MinknapOptionalParameters().set_pure();
    p.info = info;
    p.partial_solution_size = 2;
    return minknap(ins, p);
}

Output minknap_3_test(Instance& ins)
{
    Info info = Info()
        //.set_verbose(true)
        //.set_log2stderr(true)
        ;
    auto p = MinknapOptionalParameters().set_pure();
    p.info = info;
    p.partial_solution_size = 3;
    return minknap(ins, p);
}

Output minknap_combocore_k1_test(Instance& ins)
{
    Info info = Info()
        //.set_verbose(true)
        //.set_log2stderr(true)
        ;
    auto p = MinknapOptionalParameters().set_pure();
    p.info = info;
    p.partial_solution_size = 1;
    p.combo_core = true;
    return minknap(ins, p);
}

Output minknap_combocore_k2_test(Instance& ins)
{
    Info info = Info()
        //.set_verbose(true)
        //.set_log2stderr(true)
        ;
    auto p = MinknapOptionalParameters().set_pure();
    p.info = info;
    p.partial_solution_size = 2;
    p.combo_core = true;
    return minknap(ins, p);
}

Output minknap_combocore_k3_test(Instance& ins)
{
    Info info = Info()
        //.set_verbose(true)
        //.set_log2stderr(true)
        ;
    auto p = MinknapOptionalParameters().set_pure();
    p.info = info;
    p.partial_solution_size = 3;
    p.combo_core = true;
    return minknap(ins, p);
}

Output minknap_pairing_test(Instance& ins)
{
    Info info = Info()
        //.set_verbose(true)
        //.set_log2stderr(true)
        ;
    auto p = MinknapOptionalParameters().set_pure();
    p.info = info;
    p.pairing = 10;
    return minknap(ins, p);
}

std::vector<Output (*)(Instance&)> f = {
        bellman_array_test,
        minknap_1_test,
        minknap_2_test,
        minknap_3_test,
        minknap_combocore_k1_test,
        minknap_combocore_k2_test,
        minknap_combocore_k3_test,
        minknap_pairing_test,
};

TEST(minknap, TEST)   { test(TEST, f); }
TEST(minknap, SMALL)  { test(SMALL, f); }
TEST(minknap, MEDIUM) { test(MEDIUM, f); }

