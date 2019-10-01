#include "knapsack/lib/algorithms.hpp"
#include "knapsack/lib/tester.hpp"
#include "knapsack/lib/generator.hpp"

using namespace knapsack;

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    //Generator data;
    //data.n = 10000;
    //data.normal = true;
    //data.dw = 10;
    //data.t = "normal";
    //data.r = 100000000;
    //data.d = 10;
    //data.x = 0.3;
    //data.s = 174;
    //Instance ins = data.generate();
    Instance ins(95, {{24, 34}, {42, 52}, {32, 42}, {4, 14}});
    //Instance ins(40, {{11, 12}, {11, 12}, {11, 12}, {10, 10}, {10, 10}, {10, 10}, {10, 10}});
    //std::cout << ins << std::endl;

    Info info = Info()
        .set_verbose(true)
        //.set_log2stderr(true)
        .set_logfile("log.txt")
        ;

    BalknapOptionalParameters p;
    p.info = info;
    p.partial_solution_size = 1;
    auto output = sopt_balknap(ins, p);

}

