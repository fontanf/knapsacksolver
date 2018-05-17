#pragma once

#include "knapsack/lib/instance.hpp"

namespace knapsack
{

Instance generate(std::string type, ItemIdx n, Profit r, int h);

}

