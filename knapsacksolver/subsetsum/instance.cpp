#include "knapsacksolver/subsetsum/instance.hpp"

#include <iomanip>

using namespace knapsacksolver::subsetsum;

void Instance::format(
        std::ostream& os,
        int verbosity_level) const
{
    if (verbosity_level >= 1) {
        os
            << "Number of items:  " << number_of_items() << std::endl
            << "Capacity:         " << capacity() << std::endl
            ;
    }

    if (verbosity_level >= 2) {
        os
            << std::endl
            << std::setw(12) << "Item"
            << std::setw(12) << "Weight"
            << std::endl
            << std::setw(12) << "----"
            << std::setw(12) << "------"
            << std::endl;
        for (ItemId item_id = 0; item_id < number_of_items(); ++item_id) {
            os
                << std::setw(12) << item_id
                << std::setw(12) << weight(item_id)
                << std::endl;
        }
    }
}

void Instance::write(
        std::string instance_path) const
{
    std::ofstream file(instance_path);
    if (!file.good()) {
        throw std::runtime_error(
                "Unable to open file \"" + instance_path + "\".");
    }

    file << number_of_items() << " " << capacity() << std::endl;
    for (ItemId item_id = 0; item_id < number_of_items(); ++item_id)
        file << weight(item_id) << std::endl;
}
