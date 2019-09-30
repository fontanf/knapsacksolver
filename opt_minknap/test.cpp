#include "knapsack/lib/tester.hpp"
#include "knapsack/opt_minknap/minknap.hpp"
#include "knapsack/opt_bellman/bellman.hpp"
#include "knapsack/lib/generator.hpp"

using namespace knapsack;

knapsack::Output opt_bellman_array_test(Instance& ins)
{
    Info info = Info()
        //.set_verbose(true)
        //.set_log2stderr(true)
        ;
    return opt_bellman_array(ins, info);
}

knapsack::Output sopt_minknap_1_test(Instance& ins)
{
    Info info = Info()
        //.set_verbose(true)
        //.set_log2stderr(true)
        ;
    MinknapOptionalParameters p;
    p.info = info;
    p.pure();
    p.partial_solution_size = 1;
    return sopt_minknap(ins, p);
}

knapsack::Output sopt_minknap_2_test(Instance& ins)
{
    Info info = Info()
        //.set_verbose(true)
        //.set_log2stderr(true)
        ;
    MinknapOptionalParameters p;
    p.info = info;
    p.pure();
    p.partial_solution_size = 2;
    return sopt_minknap(ins, p);
}

knapsack::Output sopt_minknap_3_test(Instance& ins)
{
    Info info = Info()
        //.set_verbose(true)
        //.set_log2stderr(true)
        ;
    MinknapOptionalParameters p;
    p.info = info;
    p.pure();
    p.partial_solution_size = 3;
    return sopt_minknap(ins, p);
}

knapsack::Output sopt_minknap_combocore_k1_test(Instance& ins)
{
    Info info = Info()
        //.set_verbose(true)
        //.set_log2stderr(true)
        ;
    MinknapOptionalParameters p;
    p.info = info;
    p.pure();
    p.partial_solution_size = 1;
    p.combo_core = true;
    return sopt_minknap(ins, p);
}

knapsack::Output sopt_minknap_combocore_k2_test(Instance& ins)
{
    Info info = Info()
        //.set_verbose(true)
        //.set_log2stderr(true)
        ;
    MinknapOptionalParameters p;
    p.info = info;
    p.pure();
    p.partial_solution_size = 2;
    p.combo_core = true;
    return sopt_minknap(ins, p);
}

knapsack::Output sopt_minknap_combocore_k3_test(Instance& ins)
{
    Info info = Info()
        //.set_verbose(true)
        //.set_log2stderr(true)
        ;
    MinknapOptionalParameters p;
    p.info = info;
    p.pure();
    p.partial_solution_size = 3;
    p.combo_core = true;
    return sopt_minknap(ins, p);
}

knapsack::Output sopt_minknap_pairing_test(Instance& ins)
{
    Info info = Info()
        //.set_verbose(true)
        //.set_log2stderr(true)
        ;
    MinknapOptionalParameters p;
    p.info = info;
    p.pure();
    p.pairing = 10;
    return sopt_minknap(ins, p);
}

std::vector<knapsack::Output (*)(Instance&)> f = {
        opt_bellman_array_test,
        sopt_minknap_1_test,
        sopt_minknap_2_test,
        sopt_minknap_3_test,
        sopt_minknap_combocore_k1_test,
        sopt_minknap_combocore_k2_test,
        sopt_minknap_combocore_k3_test,
        sopt_minknap_pairing_test,
};

TEST(minknap, TEST)   { test(TEST, f); }
TEST(minknap, SMALL)  { test(SMALL, f); }
TEST(minknap, MEDIUM) { test(MEDIUM, f); }

