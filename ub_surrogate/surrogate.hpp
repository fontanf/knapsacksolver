#pragma once

#include "knapsack/lib/instance.hpp"
#include "knapsack/lib/solution.hpp"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

namespace knapsack
{

struct SurrelaxData
{
    Instance ins;
    Profit& lb;
    Solution& sol_best;
    Profit& ub;
    std::function<Solution(Instance&, Info, bool*)> func;
    bool* end;
    Info info = Info();
};

void ub_solvesurrelax(SurrelaxData d);

}

