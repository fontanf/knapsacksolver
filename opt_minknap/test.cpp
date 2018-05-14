#include "knapsack/lib/tester.hpp"

using namespace knapsack;

TEST(Balknap, SimpleInstances)
{
    boost::filesystem::path p = boost::filesystem::current_path();
    p /= boost::filesystem::path("opt_minknap");
    p /= boost::filesystem::path("main");

    test(p.string() + " -v -r none", "opt");
    test(p.string() + " -v -r part", "sopt");
    test(p.string() + " -v -r part -x 1", "sopt");
    test(p.string() + " -v -r part -x 2", "sopt");
    test(p.string() + " -v -r part -x 3", "sopt");
}

