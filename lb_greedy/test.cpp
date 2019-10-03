#include "knapsack/lib/tester.hpp"
#include "knapsack/opt_minknap/minknap.hpp"
#include "knapsack/lb_greedy/greedy.hpp"

using namespace knapsack;

Output sopt_minknap_test(Instance& ins)
{
    return sopt_minknap(ins);
}

Output sol_greedy_test(Instance& ins)
{
    Info info;
    ins.sort_partially(info);
    return sol_greedy(ins, info);
}

std::vector<Output (*)(Instance&)> f = {
        sopt_minknap_test,
        sol_greedy_test,
};

TEST(greedy, TEST)  { test(TEST, f, LB); }
TEST(greedy, SMALL) { test(SMALL, f, LB); }

