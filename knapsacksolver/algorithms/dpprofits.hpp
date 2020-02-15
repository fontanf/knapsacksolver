#pragma once

#include "knapsacksolver/instance.hpp"
#include "knapsacksolver/solution.hpp"

namespace knapsacksolver
{

Output dpprofits_array(const Instance& ins, Info info = Info());
Output dpprofits_array_all(const Instance& ins, Info info = Info());

}

