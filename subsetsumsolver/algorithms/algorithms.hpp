#pragma once

#include "subsetsumsolver/algorithms/dynamic_programming_bellman.hpp"
#include "subsetsumsolver/algorithms/dynamic_programming_balancing.hpp"

#include <random>

namespace subsetsumsolver
{

Output run(
        std::string algorithm,
        Instance& instance,
        std::mt19937_64& generator,
        Info info);

}

