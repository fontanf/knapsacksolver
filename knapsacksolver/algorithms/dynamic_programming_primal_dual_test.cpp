#include "knapsacksolver/tester.hpp"
#include "knapsacksolver/algorithms/dynamic_programming_primal_dual.hpp"
#include "knapsacksolver/algorithms/dynamic_programming_bellman.hpp"
#include "knapsacksolver/generator.hpp"

using namespace knapsacksolver;

Output dynamic_programming_bellman_array_test(Instance& instance)
{
    Info info = Info()
        //.set_verbosity_level(1)
        //.set_log2stderr(true)
        ;
    return dynamic_programming_bellman_array(instance, info);
}

Output dynamic_programming_primal_dual_1_test(Instance& instance)
{
    Info info = Info()
        //.set_verbosity_level(1)
        //.set_log2stderr(true)
        ;
    auto p = DynamicProgrammingPrimalDualOptionalParameters().set_pure();
    p.info = info;
    p.partial_solution_size = 1;
    return dynamic_programming_primal_dual(instance, p);
}

Output dynamic_programming_primal_dual_2_test(Instance& instance)
{
    Info info = Info()
        //.set_verbosity_level(1)
        //.set_log2stderr(true)
        ;
    auto p = DynamicProgrammingPrimalDualOptionalParameters().set_pure();
    p.info = info;
    p.partial_solution_size = 2;
    return dynamic_programming_primal_dual(instance, p);
}

Output dynamic_programming_primal_dual_3_test(Instance& instance)
{
    Info info = Info()
        //.set_verbosity_level(1)
        //.set_log2stderr(true)
        ;
    auto p = DynamicProgrammingPrimalDualOptionalParameters().set_pure();
    p.info = info;
    p.partial_solution_size = 3;
    return dynamic_programming_primal_dual(instance, p);
}

Output dynamic_programming_primal_dual_combocore_k1_test(Instance& instance)
{
    Info info = Info()
        //.set_verbosity_level(1)
        //.set_log2stderr(true)
        ;
    auto p = DynamicProgrammingPrimalDualOptionalParameters().set_pure();
    p.info = info;
    p.partial_solution_size = 1;
    p.combo_core = true;
    return dynamic_programming_primal_dual(instance, p);
}

Output dynamic_programming_primal_dual_combocore_k2_test(Instance& instance)
{
    Info info = Info()
        //.set_verbosity_level(1)
        //.set_log2stderr(true)
        ;
    auto p = DynamicProgrammingPrimalDualOptionalParameters().set_pure();
    p.info = info;
    p.partial_solution_size = 2;
    p.combo_core = true;
    return dynamic_programming_primal_dual(instance, p);
}

Output dynamic_programming_primal_dual_combocore_k3_test(Instance& instance)
{
    Info info = Info()
        //.set_verbosity_level(1)
        //.set_log2stderr(true)
        ;
    auto p = DynamicProgrammingPrimalDualOptionalParameters().set_pure();
    p.info = info;
    p.partial_solution_size = 3;
    p.combo_core = true;
    return dynamic_programming_primal_dual(instance, p);
}

Output dynamic_programming_primal_dual_pairing_test(Instance& instance)
{
    Info info = Info()
        //.set_verbosity_level(1)
        //.set_log2stderr(true)
        ;
    auto p = DynamicProgrammingPrimalDualOptionalParameters().set_pure();
    p.info = info;
    p.pairing = 10;
    return dynamic_programming_primal_dual(instance, p);
}

std::vector<Output (*)(Instance&)> f = {
        dynamic_programming_bellman_array_test,
        dynamic_programming_primal_dual_1_test,
        dynamic_programming_primal_dual_2_test,
        dynamic_programming_primal_dual_3_test,
        dynamic_programming_primal_dual_combocore_k1_test,
        dynamic_programming_primal_dual_combocore_k2_test,
        dynamic_programming_primal_dual_combocore_k3_test,
        dynamic_programming_primal_dual_pairing_test,
};

TEST(dynamic_programming_primal_dual, TEST)   { test(TEST, f); }
TEST(dynamic_programming_primal_dual, SMALL)  { test(SMALL, f); }
TEST(dynamic_programming_primal_dual, MEDIUM) { test(MEDIUM, f); }

