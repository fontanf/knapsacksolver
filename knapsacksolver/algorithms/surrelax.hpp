#pragma once

#include "knapsacksolver/solution.hpp"

namespace knapsacksolver
{

struct SurrelaxData
{
    SurrelaxData(Output& output): output(output) { }
    Instance instance;
    Output& output;
    std::function<Output (Instance&, Info, bool*)> func;
    bool* end;
    Info info = Info();
};

void solvesurrelax(SurrelaxData d);

Output surrelax(const Instance& instance, Info info = Info());
Output surrelax_minknap(const Instance& instance, Info info = Info());

}

