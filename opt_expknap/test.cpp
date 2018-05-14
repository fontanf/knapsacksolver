#include "knapsack/lib/tester.hpp"

TEST(Expknap, DataTests)
{
    auto p = boost::filesystem::current_path() / "opt_expknap" / "main";
    knapsack::test(p.string() + " -v", "sopt");
}

