#pragma once

#include "knapsacksolver/subsetsum/solution.hpp"

namespace knapsacksolver
{
namespace subsetsum
{

Output dynamic_programming_bellman_array(
        const Instance& instance,
        optimizationtools::Info info = optimizationtools::Info());

Output dynamic_programming_bellman_list(
        const Instance& instance,
        optimizationtools::Info info = optimizationtools::Info());

Output dynamic_programming_bellman_word_ram(
        const Instance& instance,
        optimizationtools::Info info = optimizationtools::Info());

Output dynamic_programming_bellman_word_ram_rec(
        const Instance& instance,
        optimizationtools::Info info = optimizationtools::Info());

}
}

