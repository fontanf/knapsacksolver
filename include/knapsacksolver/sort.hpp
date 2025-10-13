#pragma once

#include "knapsacksolver/solution.hpp"

namespace knapsacksolver
{

class FullSort
{

public:

    /** Constructor. */
    FullSort(const Instance& instance);

    /** Get break solution. */
    const Solution& break_solution() const { return break_solution_; }

    /** Get an item id. */
    ItemId item_id(ItemPos item_pos) const { return sorted_items_[item_pos]; }

    /** Get the break item id. */
    ItemId break_item_id() const { return break_item_id_; }

    /** Get the position of the break item in the sorted items. */
    ItemId break_item_pos() const { return break_item_pos_; }

private:

    /** Instance. */
    const Instance* instance_;

    /** Break solution. */
    Solution break_solution_;

    /** Sorted items. */
    std::vector<ItemId> sorted_items_;

    /** Break item. */
    ItemId break_item_id_ = -1;

    /** Position of the break item in the sorted items. */
    ItemPos break_item_pos_ = -1;

};

class PartialSort
{

public:

    /** Constructor. */
    PartialSort(const Instance& instance);

    /** Get instance. */
    const Instance& instance() const { return *instance_; }

    /** Get break solution. */
    const Solution& break_solution() const { return break_solution_; }

    /** Get mandatory items. */
    const Solution& mandatory_items() const { return mandatory_items_; }

    /** Get an item id. */
    ItemId item_id(ItemPos item_pos) const { return sorted_items_[item_pos]; }

    /** Get the break item id. */
    ItemId break_item_id() const { return break_item_id_; }

    /** Get the position of the break item in the sorted items. */
    ItemId break_item_pos() const { return break_item_pos_; }

    /**
     * Get the position of the first item in the sorted items.
     *
     * Once the full sort is completed, items before this position belong to
     * the optimal solution.
     */
    ItemId first_sorted_item_pos() const { return first_sorted_item_pos_; }

    /**
     * Get the position of the last item in the sorted items.
     *
     * Once the full sort is completed, items after this position don't belong
     * to the optimal solution.
     */
    ItemId last_sorted_item_pos() const { return last_sorted_item_pos_; }

    /* Get the position of the first item in the left reduced items. */
    ItemPos first_reduced_item_pos() const { return (intervals_left_.empty())? 0: intervals_left_.back().last + 1; }

    /* Get the position of the last item in the right reduced items. */
    ItemPos last_reduced_item_pos() const { return (intervals_right_.empty())? instance().number_of_items() - 1: intervals_right_.back().first - 1; }

    /** Return 'true' iff the left intervals stacks is empty. */
    bool is_intervals_left_empty() const { return intervals_left_.empty(); }

    /** Return 'true' iff the right intervals stacks is empty. */
    bool is_intervals_right_empty() const { return intervals_right_.empty(); }

    ItemId bound_item_left(
            ItemPos item_pos,
            Profit lower_bound);

    ItemId bound_item_right(
            ItemPos item_pos,
            Profit lower_bound);

    void move_item_to_core(
            ItemPos item_pos,
            ItemPos new_item_pos);

private:

    struct Interval
    {
        ItemPos first;
        ItemPos last;
    };

    /*
     * Private methods
     */

    std::pair<ItemPos, ItemPos> partition(
            ItemPos f,
            ItemPos l);

    /** Compute break solution. */
    void compute_break_solution();

    /** Sort the next left interval. */
    void sort_next_left_interval(Profit lower_bound);

    /** Sort the next right interval. */
    void sort_next_right_interval(Profit lower_bound);

    /** Check the partial sort. */
    bool check() const;

    /** Print the partial sort structures. */
    void format(std::ostream& os) const;

    /*
     * Private attributes
     */

    /** Instance. */
    const Instance* instance_;

    /** Break solution. */
    Solution break_solution_;

    /** Mandatory items. */
    Solution mandatory_items_;

    /** Break item. */
    ItemId break_item_id_;

    /** Position of the break item in the sorted items. */
    ItemPos break_item_pos_ = -1;

    /** Sorted items. */
    std::vector<ItemId> sorted_items_;

    /** Stack of unsorted intervals on the left. */
    std::vector<Interval> intervals_left_;

    /** Stack of unsorted intervals on the right. */
    std::vector<Interval> intervals_right_;

    /** Position of the first sorted item. */
    ItemPos first_sorted_item_pos_ = -1;

    /** Position of the last sorted item. */
    ItemPos last_sorted_item_pos_ = -1;

    /** Position of the first item of the initial core. */
    ItemPos initial_core_first_item_pos_ = -1;

    /** Position of the last item of the initial core. */
    ItemPos initial_core_last_item_pos_ = -1;

};

}
