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
    data.t = "wc";
    data.r = 100;
    //data.d = 2.0 / 3;
    data.h = 10;
    data.s = 2;
    Instance ins = generate(data);

    //opt_bellman_array(ins, Info().set_verbose(true));

    {
        Info info = Info().set_verbose(true);
        //MinknapParams p = MinknapParams::pure();
        //p.combo_core = true;
        auto p = MinknapParams::pure();
        p.k = 3;
        p.combo_core = true;
        Minknap(ins, p).run(info);
    }

    //{
        //Info info = Info().set_verbose(true);
        //SurrogateOut so = ub_surrogate(ins, opt, info);
        //std::cout << so.ub << " " << so.bound << std::endl;
    //}

    //Info info = Info("log.txt")
        //.set_verbose(true)
        //.set_log2stderr(true)
        ;
    //MinknapParams p = MinknapParams::pure();
    //Minknap(ins, p).run(info);
    //ExpknapParams p = ExpknapParams::fontan();
    //Expknap(ins, p).run(info);

}

