#pragma once

#include "knapsacksolver/subsetsum/solution.hpp"

namespace knapsacksolver
{
namespace subsetsum
{

Output dynamic_programming_balancing_array(
        const Instance& instance,
        optimizationtools::Info info = optimizationtools::Info());

}
}

