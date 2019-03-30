#include "knapsack/opt_bab/bab.hpp"
#include "knapsack/opt_astar/astar.hpp"
#include "knapsack/opt_bellman/bellman.hpp"
#include "knapsack/opt_dpprofits/dpprofits.hpp"
#include "knapsack/opt_expknap/expknap.hpp"
#include "knapsack/opt_balknap/balknap.hpp"
#include "knapsack/opt_minknap/minknap.hpp"
#include "knapsack/lb_greedy/greedy.hpp"
#include "knapsack/lb_greedynlogn/greedynlogn.hpp"
#include "knapsack/ub_surrogate/surrogate.hpp"

#include "knapsack/lib/tester.hpp"
#include "knapsack/lib/generator.hpp"

using namespace knapsack;

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    GenerateData data;
    data.n = 2000;
    data.t = "circle";
    data.r = 1000;
    data.d = 2.0 / 3;
    data.h = 81;
    data.s = 3081;
    Instance ins = generate(data);

    MinknapParams p = MinknapParams::pure();
    Info info = Info("log.txt")
        .set_verbose(true)
        //.set_log2stderr(true)
        ;
    Minknap(ins, p).run(info);
    //ExpknapParams p = ExpknapParams::fontan();
    //p.time_limit = 600;
    //Expknap(ins, p).run(info);

}

