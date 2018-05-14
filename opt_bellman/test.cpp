#include "knapsack/lib/tester.hpp"

TEST(Bellman, DataTests)
{
    auto p = boost::filesystem::current_path() / "opt_bellman" / "main";
    knapsack::test(p.string() + " -m array -r none",  "opt");
    knapsack::test(p.string() + " -m array -r all",  "sopt");
    knapsack::test(p.string() + " -m array -r one",  "sopt");
    knapsack::test(p.string() + " -m array -r part", "sopt");
    knapsack::test(p.string() + " -m array -r rec",  "sopt");
    knapsack::test(p.string() + " -m list -r none",  "opt");
    //knapsack::test(p.string() + " -m list -r all",  "sopt");
    //knapsack::test(p.string() + " -m list -r one",  "sopt");
    //knapsack::test(p.string() + " -m list -r part", "sopt");
    knapsack::test(p.string() + " -m list -r rec",  "sopt");
}

