#include "knapsacksolver/tester.hpp"
#include "knapsacksolver/algorithms/dynamic_programming_primal_dual.hpp"
#include "knapsacksolver/algorithms/branch_and_bound_primal_dual.hpp"

using namespace knapsacksolver;

Output dynamic_programming_primal_dual_test(Instance& instance)
{
    Info info = Info()
        //.set_verbosity_level(1)
        ;
    DynamicProgrammingPrimalDualOptionalParameters p;
    p.info = info;
    return dynamic_programming_primal_dual(instance, p);
}

Output branch_and_bound_primal_dual_test(Instance& instance)
{
    Info info = Info()
        //.set_verbosity_level(1)
        //.set_log2stderr(true)
        ;
    BranchAndBoundPrimalDualOptionalParameters p;
    p.info = info;
    return branch_and_bound_primal_dual(instance, p);
}

Output branch_and_bound_primal_dual_combocore_test(Instance& instance)
{
    Info info = Info()
        //.set_verbosity_level(1)
        //.set_log2stderr(true)
        ;
    BranchAndBoundPrimalDualOptionalParameters p;
    p.info = info;
    p.combo_core = true;
    return branch_and_bound_primal_dual(instance, p);
}

std::vector<Output (*)(Instance&)> f = {
        dynamic_programming_primal_dual_test,
        branch_and_bound_primal_dual_test,
        branch_and_bound_primal_dual_combocore_test,
};

TEST(branch_and_bound_primal_dual, TEST)  { test(TEST, f, SOPT); }
TEST(branch_and_bound_primal_dual, SMALL) { test(SMALL, f, SOPT); }

