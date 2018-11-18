#include "knapsack/lib/tester.hpp"
#include "knapsack/lib/generator.hpp"
#include "knapsack/opt_minknap/minknap.hpp"
#include "knapsack/opt_babstar/bab.hpp"

using namespace knapsack;

TEST(BABSTAR, DataTests)
{
    auto p = boost::filesystem::current_path() / "opt_babstar" / "main";
    knapsack::test(p.string(), "sopt");
}

Profit opt_minknap_list_test(Instance& ins) { Info info; return opt_minknap_list(ins, info); }
Profit opt_babstar_test(Instance& ins)      { Info info; return sopt_babstar(ins, info).profit(); }
Profit opt_babstar_dp_test(Instance& ins)   { Info info; return sopt_babstar_dp(ins, info).profit(); }

std::vector<Profit (*)(Instance&)> tested_functions()
{
    return {
        opt_minknap_list_test,
        opt_babstar_test,
        opt_babstar_dp_test,
    };
}

TEST(BABSTAR, DataPisingerSmall)
{
    test_pisinger(
        //{1, 2, 5, 10, 15, 20, 30, 40},
        {1, 2, 5, 10, 15},
        {10, 100},
        {"u", "wc", "sc", "isc", "asc", "ss", "sw"},
        {tested_functions()});
}

//TEST(Minknap, DataPisingerMedium)
//{
    //test_pisinger(
        //{50, 100},
        //{100},
        //{"wc", "isc", "asc"},
        //{tested_functions()}, 1);
//}
