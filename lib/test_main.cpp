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
    data.n = 50;
    data.t = "u";
    data.r = 100;
    data.h = 40;
    data.s = 2;
    Instance ins = generate(data);
    Info info = Info()
        .set_verbose(true)
        .set_log2stderr(true)
        .set_logfile("log.txt")
        ;
    //opt_bellman_array(ins, info);

    MinknapParams p = MinknapParams::pure();
    //p.ub_surrogate = -1;
    p.pairing = 10;
    Minknap(ins, p).run(info);

}

