#pragma once

#include "knapsacksolver/knapsack/solution.hpp"

namespace knapsacksolver
{
namespace knapsack
{

Output dynamic_programming_profits_array(
        const Instance& instance,
        Info info = Info());

Output dynamic_programming_profits_array_all(
        const Instance& instance,
        Info info = Info());

}
}

