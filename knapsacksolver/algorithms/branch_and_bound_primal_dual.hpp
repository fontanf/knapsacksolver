#pragma once

#include "knapsacksolver/solution.hpp"

#include <thread>

namespace knapsacksolver
{

struct BranchAndBoundPrimalDualOptionalParameters
{
    Info info = Info();

    bool greedy = true;
    StateIdx greedy_nlogn = -1;
    StateIdx surrogate_relaxation = -1;
    bool combo_core = false;

    bool* end = NULL;
    bool stop_if_end = false;
    bool set_end = true;

    BranchAndBoundPrimalDualOptionalParameters& set_pure()
    {
        greedy = false;
        greedy_nlogn = -1;
        surrogate_relaxation = -1;
        combo_core = false;
        return *this;
    }

    BranchAndBoundPrimalDualOptionalParameters& set_combo()
    {
        greedy = true;
        greedy_nlogn = 50000;
        surrogate_relaxation = 20000;
        combo_core = true;
        return *this;
    }

};

struct BranchAndBoundPrimalDualOutput: Output
{
    BranchAndBoundPrimalDualOutput(
            const Instance& instance,
            Info& info):
        Output(instance, info) { }

    Counter number_of_node = 0;

    BranchAndBoundPrimalDualOutput& algorithm_end(Info& info)
    {
        info.add_to_json("Algorithm", "NodeNumber", number_of_node);
        Output::algorithm_end(info);
        info.os() << "Node number: " << number_of_node << std::endl;
        return *this;
    }
};

BranchAndBoundPrimalDualOutput branch_and_bound_primal_dual(
        Instance& instance,
        BranchAndBoundPrimalDualOptionalParameters parameters = {});

}

