#pragma once

#include "optimizationtools/utils/info.hpp"

namespace subsetsumsolver
{

using optimizationtools::Info;

typedef int64_t Weight;
typedef int64_t ItemId;
typedef int64_t ItemPos;
typedef int64_t StateId;
typedef int64_t Counter;

/**
 * Instance class for a Subset Sum Problem.
 */
class Instance
{

public:

    /*
     * Constructors and destructor
     */

    /** Create instance from file. */
    Instance(std::string instance_path, std::string format);

    /** Manual constructor. */
    Instance() { }

    /** Add an item to the knapsack. */
    void add_item(Weight weight) { weights_.push_back(weight); };

    /** Set the capacity of the knapsack. */
    void set_capacity(Weight capacity) { capacity_ = capacity; }

    /*
     * Getters
     */

    /** Get the number of items in the instance. */
    inline ItemPos number_of_items() const { return weights_.size(); }

    /** Get the capacity of the instance. */
    inline Weight capacity() const { return capacity_; }

    /** Get the weight of an item. */
    inline Weight weight(ItemId j) const { return weights_[j]; }

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

    /** Weights. */
    std::vector<Weight> weights_;

    /** Capacity of the knapsack. */
    Weight capacity_;

};

void init_display(
        const Instance& instance,
        optimizationtools::Info& info);

}

