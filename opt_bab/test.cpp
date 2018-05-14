#include "knapsack/lib/tester.hpp"

TEST(BranchAndBound, DataTests)
{
    auto p = boost::filesystem::current_path() / "opt_bab" / "main";
    knapsack::test(p.string(), "sopt");
}

