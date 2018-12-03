#include "knapsack/lib/tester.hpp"
#include "knapsack/opt_bellman/bellman.hpp"

using namespace knapsack;

TEST(Bellman, DataTests)
{
    auto p = boost::filesystem::current_path() / "opt_bellman" / "main";
    knapsack::test(p.string() + " -m array -r none",  "opt");
    knapsack::test(p.string() + " -m array -r all",  "sopt");
    knapsack::test(p.string() + " -m array -r one",  "sopt");
    knapsack::test(p.string() + " -m array -r part -x 1", "sopt");
    knapsack::test(p.string() + " -m array -r part -x 2", "sopt");
    knapsack::test(p.string() + " -m array -r part -x 3", "sopt");
    knapsack::test(p.string() + " -m array -r rec",  "sopt");
    knapsack::test(p.string() + " -m list -r none",  "opt");
    //knapsack::test(p.string() + " -m list -r all",  "sopt");
    //knapsack::test(p.string() + " -m list -r one",  "sopt");
    //knapsack::test(p.string() + " -m list -r part -x 1", "sopt");
    //knapsack::test(p.string() + " -m list -r part -x 2", "sopt");
    //knapsack::test(p.string() + " -m list -r part -x 3", "sopt");
    knapsack::test(p.string() + " -m list -r rec",  "sopt");
}

Profit opt_bellman_array_test(Instance& ins)       { Info info; info.set_verbose(); return opt_bellman_array(ins, info); }
Profit opt_bellman_array_all_test(Instance& ins)   { Info info; info.set_verbose(); return sopt_bellman_array_all(ins, info).profit(); }
Profit opt_bellman_array_one_test(Instance& ins)   { Info info; info.set_verbose(); return sopt_bellman_array_one(ins, info).profit(); }
Profit opt_bellman_array_part1_test(Instance& ins) { Info info; info.set_verbose(); return sopt_bellman_array_part(ins, info, 1).profit(); }
Profit opt_bellman_array_part2_test(Instance& ins) { Info info; info.set_verbose(); return sopt_bellman_array_part(ins, info, 2).profit(); }
Profit opt_bellman_array_part3_test(Instance& ins) { Info info; info.set_verbose(); return sopt_bellman_array_part(ins, info, 3).profit(); }
Profit opt_bellman_array_rec_test(Instance& ins)   { Info info; info.set_verbose(); return sopt_bellman_array_rec(ins, info).profit(); }
Profit opt_bellman_list_test(Instance& ins)        { Info info; info.set_verbose(); return opt_bellman_list(ins, info); }
Profit opt_bellman_list_all_test(Instance& ins)    { Info info; info.set_verbose(); return sopt_bellman_list_all(ins, info).profit(); }
Profit opt_bellman_list_one_test(Instance& ins)    { Info info; info.set_verbose(); return sopt_bellman_list_one(ins, info).profit(); }
Profit opt_bellman_list_part1_test(Instance& ins)  { Info info; info.set_verbose(); return sopt_bellman_list_part(ins, info, 1).profit(); }
Profit opt_bellman_list_part2_test(Instance& ins)  { Info info; info.set_verbose(); return sopt_bellman_list_part(ins, info, 2).profit(); }
Profit opt_bellman_list_part3_test(Instance& ins)  { Info info; info.set_verbose(); return sopt_bellman_list_part(ins, info, 3).profit(); }
Profit opt_bellman_list_rec_test(Instance& ins)    { Info info; info.set_verbose(); return sopt_bellman_list_rec(ins, info).profit(); }

std::vector<Profit (*)(Instance&)> tested_functions()
{
    return {
        opt_bellman_array_test,
        opt_bellman_array_all_test,
        opt_bellman_array_one_test,
        opt_bellman_array_part1_test,
        opt_bellman_array_part2_test,
        opt_bellman_array_part3_test,
        opt_bellman_array_rec_test,
        opt_bellman_list_test,
        //opt_bellman_list_all_test,
        //opt_bellman_list_one_test,
        //opt_bellman_list_part1_test,
        //opt_bellman_list_part2_test,
        //opt_bellman_list_part3_test,
        opt_bellman_list_rec_test,
    };
}

TEST(Bellman, DataPisingerSmall)
{
    test_pisinger(
        {1, 2, 4, 8, 16, 32},
        {1, 2, 4, 8, 16, 32, 64},
        {"u", "wc", "sc", "isc", "asc", "ss"},
    {tested_functions()});
}

