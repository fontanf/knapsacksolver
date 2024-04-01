#pragma once

#include "optimizationtools/utils/output.hpp"

namespace knapsacksolver
{
namespace multiple_choice_subset_sum
{

using Weight = int64_t;
using ItemId = int64_t;
using ItemPos = int64_t;
using GroupId = int64_t;
using StateId = int64_t;
using Counter = int64_t;
using Seed = int64_t;

/**
 * Structure for a group of items.
 */
struct Group
{
    /** Items. */
    std::vector<ItemId> item_ids;
};

/**
 * Structure for an item.
 */
struct Item
{
    /** Group of the item. */
    GroupId group_id;

    /** Weight of the item. */
    Weight weight;
};

/**
 * Instance group for a multiple-choice subset sum problem.
 */
class Instance
{

public:

    /*
     * Getters
     */

    /** Get the number of groups in the instance. */
    inline GroupId number_of_groups() const { return groups_.size(); }

    /** Get the number of items in the instance. */
    inline ItemPos number_of_items() const { return items_.size(); }

    /** Get the capacity of the instance. */
    inline Weight capacity() const { return capacity_; }

    /** Get an item. */
    inline const Item& item(ItemId item_id) const { return items_[item_id]; }

    /** Get a group. */
    inline const Group& group(GroupId group_id) const { return groups_[group_id]; }

    /** Get the number of items of a group. */
    inline ItemPos number_of_items(GroupId group_id) const { return groups_[group_id].item_ids.size(); }

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

    /** Group. */
    std::vector<Group> groups_;

    /** Capacity of the knapsack. */
    Weight capacity_;

    friend class InstanceBuilder;

};

}
}
