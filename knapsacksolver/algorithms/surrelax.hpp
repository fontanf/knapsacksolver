#pragma once

#include "knapsacksolver/instance.hpp"
#include "knapsacksolver/solution.hpp"

namespace knapsacksolver
{

struct SurrelaxData
{
    Instance ins;
    Output& output;
    std::function<Output (Instance&, Info, bool*)> func;
    bool* end;
    Info info = Info();
};

void solvesurrelax(SurrelaxData d);

Output surrelax(const Instance& ins, Info info = Info());
Output surrelax_minknap(const Instance& ins, Info info = Info());

}

