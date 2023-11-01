#pragma once

#include "knapsacksolver/subsetsum/algorithms/dynamic_programming_bellman.hpp"
#include "knapsacksolver/subsetsum/algorithms/dynamic_programming_balancing.hpp"

#include <random>

namespace knapsacksolver
{
namespace subsetsum
{

Output run(
        std::string algorithm,
        Instance& instance,
        std::mt19937_64& generator,
        optimizationtools::Info info);

}
}

