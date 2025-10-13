#pragma once

#include <cstdint>
#include <vector>
#include <iostream>

namespace knapsacksolver
{

using Weight = int64_t;
using Profit = int64_t;
using ItemId = int64_t;
using ItemPos = int64_t;
using StateId = int64_t;
using Counter = int64_t;
using Seed = int64_t;

/**
 * Structure for an item.
 */
struct Item
{
    /** Weight of the item. */
    Weight weight = 0;

    /** Profit of the item. */
    Profit profit = 0;

    /*
     * Computed attributes
     */

    /** Efficiency of the item. */
    double efficiency = 0.0;
};

/**
 * Instance class for a knapsack problem.
 */
class Instance
{

public:

    /*
     * Getters
     */

    /** Get the number of items in the instance. */
    inline ItemPos number_of_items() const { return items_.size(); }

    /** Get the capacity of the instance. */
    inline Weight capacity() const { return capacity_; }

    /** Get an item. */
    inline const Item& item(ItemId item_id) const { return items_[item_id]; }

    /** Get the highest item profit. */
    inline Profit highest_item_profit() const { return highest_item_profit_; }

    /** Get the highest item weight. */
    inline Weight highest_item_weight() const { return highest_item_weight_; }

    /** Get the total item profit. */
    inline Profit total_item_profit() const { return total_item_profit_; }

    /** Get the total item weight. */
    inline Weight total_item_weight() const { return total_item_weight_; }

    /** Get the item with the highest efficiency. */
    inline ItemId highest_efficiency_item_id() const { return highest_efficiency_item_id_; }

    /** Get the items sorted by decreasing efficiency. */
    inline std::vector<ItemId> compute_sorted_items() const;

    /*
     * Export.
     */

    /** Print the instance into a stream. */
    void format(
            std::ostream& os,
            int verbosity_level = 1) const;

    /** Write the instance to a file. */
    void write(std::string instance_path) const;

private:

    /*
     * Private methods.
     */

    /** Manual constructor. */
    Instance() { }

    /*
     * Private attributes
     */

    /** Items. */
    std::vector<Item> items_;

    /** Capacity of the knapsack. */
    Weight capacity_;

    /*
     * Commputed attributes
     */

    /** Total item profit. */
    Profit highest_item_profit_ = 0;

    /** Total item weight. */
    Weight highest_item_weight_ = 0;

    /** Total item profit. */
    Profit total_item_profit_ = 0;

    /** Total item weight. */
    Weight total_item_weight_ = 0;

    /** Item with the highest efficiency. */
    ItemId highest_efficiency_item_id_;

    friend class InstanceBuilder;
    friend class InstanceFromFloatProfitsBuilder;

};

}
