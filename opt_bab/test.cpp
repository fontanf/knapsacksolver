#include "knapsack/lib/tester.hpp"

using namespace knapsack;

TEST(BranchAndBound, SimpleInstances)
{
    boost::filesystem::path p = boost::filesystem::current_path();
    p /= boost::filesystem::path("opt_bab");
    p /= boost::filesystem::path("main");

    test(p.string(), "sopt");
}

