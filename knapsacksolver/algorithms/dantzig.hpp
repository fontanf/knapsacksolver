#pragma once

#include "knapsacksolver/solution.hpp"

namespace knapsacksolver
{

Profit ub_dantzig(const Instance& instance, Info info = Info());

}

