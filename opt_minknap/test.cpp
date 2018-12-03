#include "knapsack/lib/tester.hpp"
#include "knapsack/opt_minknap/minknap.hpp"
#include "knapsack/opt_bellman/bellman.hpp"

using namespace knapsack;

TEST(Minknap, DataTests)
{
    auto p = boost::filesystem::current_path() / "opt_minknap" / "main";
    knapsack::test(p.string() + " -v -x 1", "sopt");
    knapsack::test(p.string() + " -v -x 2", "sopt");
    knapsack::test(p.string() + " -v -x 3", "sopt");
}

Profit opt_bellman_array_test(Instance& ins) { Info info; info.set_verbose(); return opt_bellman_array(ins, info); }
Profit opt_minknap_1_test(Instance& ins)     { Info info; info.set_verbose(); return sopt_minknap(ins, info, MinknapParams(), 1).profit(); }
Profit opt_minknap_2_test(Instance& ins)     { Info info; info.set_verbose(); return sopt_minknap(ins, info, MinknapParams(), 2).profit(); }
Profit opt_minknap_3_test(Instance& ins)     { Info info; info.set_verbose(); return sopt_minknap(ins, info, MinknapParams(), 3).profit(); }

std::vector<Profit (*)(Instance&)> tested_functions()
{
    return {
        opt_bellman_array_test,
        opt_minknap_1_test,
        opt_minknap_2_test,
        opt_minknap_3_test,
    };
}

TEST(Minknap, DataPisingerSmall)
{
    test_pisinger(
        {1, 2, 4, 8, 16, 32},
        {1, 2, 4, 8, 16, 32, 64},
        {"u", "wc", "sc", "isc", "asc", "ss"},
    {tested_functions()});
}

//TEST(Minknap, DataPisingerMedium)
//{
    //test_pisinger(
        //{40, 50, 60, 70, 80, 90, 100},
        //{10, 100},
        //{"u", "wc", "sc", "isc", "asc", "ss"},
    //{tested_functions()});
//}

//TEST(Minknap, DataPisingerBig)
//{
    //test_pisinger(
        //{500},
        //{100},
        //{"sc", "asc"},
        //{tested_functions()}, 1);
//}

