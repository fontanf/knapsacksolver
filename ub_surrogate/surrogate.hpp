#pragma once

#include "knapsack/lib/instance.hpp"
#include "knapsack/lib/solution.hpp"

namespace knapsack
{

struct SurrelaxData
{
    Instance ins;
    knapsack::Output& output;
    std::function<knapsack::Output (Instance&, Info, bool*)> func;
    bool* end;
    Info info = Info();
};

void ub_solvesurrelax(SurrelaxData d);

}

