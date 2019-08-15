#pragma once

#include "knapsack/lib/instance.hpp"
#include "knapsack/lib/solution.hpp"
#include "knapsack/ub_surrogate/surrogate.hpp"

#include <thread>

namespace knapsack
{

struct ExpknapParams
{
    StateIdx greedy = 0;
    StateIdx greedynlogn = -1;
    StateIdx surrogate = -1;
    bool combo_core = false;

    static ExpknapParams pure()
    {
        return {
            .greedy = -1,
            .greedynlogn = -1,
            .surrogate = -1,
            .combo_core = false,
        };
    }

    static ExpknapParams combo()
    {
        return {
            .greedy = 0,
            .greedynlogn = 50000,
            .surrogate = 20000,
            .combo_core = true,
        };
    }

    ExpknapParams& set_params(const std::map<std::string, std::string>& args)
    {
        auto it = args.end();
        if ((it = args.find("g"))  != args.end()) greedy      = std::stol(it->second);
        if ((it = args.find("gn")) != args.end()) greedynlogn = std::stol(it->second);
        if ((it = args.find("s"))  != args.end()) surrogate   = std::stol(it->second);
        if ((it = args.find("c"))  != args.end()) combo_core  = (it->second == "1");
        return *this;
    }

};

Solution sopt_expknap(Instance& ins, ExpknapParams p, Info info = Info());

}

