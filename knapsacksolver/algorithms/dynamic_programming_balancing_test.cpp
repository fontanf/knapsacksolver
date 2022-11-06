#include "knapsacksolver/tester.hpp"
#include "knapsacksolver/algorithms/dynamic_programming_primal_dual.hpp"
#include "knapsacksolver/algorithms/dynamic_programming_balancing.hpp"

using namespace knapsacksolver;

Output dynamic_programming_primal_dual_test(Instance& instance)
{
    Info info = Info()
        .set_verbosity_level(1)
        ;
    DynamicProgrammingPrimalDualOptionalParameters p;
    p.info = info;
    return dynamic_programming_primal_dual(instance, p);
}

Output dynamic_programming_balancing_1_b_test(Instance& instance)
{
    Info info = Info()
        .set_verbosity_level(1)
        ;
    DynamicProgrammingBalancingOptionalParameters p;
    p.info = info;
    p.ub = 'b';
    p.partial_solution_size = 1; return dynamic_programming_balancing(instance, p);
}

Output dynamic_programming_balancing_2_b_test(Instance& instance)
{
    Info info = Info()
        .set_verbosity_level(1)
        ;
    DynamicProgrammingBalancingOptionalParameters p;
    p.info = info;
    p.ub = 'b';
    p.partial_solution_size = 2;
    return dynamic_programming_balancing(instance, p);
}

Output dynamic_programming_balancing_3_b_test(Instance& instance)
{
    Info info = Info()
        .set_verbosity_level(1)
        ;
    DynamicProgrammingBalancingOptionalParameters p;
    p.info = info;
    p.ub = 'b';
    p.partial_solution_size = 3;
    return dynamic_programming_balancing(instance, p);
}

Output dynamic_programming_balancing_1_t_test(Instance& instance)
{
    Info info = Info()
        .set_verbosity_level(1)
        ;
    DynamicProgrammingBalancingOptionalParameters p;
    p.info = info;
    p.ub = 't';
    p.partial_solution_size = 1;
    return dynamic_programming_balancing(instance, p);
}

Output dynamic_programming_balancing_2_t_test(Instance& instance)
{
    Info info = Info()
        .set_verbosity_level(1)
        ;
    DynamicProgrammingBalancingOptionalParameters p;
    p.info = info;
    p.ub = 't';
    p.partial_solution_size = 2;
    return dynamic_programming_balancing(instance, p);
}

Output dynamic_programming_balancing_3_t_test(Instance& instance)
{
    Info info = Info()
        .set_verbosity_level(1)
        ;
    DynamicProgrammingBalancingOptionalParameters p;
    p.info = info;
    p.ub = 't';
    p.partial_solution_size = 3;
    return dynamic_programming_balancing(instance, p);
}

std::vector<Output (*)(Instance&)> f = {
        dynamic_programming_primal_dual_test,
        dynamic_programming_balancing_1_b_test,
        dynamic_programming_balancing_2_b_test,
        dynamic_programming_balancing_3_b_test,
        dynamic_programming_balancing_1_t_test,
        dynamic_programming_balancing_2_t_test,
        dynamic_programming_balancing_3_t_test,
};

TEST(dynamic_programming_balancing, TEST)  { test(TEST, f, SOPT); }
TEST(dynamic_programming_balancing, SMALL) { test(SMALL, f, SOPT); }

