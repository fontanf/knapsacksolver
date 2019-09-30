#pragma once

#include "knapsack/lib/instance.hpp"
#include "knapsack/lib/solution.hpp"

namespace knapsack
{

knapsack::Output sopt_bab(Instance& ins, bool sort = false, Info info = Info());

}

