#pragma once

#include "knapsacksolver/solution.hpp"

namespace knapsacksolver
{

void solvesurrelax(
        Instance instance,
        Output& output,
        std::function<Output (Instance&, Info, bool*)> func,
        bool* end,
        Info info = Info());

Output surrelax(
        const Instance& instance,
        Info info = Info());

Output surrelax_minknap(
        const Instance& instance,
        Info info = Info());

}

