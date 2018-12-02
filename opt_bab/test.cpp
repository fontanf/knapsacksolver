#include "knapsack/lib/tester.hpp"
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
        {1, 2, 3, 4, 5, 6, 7, 8},
        {1, 2, 4, 8, 16, 32, 64},
        {"u", "wc", "sc", "isc", "asc", "ss"},
        {tested_functions()});
}

