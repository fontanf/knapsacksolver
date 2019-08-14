#include "knapsack/lib/algorithms.hpp"
#include "knapsack/lib/tester.hpp"
#include "knapsack/lib/generator.hpp"

using namespace knapsack;

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    GenerateData data;
    data.n = 2000;
    data.t = "asc";
    data.r = 100000;
    data.h = 80;
    data.s = 102080;
    Instance ins = generate(data);
    //std::cout << ins << std::endl;

    Info info = Info()
        .set_verbose(true)
        //.set_log2stderr(true)
        .set_logfile("log.txt")
        ;
    //Profit opt = opt_bellman_array(ins, info);
    //std::cout << opt << std::endl;
    //Solution sol = sopt_bellman_array_all(ins, info);
    //std::cout << "profit " << sol.profit() << std::endl;
    //for (ItemIdx j=0; j<ins.item_number(); ++j)
        //std::cout << j << ":" << sol.contains(j) << " ";
    //std::cout << std::endl;

    Solution sol_minknap = Minknap(ins, MinknapParams::combo()).run(info);

}

