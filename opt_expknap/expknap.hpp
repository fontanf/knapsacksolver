#pragma once

#include "knapsack/lib/instance.hpp"
#include "knapsack/lib/solution.hpp"
#include "knapsack/ub_surrogate/surrogate.hpp"

#include <thread>

namespace knapsack
{

struct ExpknapOptionalParameters
{
    Info info = Info();

    bool greedy = true;
    StateIdx greedynlogn = -1;
    StateIdx surrogate = -1;
    bool combo_core = false;

    bool* end = NULL;
    bool stop_if_end = false;
    bool set_end = true;

    ExpknapOptionalParameters& pure()
    {
        greedy = false;
        greedynlogn = -1;
        surrogate = -1;
        combo_core = false;
        return *this;
    }

    ExpknapOptionalParameters& combo()
    {
        greedy = true;
        greedynlogn = 50000;
        surrogate = 20000;
        combo_core = true;
        return *this;
    }

    ExpknapOptionalParameters& set_params(const std::vector<std::string>& argv)
    {
        for (auto it = argv.begin() + 1; it != argv.end(); ++it) {
            if        (*it == "g")  { greedy      = (*(++it) == "true");
            } else if (*it == "gs") { greedynlogn = std::stol(*(++it));
            } else if (*it == "s")  { surrogate   = std::stol(*(++it));
            } else if (*it == "c")  { combo_core  = (*(++it) == "true"); }
        }
        return *this;
    }

};

struct ExpknapOutput: Output
{
    ExpknapOutput(const Instance& ins, Info& info): Output(ins, info) { }
    Cpt node_number = 0;
};

ExpknapOutput sopt_expknap(Instance& ins, ExpknapOptionalParameters p = {});

}

