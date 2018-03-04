#include "../lib/tester.hpp"

TEST(BellmanRecursion, SimpleInstances)
{
    boost::filesystem::path p = boost::filesystem::current_path();
    p /= boost::filesystem::path("opt_bellman");
    p /= boost::filesystem::path("main");

    test(p.string() + " -v -a opt",            "opt");
    test(p.string() + " -v -a sopt_1",        "sopt");
    test(p.string() + " -v -a sopt_1it",      "sopt");
    test(p.string() + " -v -a sopt_1map",     "sopt");
    test(p.string() + " -v -a sopt_1rec",     "sopt");
    test(p.string() + " -v -a sopt_1stack",   "sopt");
    test(p.string() + " -v -a sopt_2",        "sopt");
    test(p.string() + " -v -a sopt_rec",      "sopt");
    test(p.string() + " -v -a opt_list",       "opt");
    test(p.string() + " -v -a sopt_list_rec", "sopt");
    test(p.string() + " -v -a opt_list      -l greedy -u dantzig",  "opt");
    test(p.string() + " -v -a sopt_list_rec -l greedy -u dantzig", "sopt");
}

