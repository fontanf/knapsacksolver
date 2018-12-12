#include "knapsack/opt_bab/bab.hpp"
#include "knapsack/opt_astar/astar.hpp"
#include "knapsack/opt_bellman/bellman.hpp"
#include "knapsack/opt_dpprofits/dpprofits.hpp"
#include "knapsack/opt_expknap/expknap.hpp"
#include "knapsack/opt_balknap/balknap.hpp"
#include "knapsack/opt_minknap/minknap.hpp"
#include "knapsack/lb_greedy/greedy.hpp"
#include "knapsack/lb_greedynlogn/greedynlogn.hpp"

#include "knapsack/lib/tester.hpp"
#include "knapsack/lib/generator.hpp"

using namespace knapsack;

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    GenerateData data;
    data.n = 50;
    data.t = "ss";
    data.r = 100;
    data.h = 100;
    data.s = 5;
    Instance ins = generate(data);

    //Instance ins = test_instance(1);

    Logger logger("", true);
    Info info(logger, true);
    ins.sort_partially();
    MinknapParams p;
    sopt_minknap(ins, info, p, 3);
    return 0;
}

