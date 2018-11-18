#include "knapsack/lib/tester.hpp"
#include "knapsack/lib/generator.hpp"
#include "knapsack/opt_minknap/minknap.hpp"
#include "knapsack/opt_bellman/bellman_array.hpp"

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
Profit opt_minknap_list_part_test(Instance& ins) { Info info; return sopt_minknap_list_part(ins, info).profit(); }

std::vector<Profit (*)(Instance&)> tested_functions()
{
    return {
        opt_bellman_array_test,
        opt_minknap_list_test,
        opt_minknap_list_part_test,
    };
}

TEST(Minknap, DataPisingerSmall)
{
    test_pisinger(
        {1, 2, 3, 4, 5, 6, 7, 8, 9, 10},
        {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12},
        {"u", "wc", "sc", "isc", "asc", "ss", "sw"},
    {tested_functions()});
}

TEST(Minknap, DataPisingerMedium)
{
    test_pisinger(
        {15, 20, 30, 40, 50},
        {10, 50, 100},
        {"u", "wc", "sc", "isc", "asc", "ss", "sw"},
    {tested_functions()});
}

TEST(Minknap, DataPisingerMedium2)
{
    test_pisinger(
        {50, 100},
        {100},
        {"wc", "isc", "asc"},
        {tested_functions()}, 1);
}

TEST(Minknap, DataPisingerBig)
{
    test_pisinger(
        {500},
        {100},
        {"sc", "asc"},
        {tested_functions()}, 1);
}

