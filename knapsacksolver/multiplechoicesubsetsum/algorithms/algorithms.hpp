#pragma once

#include "knapsacksolver/multiplechoicesubsetsum/algorithms/dynamic_programming_bellman.hpp"

#include <random>

namespace knapsacksolver
{
namespace multiplechoicesubsetsum
{

Output run(
        std::string algorithm,
        Instance& instance,
        std::mt19937_64& generator,
        optimizationtools::Info info);

}
}

