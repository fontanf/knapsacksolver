#include "knapsack/lib/tester.hpp"
#include "knapsack/lib/generator.hpp"
#include "knapsack/ub_surrogate/surrogate.hpp"
#include "knapsack/opt_minknap/minknap.hpp"
#include "knapsack/opt_bellman/bellman.hpp"
#include "knapsack/lb_greedy/greedy.hpp"

using namespace knapsack;

Profit opt_bellman_array_test(Instance &ins) { Info info; return opt_bellman_array(ins, info); }
Profit opt_bellman_array_all_test(Instance &ins) { Info info; return sopt_bellman_array_all(ins, info).profit(); }

Profit ub_surrogate_test(Instance& ins)
{
    ins.sort_partially();
    Info info_tmp;
    Solution sol = sol_greedy(ins, info_tmp);
    Info info;
    return ub_surrogate(ins, sol.profit(), info).ub;
}

std::vector<Profit (*)(Instance&)> tested_functions()
{
    return {
        opt_bellman_array_test,
        ub_surrogate_test,
    };
}

TEST(SR, DataPisingerSmall)
{
    test_pisinger(
        {1, 2, 4, 8, 16, 32},
        {1, 2, 4, 8, 16, 32, 64},
        {"u", "wc", "sc", "isc", "asc", "ss"},
        {tested_functions()}, 1);
}

