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

    ExpknapOptionalParameters& set_params(const std::map<std::string, std::string>& args)
    {
        auto it = args.end();
        if ((it = args.find("g"))  != args.end()) greedy      = (it->second == "true");
        if ((it = args.find("gn")) != args.end()) greedynlogn = std::stol(it->second);
        if ((it = args.find("s"))  != args.end()) surrogate   = std::stol(it->second);
        if ((it = args.find("c"))  != args.end()) combo_core  = (it->second == "true");
        return *this;
    }

};

struct ExpknapOutput: Output
{
    ExpknapOutput(const Instance& ins): Output(ins) { }
    Cpt node_number = 0;
};

ExpknapOutput sopt_expknap(Instance& ins, ExpknapOptionalParameters p);

}

