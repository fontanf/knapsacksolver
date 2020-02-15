#pragma once

#include "knapsacksolver/instance.hpp"
#include "knapsacksolver/solution.hpp"

namespace knapsacksolver
{

Output branchandbound(Instance& ins, bool sort = false, Info info = Info());

}

