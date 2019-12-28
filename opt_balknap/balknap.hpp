#pragma once

#include "knapsack/lib/instance.hpp"
#include "knapsack/lib/solution.hpp"
#include "knapsack/lib/part_solution_1.hpp"

#include <thread>
#include <map>

namespace knapsack
{

struct BalknapOptionalParameters
{
    Info info = Info();

    char ub                       = 't';
    bool greedy                   = true;
    StateIdx greedynlogn          = -1;
    StateIdx surrogate            = -1;
    ItemPos partial_solution_size = 64;

    bool* end = NULL;
    bool stop_if_end = false;
    bool set_end = true;

    BalknapOptionalParameters& pure()
    {
        ub = 'b';
        greedy = false;
        greedynlogn = -1;
        surrogate = -1;
        partial_solution_size = 64;
        return *this;
    }

    BalknapOptionalParameters& combo()
    {
        ub = 't';
        greedy = -1;
        greedynlogn = 0;
        surrogate = 2000;
        partial_solution_size = 64;
        return *this;
    }

    BalknapOptionalParameters& set_params(const std::vector<std::string>& argv)
    {
        for (auto it = argv.begin() + 1; it != argv.end(); ++it) {
            if        (*it == "u")  { ub                     = (*(++it))[0];
            } else if (*it == "g")  { greedy                 = (*(++it) == "true");
            } else if (*it == "gs") { greedynlogn            = std::stol(*(++it));
            } else if (*it == "s")  { surrogate              = std::stol(*(++it));
            } else if (*it == "k")  { partial_solution_size  = std::stol(*(++it)); }
        }
        return *this;
    }

};

struct BalknapOutput: Output
{
    BalknapOutput(const Instance& ins, Info& info): Output(ins, info) { }
    Cpt recursive_call_number = 0;
};

BalknapOutput sopt_balknap(Instance& ins, BalknapOptionalParameters p = {});

}

