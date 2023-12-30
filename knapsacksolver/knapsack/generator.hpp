#pragma once

#include "knapsacksolver/knapsack/instance.hpp"

#include <random>

namespace knapsacksolver
{
namespace knapsack
{

Instance generate_u(
        ItemPos number_of_items,
        Weight maximum_weight,
        Profit maximum_profit,
        double capacity_ratio,
        std::mt19937_64& generator);

}
}
