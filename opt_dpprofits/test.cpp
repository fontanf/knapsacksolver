#include "knapsack/lib/tester.hpp"

TEST(DPProfits, DataTests)
{
    auto p = boost::filesystem::current_path() / "opt_dpprofits" / "main";
    knapsack::test(p.string() + " -m array -r none", "opt");
    knapsack::test(p.string() + " -m array -r all", "sopt");
}

