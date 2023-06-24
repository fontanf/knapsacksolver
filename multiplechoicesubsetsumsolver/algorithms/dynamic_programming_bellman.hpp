#pragma once

#include "multiplechoicesubsetsumsolver/solution.hpp"

namespace multiplechoicesubsetsumsolver
{

Output dynamic_programming_bellman_array(
        const Instance& instance,
        optimizationtools::Info info = optimizationtools::Info());

Output dynamic_programming_bellman_word_ram(
        const Instance& instance,
        optimizationtools::Info info = optimizationtools::Info());

Output dynamic_programming_bellman_word_ram_rec(
        const Instance& instance,
        optimizationtools::Info info = optimizationtools::Info());

}

