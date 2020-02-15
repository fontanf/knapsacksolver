#include "knapsacksolver/tester.hpp"
#include "knapsacksolver/algorithms/minknap.hpp"
#include "knapsacksolver/algorithms/balknap.hpp"

using namespace knapsacksolver;

Output minknap_test(Instance& ins)
{
    Info info = Info()
        .set_verbose(true)
        ;
    MinknapOptionalParameters p;
    p.info = info;
    return minknap(ins, p);
}

Output balknap_1_b_test(Instance& ins)
{
    Info info = Info()
        .set_verbose(true)
        ;
    BalknapOptionalParameters p;
    p.info = info;
    p.ub = 'b';
    p.partial_solution_size = 1; return balknap(ins, p);
}

Output balknap_2_b_test(Instance& ins)
{
    Info info = Info()
        .set_verbose(true)
        ;
    BalknapOptionalParameters p;
    p.info = info;
    p.ub = 'b';
    p.partial_solution_size = 2;
    return balknap(ins, p);
}

Output balknap_3_b_test(Instance& ins)
{
    Info info = Info()
        .set_verbose(true)
        ;
    BalknapOptionalParameters p;
    p.info = info;
    p.ub = 'b';
    p.partial_solution_size = 3;
    return balknap(ins, p);
}

Output balknap_1_t_test(Instance& ins)
{
    Info info = Info()
        .set_verbose(true)
        ;
    BalknapOptionalParameters p;
    p.info = info;
    p.ub = 't';
    p.partial_solution_size = 1;
    return balknap(ins, p);
}

Output balknap_2_t_test(Instance& ins)
{
    Info info = Info()
        .set_verbose(true)
        ;
    BalknapOptionalParameters p;
    p.info = info;
    p.ub = 't';
    p.partial_solution_size = 2;
    return balknap(ins, p);
}

Output balknap_3_t_test(Instance& ins)
{
    Info info = Info()
        .set_verbose(true)
        ;
    BalknapOptionalParameters p;
    p.info = info;
    p.ub = 't';
    p.partial_solution_size = 3;
    return balknap(ins, p);
}

std::vector<Output (*)(Instance&)> f = {
        minknap_test,
        balknap_1_b_test,
        balknap_2_b_test,
        balknap_3_b_test,
        balknap_1_t_test,
        balknap_2_t_test,
        balknap_3_t_test,
};

TEST(balknap, TEST)  { test(TEST, f, SOPT); }
TEST(balknap, SMALL) { test(SMALL, f, SOPT); }

