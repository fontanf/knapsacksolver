#pragma once

#include "knapsack/lib/instance.hpp"
#include "knapsack/lib/solution.hpp"
#include "knapsack/lib/part_solution_2.hpp"

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
        greedy = -1;
        pairing = -1;
        surrogate = -1;
        combo_core = false;
        partial_solution_size = 64;
        return *this;
    }

    MinknapOptionalParameters& combo()
    {
        greedy = 0;
        pairing = 10000;
        surrogate = 2000;
        combo_core = true;
        partial_solution_size = 64;
        return *this;
    }

    MinknapOptionalParameters& set_params(const std::map<std::string, std::string>& args)
    {
        auto it = args.end();
        if ((it = args.find("k"))  != args.end()) partial_solution_size = std::stol(it->second);
        if ((it = args.find("g"))  != args.end()) greedy                = (it->second == "true");
        if ((it = args.find("p"))  != args.end()) pairing               = std::stol(it->second);
        if ((it = args.find("s"))  != args.end()) surrogate             = std::stol(it->second);
        if ((it = args.find("c"))  != args.end()) combo_core            = (it->second == "true");
        return *this;
    }

};

struct MinknapOutput: Output
{
    MinknapOutput(const Instance& ins): Output(ins) { }
    Cpt recursive_call_number = 0;
};

MinknapOutput sopt_minknap(Instance& ins, MinknapOptionalParameters p = {});

}

