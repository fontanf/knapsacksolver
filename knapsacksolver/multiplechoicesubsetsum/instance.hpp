#pragma once

#include "optimizationtools/utils/info.hpp"

namespace knapsacksolver
{
namespace multiplechoicesubsetsum
{

typedef int64_t Weight;
typedef int64_t ItemId;
typedef int64_t ItemPos;
typedef int64_t GroupId;
typedef int64_t StateId;
typedef int64_t Counter;

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
     * Constructors and destructor
     */

    /** Create instance from file. */
    Instance(
            std::string instance_path,
            std::string format);

    /** Manual constructor. */
    Instance() { }

    /** Add an item to the knapsack. */
    void add_item(
            GroupId group_id,
            Weight weight);

    /** Set the capacity of the knapsack. */
    void set_capacity(Weight capacity) { capacity_ = capacity; }

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
    std::ostream& print(
            std::ostream& os,
            int verbose = 1) const;

    /** Write the instance to a file. */
    void write(std::string instance_path) const;

private:

    /*
     * Private methods.
     */

    void read_standard(std::ifstream& file);

    /*
     * Private attributes
     */

    /** Items. */
    std::vector<Item> items_;

    /** Group. */
    std::vector<Group> groups_;

    /** Capacity of the knapsack. */
    Weight capacity_;

};

void init_display(
        const Instance& instance,
        optimizationtools::Info& info);

}
}

