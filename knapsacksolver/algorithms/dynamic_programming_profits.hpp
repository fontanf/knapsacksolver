#pragma once

#include "knapsacksolver/solution.hpp"

namespace knapsacksolver
{

Output dynamic_programming_profits_array(
        const Instance& instance,
        Info info = Info());

Output dynamic_programming_profits_array_all(
        const Instance& instance,
        Info info = Info());

}

