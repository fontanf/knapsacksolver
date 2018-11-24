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

    std::string t = "asc";
    ItemIdx i = 150;
    Weight r = 10000;
    int h = 58;
    std::cout << t << " " << i << " " << r << " " << h << std::endl;
    std::cout << std::endl;
    Instance ins = generate(t, i, r, h);

    Instance ins1(ins);
    Info info1;
    info1.set_verbose();
    Solution sopt1 = sopt_minknap_list_part(ins1, info1);

    std::cout << std::endl;

    Instance ins2(ins);
    Info info2;
    info2.set_verbose();
    StarknapParams p;
    p.upper_bound = "t";
    p.lb_greedynlogn = 0;
    Solution sopt2 = sopt_starknap(ins2, info2, p);

    std::cout << std::endl;

    Instance ins3(ins);
    Info info3;
    info3.set_verbose();
    Solution sopt3 = sopt_expknap(ins2, info3);

    return 0;
}
