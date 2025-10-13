#pragma once

#include "knapsacksolver/solution.hpp"

namespace knapsacksolver
{

using SolveCallback = std::function<Output(const Instance&)>;

struct SurrogateRelaxationParameters: Parameters
{
    SolveCallback solve_callback = [](const Instance& instance) { return Output(instance); };
};

Output surrogate_relaxation(
        const Instance& instance,
        const SurrogateRelaxationParameters& parameters = {});

}
