#include "knapsack/lib/tester.hpp"

TEST(Minknap, DataTests)
{
    auto p = boost::filesystem::current_path() / "opt_minknap" / "main";
    knapsack::test(p.string() + " -v -r none", "opt");
    knapsack::test(p.string() + " -v -r part", "sopt");
    knapsack::test(p.string() + " -v -r part -x 1", "sopt");
    knapsack::test(p.string() + " -v -r part -x 2", "sopt");
    knapsack::test(p.string() + " -v -r part -x 3", "sopt");
}

