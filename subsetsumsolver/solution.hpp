#pragma once

#include "subsetsumsolver/instance.hpp"

namespace subsetsumsolver
{

/**
 * Solution class for a subset sum problem.
 */
class Solution
{

public:

    /*
     * Constructors and destructor
     */

    /** Create an empty solution. */
    Solution(const Instance& instance);

    /** Create a solution from a file. */
    Solution(
            const Instance& instance,
            std::string certificate_path);

    /** Add an item to the solution. */
    void add(ItemId item_id);

    /*
     * Getters
     */

    /** Get the instance. */
    inline const Instance& instance() const { return *instance_; }

    /** Get the number of items in the solution. */
    inline ItemPos number_of_items() const { return number_of_items_; }

    /** Get the total weight of the solution. */
    inline Weight weight() const { return weight_; }

    /** Return 'true' iff the solution contains item 'j'. */
    int8_t contains(ItemId item_id) const { return contains_[item_id]; }

    /** Return 'true' iff the solution is feasible. */
    bool feasible() const { return weight_ <= instance().capacity(); }

    /**
     * Return 'true' iff the solution is strictly better than the given lower
     * bound.
     */
    bool better(Weight lower_bound) const { return feasible() && weight() > lower_bound; };

    /*
     * Export
     */

    /** Print the instance. */
    std::ostream& print(
            std::ostream& os,
            int verbose = 1) const;

    /** Write the solution to a file. */
    void write(std::string filepath);

private:

    /** Instance. */
    const Instance* instance_;

    /** Number of items in the solution. */
    ItemPos number_of_items_ = 0;

    /** Weight of the solution. */
    Weight weight_ = 0;

    /** 'contains_[j] == true' iff the solution contains item 'j'. */
    std::vector<int8_t> contains_;

};

/** Stream insertion operator. */
std::ostream& operator<<(std::ostream& os, const Solution& solution);

////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////// Output ////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/**
 * Output structure for a subset sum problem.
 */
struct Output
{
    /** Constructor. */
    Output(
            const Instance& instance,
            optimizationtools::Info& info);

    /** Solution. */
    Solution solution;

    /** Lower bound. */
    Weight lower_bound = 0;

    /** Upper bound. */
    Weight upper_bound = -1;

    /** Elapsed time. */
    double time = -1;

    /** Return 'true' iff the solution is optimal. */
    bool optimal() const;

    /** Print current state. */
    void print(
            optimizationtools::Info& info,
            const std::stringstream& s) const;

    /** Update the solution. */
    void update_solution(
            const Solution& solution_new,
            const std::stringstream& s,
            optimizationtools::Info& info);

    /** Update the lower bound. */
    void update_lower_bound(
            Weight lower_bound_new,
            const std::stringstream& s,
            optimizationtools::Info& info);

    /** Update the upper bound. */
    void update_upper_bound(
            Weight upper_bound_new,
            const std::stringstream& s,
            optimizationtools::Info& info);

    /** Print the algorithm statistics. */
    virtual void print_statistics(
            optimizationtools::Info& info) const { (void)info; }

    /** Method to call at the end of the algorithm. */
    Output& algorithm_end(
            optimizationtools::Info& info);
};

}

