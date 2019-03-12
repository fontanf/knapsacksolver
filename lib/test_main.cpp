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
    data.n = 8;
    data.t = "u";
    data.r = 7;
    data.h = 50;
    data.s = 8;
    Instance ins = generate(data);

    //Instance ins = test_instance(1);

    //Instance ins(3, 3);
    //ins.add_items({{2,1}, {2,2}, {3,3}});
    //std::cout << ins << std::endl;

    Logger logger("log.txt", true);
    Info info(logger, true);
    {
        Instance ins_tmp(ins);
        Info info_tmp(logger);
        MinknapParams p = MinknapParams::pure();
        p.k = 1;
        Minknap(ins_tmp, p).run(info_tmp);
    }
    {
        Instance ins_tmp(ins);
        Info info_tmp(logger);
        MinknapParams p = MinknapParams::pure();
        p.k = 3;
        p.combo_core = true;
        Minknap(ins_tmp, p).run(info_tmp);
    }
    return 0;
}

