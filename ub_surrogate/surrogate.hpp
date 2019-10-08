#pragma once

#include "knapsack/lib/instance.hpp"
#include "knapsack/lib/solution.hpp"

namespace knapsack
{

struct SurrelaxData
{
    Instance ins;
    Output& output;
    std::function<Output (Instance&, Info, bool*)> func;
    bool* end;
    Info info = Info();
};

void ub_solvesurrelax(SurrelaxData d);

Output ub_surrelax(const Instance& ins, Info info = Info());
Output ub_surrelax_minknap(const Instance& ins, Info info = Info());

}

