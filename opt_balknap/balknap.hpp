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

    BalknapOptionalParameters& set_params(const std::map<std::string, std::string>& args)
    {
        auto it = args.end();
        if ((it = args.find("u"))  != args.end()) ub                    = it->second[0];
        if ((it = args.find("g"))  != args.end()) greedy                = (it->second == "true");
        if ((it = args.find("gn")) != args.end()) greedynlogn           = std::stol(it->second);
        if ((it = args.find("s"))  != args.end()) surrogate             = std::stol(it->second);
        if ((it = args.find("k"))  != args.end()) partial_solution_size = std::stol(it->second);
        return *this;
    }

};

struct BalknapOutput: Output
{
    BalknapOutput(const Instance& ins): Output(ins) { }
    Cpt recursive_call_number = 0;
};

BalknapOutput sopt_balknap(Instance& ins, BalknapOptionalParameters p = {});

}

