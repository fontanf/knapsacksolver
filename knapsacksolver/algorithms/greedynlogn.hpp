#pragma once

#include "knapsacksolver/solution.hpp"

namespace knapsacksolver
{

Output forwardgreedynlogn(const Instance& instance, Info info = Info());
Output backwardgreedynlogn(const Instance& instance, Info info = Info());
Output greedynlogn(const Instance& instance, Info info = Info());

}

