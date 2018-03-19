#include "../lib/tester.hpp"

TEST(Expknap, SimpleInstances)
{
    boost::filesystem::path p = boost::filesystem::current_path();
    p /= boost::filesystem::path("opt_expknap");
    p /= boost::filesystem::path("main");
    test(p.string() + " -v", "sopt");
}

