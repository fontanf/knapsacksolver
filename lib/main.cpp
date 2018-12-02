#include "knapsack/opt_bab/bab.hpp"
#include "knapsack/opt_babstar/bab.hpp"
#include "knapsack/opt_bellman/bellman.hpp"
#include "knapsack/opt_dpprofits/dpprofits.hpp"
#include "knapsack/opt_expknap/expknap.hpp"
#include "knapsack/opt_balknap/balknap.hpp"
#include "knapsack/opt_minknap/minknap.hpp"

#include "knapsack/lib/generator.hpp"

using namespace knapsack;

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    /*
    {
        Instance ins = generate("sc", 1000, 1000, 50);
        Info info;
        info.set_verbose();
        //info.set_debug();
        //info.set_debuglive();
        Solution sopt2 = sopt_bellman_array_rec(ins, info);
        return 0;
    }
    */

    GenerateData data;
    data.spanner = false;
    data.v = 50;
    data.m = 50;

    double t1 = 0;
    double t2 = 0;

    data.type = "sc";
    data.n = 25;
    data.r = 100;
    data.seed = 5;
    for (data.h=1; data.h<=100; ++data.h) {
        Instance ins = generate(data);

        Instance ins1(ins);
        Info info1;
        Solution sopt1 = sopt_minknap_list_part(ins1, info1);
        t1 += info1.pt.get<double>("Solution.Time");

        Instance ins2(ins);
        Info info2;
        Solution sopt2 = sopt_bellman_array_part(ins2, info2);
        t2 += info2.pt.get<double>("Solution.Time");
    }

    std::cout << STR1(t1) << std::endl;
    std::cout << STR1(t2) << std::endl;

    return 0;
}

