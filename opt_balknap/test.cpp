#include "../lib/tester.hpp"

TEST(Balknap, SimpleInstances)
{
    boost::filesystem::path p = boost::filesystem::current_path();
    p /= boost::filesystem::path("opt_balknap");
    p /= boost::filesystem::path("main");

    test(p.string() + " -a  opt",                  "opt");
    test(p.string() + " -a sopt",                 "sopt");
    test(p.string() + " -a  opt_list -u dembo",    "opt");
    test(p.string() + " -a sopt_list -u dembo",   "sopt");
    test(p.string() + " -a  opt_list -u trivial",  "opt");
    test(p.string() + " -a sopt_list -u trivial", "sopt");
    test(p.string() + " -a  opt_list -u dantzig",  "opt");
    test(p.string() + " -a sopt_list -u dantzig", "sopt");
}

