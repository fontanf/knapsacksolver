#pragma once

#include "knapsacksolver/solution.hpp"

namespace knapsacksolver
{

Output branch_and_bound(
        Instance& instance,
        bool sort = false,
        Info info = Info());

}

