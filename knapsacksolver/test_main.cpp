#include "knapsacksolver/algorithms/algorithms.hpp"
#include "knapsacksolver/tester.hpp"
#include "knapsacksolver/generator.hpp"

using namespace knapsacksolver;

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    Generator data;
    data.n = 1000000;
    data.normal = true;
    data.dw = 10;
    data.d = 10;
    data.t = "normal";
    data.r = 100000000;
    data.x = 0.5;
    data.s = 296;
    Instance ins = data.generate();
    //Instance ins(95, {{24, 34}, {42, 52}, {32, 42}, {4, 14}});
    //Instance ins(40, {{11, 12}, {11, 12}, {11, 12}, {10, 10}, {10, 10}, {10, 10}, {10, 10}});
    //std::cout << ins << std::endl;

    Info info = Info()
        .set_verbosity_level(1)
        //.set_log2stderr(true)
        //.set_logfile("log.txt")
        ;

    DynamicProgrammingPrimalDualOptionalParameters p;
    p.info = info;
    p.set_combo();
    auto output = dynamic_programming_primal_dual(ins, p);

}

