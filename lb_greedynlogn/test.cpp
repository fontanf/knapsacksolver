#include "knapsack/lib/tester.hpp"
#include "knapsack/opt_minknap/minknap.hpp"
#include "knapsack/lb_greedynlogn/greedynlogn.hpp"

using namespace knapsack;

Output sopt_minknap_test(Instance& ins)
{
    return sopt_minknap(ins);
}

Output sol_forwardgreedynlogn_test(Instance& ins)
{
    Info info;
    ins.sort_partially(info);
    return sol_forwardgreedynlogn(ins, info);
}

Output sol_backwardgreedynlogn_test(Instance& ins)
{
    Info info;
    ins.sort_partially(info);
    return sol_backwardgreedynlogn(ins, info);
}

Output sol_greedynlogn_test(Instance& ins)
{
    Info info; ins.sort_partially(info);
    return sol_greedynlogn(ins, info);
}

std::vector<Output (*)(Instance&)> f = {
        sopt_minknap_test,
        sol_forwardgreedynlogn_test,
        sol_backwardgreedynlogn_test,
        sol_greedynlogn_test,
};

TEST(greedynlogn, TEST)  { test(TEST, f, LB); }
TEST(greedynlogn, SMALL) { test(SMALL, f, LB); }

