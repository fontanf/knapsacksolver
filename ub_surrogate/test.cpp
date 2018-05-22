#include "knapsack/lib/tester.hpp"
#include "knapsack/lib/generator.hpp"
#include "knapsack/ub_surrogate/surrogate.hpp"
#include "knapsack/opt_minknap/minknap.hpp"
#include "knapsack/opt_bellman/bellman_array.hpp"
#include "knapsack/lb_greedy/greedy.hpp"

using namespace knapsack;

Profit opt_bellman_array_test(Instance &ins)
{
    return opt_bellman_array(ins);
}

Profit opt_bellman_array_all_test(Instance &ins)
{
    return sopt_bellman_array_all(ins).profit();
}

Profit ub_surrogate_test(Instance& ins)
{
    ins.sort_partially();
    Solution sol = sol_greedy(ins);
    return ub_surrogate(ins, sol.profit()).ub;
}

std::vector<Profit (*)(Instance&)> tested_functions()
{
    return {
        opt_bellman_array_test,
        ub_surrogate_test,
    };
}

TEST(Balknap, DataPisingerSmall)
{
    test_pisinger(
        {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20},
        {10},
        {"u", "wc", "sc", "isc", "asc", "ss", "sw"},
        {tested_functions()}, 1);
}

TEST(Balknap, DataPisingerMedium)
{
    test_pisinger(
        {50, 100},
        {10, 100},
        {"u", "wc", "isc", "asc"},
        {tested_functions()}, 1);
}

TEST(Balknap, DataPisingerBig)
{
    test_pisinger(
        {1000},
        {100},
        {"sc", "isc", "asc"},
        {tested_functions()}, 1);
}

