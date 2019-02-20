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

    //GenerateData data;
    //data.n = 5;
    //data.t = "sc";
    //data.r = 10;
    //data.h = 1;
    //data.s = 3;
    //Instance ins = generate(data);

    //Instance ins = test_instance(1);

    Instance ins(3, 3);
    ins.add_items({{2,1}, {2,2}, {3,3}});
    std::cout << ins << std::endl;

    Logger logger("log.txt", true);
    Info info(logger, true);
    ins.sort_partially(info, -1);
    return 0;
}

