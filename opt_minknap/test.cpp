#include "knapsack/lib/tester.hpp"
#include "knapsack/opt_minknap/minknap.hpp"
#include "knapsack/opt_bellman/bellman.hpp"

using namespace knapsack;

TEST(Minknap, DataTests)
{
    auto p = boost::filesystem::current_path() / "opt_minknap" / "main";
    knapsack::test(p.string() + " -v -r none", "opt");
    knapsack::test(p.string() + " -v -r part", "sopt");
    knapsack::test(p.string() + " -v -r part -x 1", "sopt");
    knapsack::test(p.string() + " -v -r part -x 2", "sopt");
    knapsack::test(p.string() + " -v -r part -x 3", "sopt");
}

Profit opt_bellman_array_test(Instance& ins)     { Info info; return opt_bellman_array(ins, info); }
Profit opt_minknap_list_test(Instance& ins)      { Info info; return opt_minknap_list(ins, info); }
Profit opt_minknap_list_part1_test(Instance& ins) { Info info; info.set_verbose(); return sopt_minknap_list_part(ins, info, MinknapParams(), 1).profit(); }
Profit opt_minknap_list_part2_test(Instance& ins) { Info info; info.set_verbose(); return sopt_minknap_list_part(ins, info, MinknapParams(), 2).profit(); }
Profit opt_minknap_list_part3_test(Instance& ins) { Info info; info.set_verbose(); return sopt_minknap_list_part(ins, info, MinknapParams(), 3).profit(); }

std::vector<Profit (*)(Instance&)> tested_functions()
{
    return {
        opt_bellman_array_test,
        opt_minknap_list_test,
        opt_minknap_list_part1_test,
        opt_minknap_list_part2_test,
        opt_minknap_list_part3_test,
    };
}

TEST(Minknap, DataPisingerSmall)
{
    test_pisinger(
        {1, 2, 3, 4, 5, 6, 7, 8, 10, 12, 14, 16, 20, 24, 28, 32},
        {1, 2, 4, 8, 16, 32, 64, 128},
        {"u", "wc", "sc", "isc", "asc", "ss"},
    {tested_functions()});
}

TEST(Minknap, DataPisingerMedium)
{
    test_pisinger(
        {40, 50, 60, 70, 80, 90, 100},
        {10, 100},
        {"u", "wc", "sc", "isc", "asc", "ss"},
    {tested_functions()});
}

TEST(Minknap, DataPisingerBig)
{
    test_pisinger(
        {500},
        {100},
        {"sc", "asc"},
        {tested_functions()}, 1);
}

