#pragma once

#include "knapsack/lib/instance.hpp"
#include "knapsack/lib/solution.hpp"

namespace knapsack
{

struct SurrelaxData
{
    Instance ins;
    Profit* lb;
    Solution* sol_best;
    Profit& ub;
    std::function<Solution(Instance&, Info, bool*)> func;
    bool* end;
    Info info = Info();

    Profit lowerbound()
    {
        assert(sol_best != NULL || lb != NULL);
        if (sol_best == NULL) {
            return *lb;
        } else if (lb == NULL) {
            return sol_best->profit();
        } else {
            return std::max(*lb, sol_best->profit());
        }
    }

};

void ub_solvesurrelax(SurrelaxData d);

}

