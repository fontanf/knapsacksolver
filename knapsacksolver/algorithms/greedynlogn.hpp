#pragma once

#include "knapsacksolver/solution.hpp"

namespace knapsacksolver
{

Output forwardgreedynlogn(const Instance& ins, Info info = Info());
Output backwardgreedynlogn(const Instance& ins, Info info = Info());
Output greedynlogn(const Instance& ins, Info info = Info());

}

