#include "../lib/tester.hpp"

using namespace knapsack;

TEST(DPProfits, SimpleInstances)
{
    boost::filesystem::path p = boost::filesystem::current_path();
    p /= boost::filesystem::path("opt_dpprofits");
    p /= boost::filesystem::path("main");

    test(p.string() + " -m array -r none", "opt");
    test(p.string() + " -m array -r all", "sopt");
}

