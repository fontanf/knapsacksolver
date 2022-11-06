#pragma once

#include "knapsacksolver/solution.hpp"

#include <thread>

namespace knapsacksolver
{

struct DynamicProgrammingPrimalDualOptionalParameters
{
    Info info = Info();

    bool greedy = true;
    StateIdx pairing = -1;
    StateIdx surrogate_relaxation = -1;
    bool combo_core = false;
    ItemIdx partial_solution_size = 64;

    // If "stop_if_end" == true, the algorithm will stop when *end"" becomes
    // equal to "true".
    // If end == NULL, then a new pointer will be allocated and deallocated.
    // If set_end == true, then "*end" will be set to "true" at the end of the
    // algorithm.
    // This is used for parallelization.
    // Normal users should not need to change "end" and "set_end" default values.
    bool* end = NULL;
    bool stop_if_end = false;
    bool set_end = true;

    DynamicProgrammingPrimalDualOptionalParameters& set_pure()
    {
        greedy = false;
        pairing = -1;
        surrogate_relaxation = -1;
        combo_core = false;
        partial_solution_size = 64;
        return *this;
    }

    DynamicProgrammingPrimalDualOptionalParameters& set_combo()
    {
        greedy = true;
        pairing = 10000;
        surrogate_relaxation = 2000;
        combo_core = true;
        partial_solution_size = 64;
        return *this;
    }
};

struct DynamicProgrammingPrimalDualOutput: Output
{
    DynamicProgrammingPrimalDualOutput(
            const Instance& instance,
            Info& info):
        Output(instance, info) { }

    Counter number_of_recursive_calls = 0;

    DynamicProgrammingPrimalDualOutput& algorithm_end(Info& info)
    {
        info.add_to_json("Algorithm", "NumberOfRecursiveCalls", number_of_recursive_calls);
        Output::algorithm_end(info);
        info.os() << "Number of recursive calls:  " << number_of_recursive_calls << std::endl;
        return *this;
    }
};

DynamicProgrammingPrimalDualOutput dynamic_programming_primal_dual(
        Instance& instance,
        DynamicProgrammingPrimalDualOptionalParameters parameters = {});

}

