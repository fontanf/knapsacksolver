#include "knapsack/lib/tester.hpp"
#include "knapsack/lib/generator.hpp"
#include "knapsack/opt_balknap/balknap.hpp"
#include "knapsack/opt_minknap/minknap.hpp"

using namespace knapsack;

TEST(Balknap, DataTests)
{
    auto p = boost::filesystem::current_path() / "opt_balknap" / "main";
    knapsack::test(p.string() + " -v -u b -x 1", "sopt");
    knapsack::test(p.string() + " -v -u b -x 2", "sopt");
    knapsack::test(p.string() + " -v -u b -x 3", "sopt");
    knapsack::test(p.string() + " -v -u t -x 1", "sopt");
    knapsack::test(p.string() + " -v -u t -x 2", "sopt");
    knapsack::test(p.string() + " -v -u t -x 3", "sopt");
}

Profit opt_minknap_test(Instance& ins)     { Info info; info.set_verbose(); return sopt_minknap(ins, info).profit(); }
Profit opt_balknap_1_b_test(Instance& ins) { Info info; info.set_verbose(); BalknapParams p; p.ub_type = 'b'; return sopt_balknap(ins, info, p, 1).profit(); }
Profit opt_balknap_2_b_test(Instance& ins) { Info info; info.set_verbose(); BalknapParams p; p.ub_type = 'b'; return sopt_balknap(ins, info, p, 2).profit(); }
Profit opt_balknap_3_b_test(Instance& ins) { Info info; info.set_verbose(); BalknapParams p; p.ub_type = 'b'; return sopt_balknap(ins, info, p, 3).profit(); }
Profit opt_balknap_1_t_test(Instance& ins) { Info info; info.set_verbose(); BalknapParams p; p.ub_type = 't'; return sopt_balknap(ins, info, p, 1).profit(); }
Profit opt_balknap_2_t_test(Instance& ins) { Info info; info.set_verbose(); BalknapParams p; p.ub_type = 't'; return sopt_balknap(ins, info, p, 2).profit(); }
Profit opt_balknap_3_t_test(Instance& ins) { Info info; info.set_verbose(); BalknapParams p; p.ub_type = 't'; return sopt_balknap(ins, info, p, 3).profit(); }

std::vector<Profit (*)(Instance&)> tested_functions()
{
    return {
        opt_minknap_test,
        opt_balknap_1_b_test,
        opt_balknap_2_b_test,
        opt_balknap_3_b_test,
        opt_balknap_1_t_test,
        opt_balknap_2_t_test,
        opt_balknap_3_t_test,
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

