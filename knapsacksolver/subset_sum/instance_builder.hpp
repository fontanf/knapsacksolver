#pragma once

#include "knapsacksolver/subset_sum/instance.hpp"

namespace knapsacksolver
{
namespace subset_sum
{

class InstanceBuilder
{

public:

    /** Constructor. */
    InstanceBuilder() { }

    /** Add an item to the knapsack. */
    void add_item(Weight weight) { instance_.weights_.push_back(weight); };

    /** Set the capacity of the knapsack. */
    void set_capacity(Weight capacity) { instance_.capacity_ = capacity; }

    /** Read an instance from a file. */
    void read(
            const std::string& instance_path,
            const std::string& format);

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

    /*
     * Private attributes
     */

    /** Instance. */
    Instance instance_;

};

}
}
