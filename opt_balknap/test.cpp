#include "knapsack/lib/tester.hpp"
#include "knapsack/opt_minknap/minknap.hpp"
#include "knapsack/opt_balknap/balknap.hpp"

using namespace knapsack;

Output sopt_minknap_test(Instance& ins)
{
    Info info = Info()
        .set_verbose(true)
        ;
    MinknapOptionalParameters p;
    p.info = info;
    return sopt_minknap(ins, p);
}

Output sopt_balknap_1_b_test(Instance& ins)
{
    Info info = Info()
        .set_verbose(true)
        ;
    BalknapOptionalParameters p;
    p.info = info;
    p.ub = 'b';
    p.partial_solution_size = 1; return sopt_balknap(ins, p);
}

Output sopt_balknap_2_b_test(Instance& ins)
{
    Info info = Info()
        .set_verbose(true)
        ;
    BalknapOptionalParameters p;
    p.info = info;
    p.ub = 'b';
    p.partial_solution_size = 2;
    return sopt_balknap(ins, p);
}

Output sopt_balknap_3_b_test(Instance& ins)
{
    Info info = Info()
        .set_verbose(true)
        ;
    BalknapOptionalParameters p;
    p.info = info;
    p.ub = 'b';
    p.partial_solution_size = 3;
    return sopt_balknap(ins, p);
}

Output sopt_balknap_1_t_test(Instance& ins)
{
    Info info = Info()
        .set_verbose(true)
        ;
    BalknapOptionalParameters p;
    p.info = info;
    p.ub = 't';
    p.partial_solution_size = 1;
    return sopt_balknap(ins, p);
}

Output sopt_balknap_2_t_test(Instance& ins)
{
    Info info = Info()
        .set_verbose(true)
        ;
    BalknapOptionalParameters p;
    p.info = info;
    p.ub = 't';
    p.partial_solution_size = 2;
    return sopt_balknap(ins, p);
}

Output sopt_balknap_3_t_test(Instance& ins)
{
    Info info = Info()
        .set_verbose(true)
        ;
    BalknapOptionalParameters p;
    p.info = info;
    p.ub = 't';
    p.partial_solution_size = 3;
    return sopt_balknap(ins, p);
}

std::vector<Output (*)(Instance&)> f = {
        sopt_minknap_test,
        sopt_balknap_1_b_test,
        sopt_balknap_2_b_test,
        sopt_balknap_3_b_test,
        sopt_balknap_1_t_test,
        sopt_balknap_2_t_test,
        sopt_balknap_3_t_test,
};

TEST(balknap, TEST)  { test(TEST, f, SOPT); }
TEST(balknap, SMALL) { test(SMALL, f, SOPT); }

