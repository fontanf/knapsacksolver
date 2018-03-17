#include "../lib/tester.hpp"

TEST(Balknap, SimpleInstances)
{
    boost::filesystem::path p = boost::filesystem::current_path();
    p /= boost::filesystem::path("opt_balknap");
    p /= boost::filesystem::path("main");

    test(p.string() + " -v -m array -r none", "opt");
    test(p.string() + " -v -m array -r all", "sopt");
    //test(p.string() + " -v -m array -r part", "sopt");
    //test(p.string() + " -v -m list -r none -u b", "opt");
    //test(p.string() + " -v -m list -r all -u b", "sopt");
    //test(p.string() + " -v -m list -r part -u b", "sopt");
    //test(p.string() + " -v -m list -r none -u t", "opt");
    //test(p.string() + " -v -m list -r all -u t", "sopt");
    //test(p.string() + " -v -m list -r part -u t", "sopt");
}

