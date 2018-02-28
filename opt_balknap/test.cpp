#include "../lib/tester.hpp"

TEST(Balknap, SimpleInstances)
{
    boost::filesystem::path p = boost::filesystem::current_path();
    p /= boost::filesystem::path("opt_balknap");
    p /= boost::filesystem::path("main");

    test(p.string() + " -v -a  opt",                  "opt");
    test(p.string() + " -v -a sopt",                 "sopt");
    test(p.string() + " -v -a  opt_list -u dembo",    "opt");
    test(p.string() + " -v -a sopt_list -u dembo",   "sopt");
    test(p.string() + " -v -a  opt_list -u trivial",  "opt");
    test(p.string() + " -v -a sopt_list -u trivial", "sopt");
    test(p.string() + " -v -a  opt_list -u dantzig",  "opt");
    test(p.string() + " -v -a sopt_list -u dantzig", "sopt");
    test(p.string() + " -v -a  opt_list -u dembo -r 1",    "opt");
    test(p.string() + " -v -a sopt_list -u dembo -r 1",   "sopt");
    test(p.string() + " -v -a  opt_list -u trivial -r 1",  "opt");
    test(p.string() + " -v -a sopt_list -u trivial -r 1", "sopt");
    test(p.string() + " -v -a  opt_list -u dantzig -r 1",  "opt");
    test(p.string() + " -v -a sopt_list -u dantzig -r 1", "sopt");
    test(p.string() + " -v -a  opt_list -u dembo -r 2",    "opt");
    test(p.string() + " -v -a sopt_list -u dembo -r 2",   "sopt");
    test(p.string() + " -v -a  opt_list -u trivial -r 2",  "opt");
    test(p.string() + " -v -a sopt_list -u trivial -r 2", "sopt");
    test(p.string() + " -v -a  opt_list -u dantzig -r 2",  "opt");
    test(p.string() + " -v -a sopt_list -u dantzig -r 2", "sopt");
}

