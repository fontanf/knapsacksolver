#pragma once

#include "knapsack/lib/instance.hpp"
#include "knapsack/lib/solution.hpp"
#include "knapsack/lib/part_solution_1.hpp"

#include <thread>
#include <map>

namespace knapsack
{

/*
struct BalknapParams
{
    ItemPos k            = 64;
    char ub              = 't';
    StateIdx greedy      = 0;
    StateIdx greedynlogn = -1;
    StateIdx surrogate   = -1;

    static BalknapParams pure()
    {
        return {
            .k = 64,
            .ub = 'b',
            .greedy = -1,
            .greedynlogn = -1,
            .surrogate = -1,
        };
    }

    static BalknapParams combo()
    {
        return {
            .k = 64,
            .ub = 't',
            .greedy = -1,
            .greedynlogn = 0,
            .surrogate = 2000,
        };
    }

    BalknapParams& set_params(const std::map<std::string, std::string>& args)
    {
        auto it = args.end();
        if ((it = args.find("u"))  != args.end()) ub          = it->second[0];
        if ((it = args.find("k"))  != args.end()) k           = std::stol(it->second);
        if ((it = args.find("g"))  != args.end()) greedy      = std::stol(it->second);
        if ((it = args.find("gn")) != args.end()) greedynlogn = std::stol(it->second);
        if ((it = args.find("s"))  != args.end()) surrogate   = std::stol(it->second);
        return *this;
    }

};

Solution sopt_balknap(Instance& ins, BalknapParams p, Info info = Info());
*/

}

