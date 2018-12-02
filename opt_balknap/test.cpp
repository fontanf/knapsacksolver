#include "knapsack/lib/tester.hpp"
#include "knapsack/lib/generator.hpp"
#include "knapsack/opt_balknap/balknap.hpp"
#include "knapsack/opt_minknap/minknap.hpp"

using namespace knapsack;

TEST(Balknap, DataTests)
{
    auto p = boost::filesystem::current_path() / "opt_balknap" / "main";
    knapsack::test(p.string() + " -v -m array -r none", "opt");
    knapsack::test(p.string() + " -v -m array -r all", "sopt");
    //knapsack::test(p.string() + " -v -m array -r part", "sopt");
    knapsack::test(p.string() + " -v -m list -r none -u b", "opt");
    knapsack::test(p.string() + " -v -m list -r all -u b", "sopt");
    knapsack::test(p.string() + " -v -m list -r part -u b", "sopt");
    knapsack::test(p.string() + " -v -m list -r none -u t", "opt");
    knapsack::test(p.string() + " -v -m list -r all -u t", "sopt");
    knapsack::test(p.string() + " -v -m list -r part -u t", "sopt");
    knapsack::test(p.string() + " -v -m list -r all -u t -x 2", "sopt");
    knapsack::test(p.string() + " -v -m list -r part -u t -x 2", "sopt");
    knapsack::test(p.string() + " -v -m list -r part -u b -s 0", "sopt");
    knapsack::test(p.string() + " -v -m list -r part -u t -s 0", "sopt");
}

Profit opt_minknap_list_test(Instance& ins)        { std::cout << "MKL"   << std::endl; Info info; return opt_minknap_list(ins, info); }
Profit opt_balknap_array_test(Instance& ins)       { std::cout << "BKA"   << std::endl; Info info; return opt_balknap_array(ins, info); }
Profit opt_balknap_array_all_test(Instance& ins)   { std::cout << "BKAA"  << std::endl; Info info; return sopt_balknap_array_all(ins, info).profit(); }
//Profit opt_balknap_array_part_test(Instance& ins)  { std::cout << "BKAP"  << std::endl; Info info; return sopt_balknap_array_part(ins, info, BalknapParams()).profit(); }
Profit opt_balknap_list_test(Instance& ins)        { std::cout << "BKL"   << std::endl; Info info; return opt_balknap_list(ins, info); }
Profit opt_balknap_list_t_test(Instance& ins)      { std::cout << "BKLT"  << std::endl; Info info; BalknapParams p; p.upper_bound = "t"; return opt_balknap_list(ins, info); }
Profit opt_balknap_list_all_test(Instance& ins)    { std::cout << "BKLA"  << std::endl; Info info; return sopt_balknap_list_all(ins, info).profit(); }
Profit opt_balknap_list_all_t_test(Instance& ins)  { std::cout << "BKLAT" << std::endl; Info info; BalknapParams p; p.upper_bound = "t"; return sopt_balknap_list_all(ins, info).profit(); }
Profit opt_balknap_list_part1_test(Instance& ins)   { std::cout << "BKLP"  << std::endl; Info info; return sopt_balknap_list_part(ins, info, BalknapParams(), 1).profit(); }
Profit opt_balknap_list_part2_test(Instance& ins)   { std::cout << "BKLP"  << std::endl; Info info; return sopt_balknap_list_part(ins, info, BalknapParams(), 2).profit(); }
Profit opt_balknap_list_part3_test(Instance& ins)   { std::cout << "BKLP"  << std::endl; Info info; return sopt_balknap_list_part(ins, info, BalknapParams(), 3).profit(); }
Profit opt_balknap_list_part1_t_test(Instance& ins) { std::cout << "BKLPT" << std::endl; Info info; BalknapParams p; p.upper_bound = "t"; return sopt_balknap_list_part(ins, info, BalknapParams(), 1).profit(); }
Profit opt_balknap_list_part2_t_test(Instance& ins) { std::cout << "BKLPT" << std::endl; Info info; BalknapParams p; p.upper_bound = "t"; return sopt_balknap_list_part(ins, info, BalknapParams(), 2).profit(); }
Profit opt_balknap_list_part3_t_test(Instance& ins) { std::cout << "BKLPT" << std::endl; Info info; BalknapParams p; p.upper_bound = "t"; return sopt_balknap_list_part(ins, info, BalknapParams(), 3).profit(); }

std::vector<Profit (*)(Instance&)> tested_functions()
{
    return {
        opt_minknap_list_test,
        opt_balknap_array_test,
        opt_balknap_list_test,
        opt_balknap_list_t_test,
        opt_balknap_array_all_test,
        //opt_balknap_array_part_test,
        opt_balknap_list_all_test,
        opt_balknap_list_all_t_test,
        opt_balknap_list_part1_test,
        opt_balknap_list_part2_test,
        opt_balknap_list_part3_test,
        opt_balknap_list_part1_t_test,
        opt_balknap_list_part2_t_test,
        opt_balknap_list_part3_t_test,
    };
}

TEST(Balknap, DataPisingerSmall)
{
    test_pisinger(
        {1, 2, 3, 4, 5, 6, 7, 8, 10, 12, 14, 16, 20, 24, 28, 32},
        {1, 2, 4, 8, 16, 32, 64, 128},
        {"u", "wc", "sc", "isc", "asc", "ss"},
    {tested_functions()});
}

