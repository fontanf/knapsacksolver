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

    {
        GenerateData data;
        data.n    = 2;
        data.type = "u";
        data.r    = 2;
        data.h    = 1;
        data.seed = 5;
        Instance ins = generate(data);
        Info info;
        info.set_verbose();
        info.set_debug();
        info.set_debuglive();
        BalknapParams p;
        p.ub_type = 'b';
        sopt_balknap(ins, info, p, 1);
        return 0;
    }

    GenerateData data;
    data.spanner = false;
    data.v = 50;
    data.m = 50;

    double t1 = 0;
    double t2 = 0;

    data.type = "sc";
    data.n = 100;
    data.r = 100;
    data.seed = 0;
    for (data.h=1; data.h<=100; ++data.h) {
        std::cout << data.h << std::endl;
        Instance ins = generate(data);

        Instance ins1(ins);
        Info info1;
        info1.set_verbose();
        BalknapParams p1;
        p1.ub_type = 't';
        p1.cpt_greedynlogn = 0;
        p1.cpt_surrogate = 0;
        p1.cpt_solve_sur = -1;
        sopt_balknap(ins1, info1, p1);
        t1 += info1.pt.get<double>("Solution.Time");

        Instance ins2(ins);
        Info info2;
        info2.set_verbose();
        sopt_balknap(ins2, info2);
        t2 += info2.pt.get<double>("Solution.Time");
        std::cout << std::endl;
    }

    std::cout << STR1(t1) << std::endl;
    std::cout << STR1(t2) << std::endl;

    return 0;
}

