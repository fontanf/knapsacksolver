#pragma once

#include "knapsacksolver/knapsack/solution.hpp"

namespace knapsacksolver
{
namespace knapsack
{

Profit upper_bound_dantzig(
        const Instance& instance,
        Info info = Info());

}
}

