#include "knapsack/lib/tester.hpp"
#include "knapsack/opt_minknap/minknap.hpp"
#include "knapsack/opt_balknap/balknap.hpp"

using namespace knapsack;

Profit opt_minknap_test(Instance& ins)     { Logger logger; Info info(logger, true); return sopt_minknap(ins, info).profit(); }
Profit opt_balknap_1_b_test(Instance& ins) { Logger logger; Info info(logger, true); BalknapParams p; p.ub_type = 'b'; return sopt_balknap(ins, info, p, 1).profit(); }
Profit opt_balknap_2_b_test(Instance& ins) { Logger logger; Info info(logger, true); BalknapParams p; p.ub_type = 'b'; return sopt_balknap(ins, info, p, 2).profit(); }
Profit opt_balknap_3_b_test(Instance& ins) { Logger logger; Info info(logger, true); BalknapParams p; p.ub_type = 'b'; return sopt_balknap(ins, info, p, 3).profit(); }
Profit opt_balknap_1_t_test(Instance& ins) { Logger logger; Info info(logger, true); BalknapParams p; p.ub_type = 't'; return sopt_balknap(ins, info, p, 1).profit(); }
Profit opt_balknap_2_t_test(Instance& ins) { Logger logger; Info info(logger, true); BalknapParams p; p.ub_type = 't'; return sopt_balknap(ins, info, p, 2).profit(); }
Profit opt_balknap_3_t_test(Instance& ins) { Logger logger; Info info(logger, true); BalknapParams p; p.ub_type = 't'; return sopt_balknap(ins, info, p, 3).profit(); }

std::vector<Profit (*)(Instance&)> f = {
        opt_minknap_test,
        opt_balknap_1_b_test,
        opt_balknap_2_b_test,
        opt_balknap_3_b_test,
        opt_balknap_1_t_test,
        opt_balknap_2_t_test,
        opt_balknap_3_t_test,
};

TEST(Bellman, TEST)  { test(TEST, f); }
TEST(Bellman, SMALL) { test(SMALL, f); }

