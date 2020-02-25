#pragma once

#include "knapsacksolver/solution.hpp"

#include <thread>

namespace knapsacksolver
{

struct MinknapOptionalParameters
{
    Info info = Info();

    bool greedy = true;
    StateIdx pairing = -1;
    StateIdx surrelax = -1;
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

    MinknapOptionalParameters& set_pure()
    {
        greedy = false;
        pairing = -1;
        surrelax = -1;
        combo_core = false;
        partial_solution_size = 64;
        return *this;
    }

    MinknapOptionalParameters& set_combo()
    {
        greedy = true;
        pairing = 10000;
        surrelax = 2000;
        combo_core = true;
        partial_solution_size = 64;
        return *this;
    }
};

struct MinknapOutput: Output
{
    MinknapOutput(const Instance& instance, Info& info): Output(instance, info) { }

    Counter recursive_call_number = 0;

    void algorithm_end(Info& info)
    {
        PUT(info, "Algorithm", "RecursiveCallNumber", recursive_call_number);
        Output::algorithm_end(info);
        VER(info, "Recursive call number: " << recursive_call_number << std::endl);
    }
};

MinknapOutput minknap(Instance& instance, MinknapOptionalParameters p = {});

}

