#pragma once

#include "knapsacksolver/solution.hpp"

#include <thread>

namespace knapsacksolver
{

struct ExpknapOptionalParameters
{
    Info info = Info();

    bool greedy = true;
    StateIdx greedynlogn = -1;
    StateIdx surrelax = -1;
    bool combo_core = false;

    bool* end = NULL;
    bool stop_if_end = false;
    bool set_end = true;

    ExpknapOptionalParameters& set_pure()
    {
        greedy = false;
        greedynlogn = -1;
        surrelax = -1;
        combo_core = false;
        return *this;
    }

    ExpknapOptionalParameters& set_combo()
    {
        greedy = true;
        greedynlogn = 50000;
        surrelax = 20000;
        combo_core = true;
        return *this;
    }

};

struct ExpknapOutput: Output
{
    ExpknapOutput(const Instance& instance, Info& info): Output(instance, info) { }
    Counter node_number = 0;

    ExpknapOutput& algorithm_end(Info& info)
    {
        PUT(info, "Algorithm", "NodeNumber", node_number);
        Output::algorithm_end(info);
        VER(info, "Node number: " << node_number << std::endl);
        return *this;
    }
};

ExpknapOutput expknap(Instance& instance, ExpknapOptionalParameters p = {});

}

