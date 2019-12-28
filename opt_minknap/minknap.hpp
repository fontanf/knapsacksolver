#pragma once

#include "knapsack/lib/solution.hpp"

#include <thread>

namespace knapsack
{

using namespace knapsack;

struct MinknapOptionalParameters
{
    Info info = Info();

    bool greedy = true;
    StateIdx pairing = -1;
    StateIdx surrogate = -1;
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

    MinknapOptionalParameters& pure()
    {
        greedy = false;
        pairing = -1;
        surrogate = -1;
        combo_core = false;
        partial_solution_size = 64;
        return *this;
    }

    MinknapOptionalParameters& combo()
    {
        greedy = true;
        pairing = 10000;
        surrogate = 2000;
        combo_core = true;
        partial_solution_size = 64;
        return *this;
    }

    MinknapOptionalParameters& set_params(const std::vector<std::string>& argv)
    {
        for (auto it = argv.begin() + 1; it != argv.end(); ++it) {
            if        (*it == "g") { greedy                 = (*(++it) == "true");
            } else if (*it == "p") { pairing                = std::stol(*(++it));
            } else if (*it == "s") { surrogate              = std::stol(*(++it));
            } else if (*it == "c") { combo_core             = (*(++it) == "true");
            } else if (*it == "k") { partial_solution_size  = std::stol(*(++it)); }
        }
        return *this;
    }

};

struct MinknapOutput: Output
{
    MinknapOutput(const Instance& ins, Info& info): Output(ins, info) { }
    Cpt recursive_call_number = 0;
    void algorithm_end(Info& info)
    {
        Output::algorithm_end(info);
        PUT(info, "Algorithm", "RecursiveCallNumber", recursive_call_number);
        VER(info, "Recursive call number: " << recursive_call_number << std::endl);
    }
};

MinknapOutput sopt_minknap(Instance& ins, MinknapOptionalParameters p = {});

}

