#include "knapsack/lib/tester.hpp"
#include "knapsack/lib/generator.hpp"
#include "knapsack/opt_minknap/minknap.hpp"
#include "knapsack/opt_bab/bab.hpp"

using namespace knapsack;

TEST(BranchAndBound, DataTests)
{
    auto p = boost::filesystem::current_path() / "opt_bab" / "main";
    knapsack::test(p.string(), "sopt");
}

Profit opt_minknap_list_test(Instance& ins) { Info info; return opt_minknap_list(ins, info); }
Profit opt_bab_test(Instance& ins)          { Info info; return sopt_bab(ins, info).profit(); }

std::vector<Profit (*)(Instance&)> tested_functions()
{
    return {
        opt_minknap_list_test,
        opt_bab_test,
    };
}

TEST(Minknap, DataPisingerSmall)
{
    test_pisinger(
        {1, 2, 5, 10, 15},
        {10, 100},
        {"u", "wc", "sc", "isc", "asc", "ss", "sw"},
        {tested_functions()});
}

