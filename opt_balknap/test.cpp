#include "knapsack/lib/tester.hpp"

TEST(Balknap, DataTests)
{
    auto p = boost::filesystem::current_path() / "opt_balknap" / "main";
    knapsack::test(p.string() + " -v -m array -r none", "opt");
    knapsack::test(p.string() + " -v -m array -r all", "sopt");
    //knapsack::test(p.string() + " -v -m array -r part", "sopt");
    knapsack::test(p.string() + " -v -m list -r none -u b", "opt");
    knapsack::test(p.string() + " -v -m list -r all -u b", "sopt");
    knapsack::test(p.string() + " -v -m list -r part -u b", "sopt");
    knapsack::test(p.string() + " -v -m list -r none -u t", "opt");
    knapsack::test(p.string() + " -v -m list -r all -u t", "sopt");
    knapsack::test(p.string() + " -v -m list -r part -u t", "sopt");
    knapsack::test(p.string() + " -v -m list -r all -u t -x 2", "sopt");
    knapsack::test(p.string() + " -v -m list -r part -u t -x 2", "sopt");
    knapsack::test(p.string() + " -v -m list -r part -u b -s 0", "sopt");
    knapsack::test(p.string() + " -v -m list -r part -u t -s 0", "sopt");
}

