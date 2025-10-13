#pragma once

#include "knapsacksolver/instance.hpp"

namespace knapsacksolver
{

class InstanceBuilder
{

public:

    /** Constructor. */
    InstanceBuilder() { }

    /** Add an item to the knapsack. */
    void add_item(
            Profit profit,
            Weight weight);

    /** Set the capacity of the knapsack. */
    void set_capacity(Weight capacity) { instance_.capacity_ = capacity; }

    /** Read an instance from a file. */
    void read(
            const std::string& instance_path,
            const std::string& format = "standard");

    /*
     * Build
     */

    /** Build. */
    Instance build();

private:

    /*
     * Private methods
     */

    /*
     * Read input file
     */

    /** Read an instance file in 'standard' format. */
    void read_standard(std::ifstream& file);

    /** Read an instance file in 'pisinger' format. */
    void read_pisinger(std::ifstream& file);

    /** Read an instance file in 'jooken' format. */
    void read_jooken(std::ifstream& file);

    /** Read an subset_sum instance file in 'standard' format. */
    void read_subset_sum_standard(std::ifstream& file);

    /*
     * Private attributes
     */

    /** Instance. */
    Instance instance_;

};

class InstanceFromFloatProfitsBuilder
{

public:

    /** Constructor. */
    InstanceFromFloatProfitsBuilder() { }

    /** Add an item to the knapsack. */
    void add_item(
            double profit,
            Weight weight);

    /** Set the capacity of the knapsack. */
    void set_capacity(Weight capacity) { instance_.capacity_ = capacity; }

    /*
     * Build
     */

    /** Build. */
    Instance build();

private:

    /*
     * Private methods
     */

    /*
     * Private attributes
     */

    /** Instance. */
    Instance instance_;

    std::vector<double> profits_double_;

};

}
