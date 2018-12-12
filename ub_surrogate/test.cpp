#include "knapsack/lib/tester.hpp"
#include "knapsack/opt_minknap/minknap.hpp"
#include "knapsack/ub_surrogate/surrogate.hpp"
#include "knapsack/lb_greedy/greedy.hpp"
#include "knapsack/lb_greedynlogn/greedynlogn.hpp"

using namespace knapsack;

Profit opt_minknap_test(Instance& ins)  { Logger logger; Info info(logger, true); return sopt_minknap(ins, info).profit(); }
Profit ub_surrogate_test(Instance& ins)
{
    Logger logger;
    Info info(logger, true);
    Info info_tmp(info.logger);
    //Profit lb = sol_greedynlogn(ins, info_tmp).profit();
    ins.sort_partially();
    Profit lb = sol_greedy(ins, info_tmp).profit();
    return ub_surrogate(ins, lb, info).ub;
}

std::vector<Profit (*)(Instance&)> f = {
        opt_minknap_test,
        ub_surrogate_test,
};

TEST(surrogate, TEST)  { test(TEST, f, UB); }
TEST(surrogate, SMALL) { test(SMALL, f, UB); }

