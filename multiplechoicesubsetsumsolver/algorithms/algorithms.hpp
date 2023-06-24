#pragma once

#include "multiplechoicesubsetsumsolver/algorithms/dynamic_programming_bellman.hpp"

#include <random>

namespace multiplechoicesubsetsumsolver
{

Output run(
        std::string algorithm,
        Instance& instance,
        std::mt19937_64& generator,
        optimizationtools::Info info);

}

