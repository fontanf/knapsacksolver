#include "knapsack/opt_bab/bab.hpp"
#include "knapsack/opt_astar/astar.hpp"
#include "knapsack/opt_bellman/bellman.hpp"
#include "knapsack/opt_dpprofits/dpprofits.hpp"
#include "knapsack/opt_expknap/expknap.hpp"
#include "knapsack/opt_balknap/balknap.hpp"
#include "knapsack/opt_minknap/minknap.hpp"

#include "knapsack/lib/tester.hpp"
#include "knapsack/lib/generator.hpp"

using namespace knapsack;

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    //GenerateData data;
    //data.n = 2;
    //data.t = "u";
    //data.r = 2;
    //data.h = 1;
    //data.s = 5;
    //Instance ins = generate(data);

    Instance ins = test_instance(1);

    Logger logger;
    Info info(logger, true);
    sopt_astar_dp(ins, info);
    return 0;
}

