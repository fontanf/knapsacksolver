#pragma once

#include "knapsacksolver/instance.hpp"
#include "knapsacksolver/solution.hpp"

namespace knapsacksolver
{

Profit ub_dantzig(const Instance& ins, Info info = Info());

}

