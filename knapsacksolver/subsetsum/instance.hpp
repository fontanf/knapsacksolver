#pragma once

#include "optimizationtools/utils/output.hpp"

namespace knapsacksolver
{
namespace subsetsum
{

using Weight = int64_t;
using ItemId = int64_t;
using ItemPos = int64_t;
using StateId = int64_t;
using Counter = int64_t;
using Seed = int64_t;

/**
 * Instance class for a subset sum problem.
 */
class Instance
{

public:

    /*
     * Getters
     */

    /** Get the number of items in the instance. */
    inline ItemPos number_of_items() const { return weights_.size(); }

    /** Get the capacity of the instance. */
    inline Weight capacity() const { return capacity_; }

    /** Get the weight of an item. */
    inline Weight weight(ItemId item_id) const { return weights_[item_id]; }

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

    /** Weights. */
    std::vector<Weight> weights_;

    /** Capacity of the knapsack. */
    Weight capacity_;

    friend class InstanceBuilder;

};

}
}
