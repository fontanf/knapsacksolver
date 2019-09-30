#include "knapsack/lib/tester.hpp"
#include "knapsack/opt_minknap/minknap.hpp"
#include "knapsack/opt_balknap/balknap.hpp"

using namespace knapsack;

/*
Profit opt_minknap(Instance& ins)          { Info info; return sopt_minknap(ins, MinknapParams(), info).profit(); }
Profit opt_balknap_1_b_test(Instance& ins) { BalknapParams p; p.ub = 'b'; p.k = 1; return sopt_balknap(ins, p, Info()).profit(); }
Profit opt_balknap_2_b_test(Instance& ins) { BalknapParams p; p.ub = 'b'; p.k = 2; return sopt_balknap(ins, p, Info()).profit(); }
Profit opt_balknap_3_b_test(Instance& ins) { BalknapParams p; p.ub = 'b'; p.k = 3; return sopt_balknap(ins, p, Info()).profit(); }
Profit opt_balknap_1_t_test(Instance& ins) { BalknapParams p; p.ub = 't'; p.k = 1; return sopt_balknap(ins, p, Info()).profit(); }
Profit opt_balknap_2_t_test(Instance& ins) { BalknapParams p; p.ub = 't'; p.k = 2; return sopt_balknap(ins, p, Info()).profit(); }
Profit opt_balknap_3_t_test(Instance& ins) { BalknapParams p; p.ub = 't'; p.k = 3; return sopt_balknap(ins, p, Info()).profit(); }

std::vector<Profit (*)(Instance&)> f = {
        opt_minknap,
        opt_balknap_1_b_test,
        opt_balknap_2_b_test,
        opt_balknap_3_b_test,
        opt_balknap_1_t_test,
        opt_balknap_2_t_test,
        opt_balknap_3_t_test,
};

TEST(balknap, TEST)  { test(TEST, f); }
TEST(balknap, SMALL) { test(SMALL, f); }

*/

