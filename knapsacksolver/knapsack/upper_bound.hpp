#pragma once

#include "knapsacksolver/knapsack/solution.hpp"
#include "knapsacksolver/knapsack/sort.hpp"

namespace knapsacksolver
{
namespace knapsack
{

/**
 * Value of the solution if all the remaining capacity is filled with items
 * with the efficiency of 'item_id'.
 *
 * 'current_weight' must be smaller than or equal to 'instance.capacity()'.
 */
inline Profit upper_bound(
        const Instance& instance,
        Profit current_profit,
        Weight current_weight,
        ItemId item_id)
{
    if (item_id == -1)
        return current_profit;
    const Item& item = instance.item(item_id);
    return current_profit + ((instance.capacity() - current_weight) * item.profit) / item.weight;
}

/**
 * Value of the solution if all the overcapacity is fixed by removing items
 * with the efficiency of 'item_id'.
 *
 * 'current_weight' must be strictly greater than 'instance.capacity()'.
 */
inline Profit upper_bound_reverse(
        const Instance& instance,
        Profit current_profit,
        Weight current_weight,
        ItemPos item_id)
{
    if (item_id == -1)
        return -1;
    const Item& item = instance.item(item_id);
    return current_profit + ((instance.capacity() - current_weight) * item.profit + 1) / item.weight - 1;
}

}
}
