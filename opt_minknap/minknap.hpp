#pragma once

#include "knapsack/lib/instance.hpp"
#include "knapsack/lib/solution.hpp"
#include "knapsack/lib/part_solution_2.hpp"

#include <thread>

namespace knapsack
{

using namespace knapsack;

struct MinknapParams
{
    Cpt k = 64;
    StateIdx greedy = 0;
    StateIdx greedynlogn = -1;
    StateIdx pairing = -1;
    StateIdx surrogate = -1;
    bool combo_core = false;

    static MinknapParams pure()
    {
        return {
            .k = 64,
            .greedy = -1,
            .greedynlogn = -1,
            .pairing = -1,
            .surrogate = -1,
            .combo_core = false,
        };
    }

    static MinknapParams combo()
    {
        return {
            .k = 64,
            .greedy = 0,
            .greedynlogn = -1,
            .pairing = 10000,
            .surrogate = 2000,
            .combo_core = true,
        };
    }

    MinknapParams& set_params(const std::map<std::string, std::string>& args)
    {
        auto it = args.end();
        if ((it = args.find("k"))  != args.end()) k           = std::stol(it->second);
        if ((it = args.find("g"))  != args.end()) greedy      = std::stol(it->second);
        if ((it = args.find("gn")) != args.end()) greedynlogn = std::stol(it->second);
        if ((it = args.find("p"))  != args.end()) pairing     = std::stol(it->second);
        if ((it = args.find("s"))  != args.end()) surrogate   = std::stol(it->second);
        if ((it = args.find("c"))  != args.end()) combo_core  = (it->second == "1");
        return *this;
    }

};

Solution sopt_minknap(Instance& ins, MinknapParams p, Info info = Info());

}

