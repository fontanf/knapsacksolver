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

Profit opt_minknap_list_test(Instance& ins) { std::cout << "MKL " << std::endl; return opt_minknap_list(ins); }
Profit opt_bab_test(Instance& ins)
{
    Info info;
    info.verbose(true);
    std::cout << "BAB" << std::endl;
    return sopt_bab(ins, &info).profit();
}

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
