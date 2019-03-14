#pragma once

#include "knapsack/lib/instance.hpp"
#include "knapsack/lib/solution.hpp"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

namespace knapsack
{

struct SurrogateOut
{
    Profit ub = 0;
    ItemIdx bound = -1;
    Weight multiplier = 0;
};

SurrogateOut ub_surrogate(const Instance& instance, Profit lb, Info& info);

}

