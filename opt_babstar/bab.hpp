#pragma once

#include "knapsack/lib/instance.hpp"
#include "knapsack/lib/solution.hpp"

namespace knapsack
{

Solution sopt_babstar(Instance& ins, Info& info);

Solution sopt_babstar_dp(Instance& ins, Info& info);

}

