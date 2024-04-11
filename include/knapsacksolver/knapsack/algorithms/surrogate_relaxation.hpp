#pragma once

#include "knapsacksolver/knapsack/solution.hpp"

namespace knapsacksolver
{
namespace knapsack
{

using SolveCallback = std::function<Output(const Instance&)>;

struct SurrogateRelaxationParameters: Parameters
{
    SolveCallback solve_callback = [](const Instance& instance) { return Output(instance); };
};

const Output surrogate_relaxation(
        const Instance& instance,
        const SurrogateRelaxationParameters& parameters = {});

}
}
