#pragma once

#include "knapsacksolver/solution.hpp"

namespace knapsacksolver
{

void solve_surrogate_relaxation(
        Instance instance,
        Output& output,
        std::function<Output (Instance&, Info, bool*)> func,
        bool* end,
        Info info = Info());

Output surrogate_relaxation(
        const Instance& instance,
        Info info = Info());

Output solve_surrogate_instance(
        const Instance& instance,
        Info info = Info());

}

