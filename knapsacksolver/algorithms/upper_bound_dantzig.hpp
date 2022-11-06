#pragma once

#include "knapsacksolver/solution.hpp"

namespace knapsacksolver
{

Profit upper_bound_dantzig(
        const Instance& instance,
        Info info = Info());

}

