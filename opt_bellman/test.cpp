#include "knapsack/lib/tester.hpp"

using namespace knapsack;

TEST(BellmanRecursion, SimpleInstances)
{
    boost::filesystem::path p = boost::filesystem::current_path();
    p /= boost::filesystem::path("opt_bellman");
    p /= boost::filesystem::path("main");

    test(p.string() + " -m array -r none",  "opt");
    test(p.string() + " -m array -r all",  "sopt");
    test(p.string() + " -m array -r one",  "sopt");
    test(p.string() + " -m array -r part", "sopt");
    test(p.string() + " -m array -r rec",  "sopt");
    test(p.string() + " -m list -r none",  "opt");
    //test(p.string() + " -m list -r all",  "sopt");
    //test(p.string() + " -m list -r one",  "sopt");
    //test(p.string() + " -m list -r part", "sopt");
    test(p.string() + " -m list -r rec",  "sopt");
}

