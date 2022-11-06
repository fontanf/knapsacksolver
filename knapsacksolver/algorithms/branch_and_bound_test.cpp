#include "knapsacksolver/tester.hpp"
#include "knapsacksolver/algorithms/dynamic_programming_primal_dual.hpp"
#include "knapsacksolver/algorithms/branch_and_bound.hpp"

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

Output branch_and_bound_test(Instance& instance)
{
    Info info = Info()
        //.set_verbosity_level(1)
        ;
    return branch_and_bound(instance, false, info);
}

Output branch_and_bound_sort_test(Instance& instance)
{
    Info info = Info()
        //.set_verbosity_level(1)
        ;
    return branch_and_bound(instance, true, info);
}

std::vector<Output (*)(Instance&)> f = {
        dynamic_programming_primal_dual_test,
        branch_and_bound_test,
        branch_and_bound_sort_test,
};

TEST(bab, TEST)  { test(TEST, f); }
TEST(bab, SMALL) { test(SMALL, f); }

