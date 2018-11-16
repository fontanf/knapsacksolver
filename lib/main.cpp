//#include "knapsack/opt_bab/bab.hpp"
#include "knapsack/opt_babstar/bab.hpp"
//#include "knapsack/opt_bellman/bellman_list.hpp"
//#include "knapsack/opt_bellman/bellman_array.hpp"
//#include "knapsack/opt_expknap/expknap.hpp"
//#include "knapsack/opt_balknap/balknap.hpp"
#include "knapsack/opt_minknap/minknap.hpp"

#include "knapsack/lib/generator.hpp"

using namespace knapsack;

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    Instance ins = generate("u", 10, 10, 25);
    std::cout << ins << std::endl;

    Instance ins1(ins);
    Info info1(true, false);
    Solution sopt1 = sopt_minknap_list_part(ins1, info1);
    std::cout << "OPT1 " << sopt1.profit() << std::endl;
    //double t1 = info1.elapsed_time();

    Instance ins2(ins);
    Info info2(true, true);
    Solution sopt2 = sopt_babstar_dp(ins2, info2);
    //double t2 = info2.elapsed_time();
    std::cout << "OPT2 " << sopt2.profit() << std::endl;
    std::cout << info2.debug_string << std::endl;

    return 0;
}
