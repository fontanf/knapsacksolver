#include "knapsacksolver/multiplechoicesubsetsum/instance.hpp"

#include <iomanip>

using namespace knapsacksolver::multiplechoicesubsetsum;

void Instance::format(
        std::ostream& os,
        int verbosity_level) const
{
    if (verbosity_level >= 1) {
        os
            << "Number of groups:  " << number_of_groups() << std::endl
            << "Number of items:   " << number_of_items() << std::endl
            << "Capacity:          " << capacity() << std::endl
            ;
    }

    if (verbosity_level >= 2) {
        os
            << std::endl
            << std::setw(12) << "Group"
            << std::setw(12) << "Size"
            << std::endl
            << std::setw(12) << "-----"
            << std::setw(12) << "----"
            << std::endl;
        for (GroupId group_id = 0; group_id < number_of_groups(); ++group_id) {
            os
                << std::setw(12) << group_id
                << std::setw(12) << number_of_items(group_id)
                << std::endl;
        }

        os
            << std::endl
            << std::setw(12) << "Item"
            << std::setw(12) << "Group"
            << std::setw(12) << "Weight"
            << std::endl
            << std::setw(12) << "----"
            << std::setw(12) << "-----"
            << std::setw(12) << "------"
            << std::endl;
        for (ItemId item_id = 0; item_id < number_of_items(); ++item_id) {
            const Item& item = this->item(item_id);
            os
                << std::setw(12) << item_id
                << std::setw(12) << item.group_id
                << std::setw(12) << item.weight
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

    file << number_of_groups() << " " << capacity() << std::endl;
    for (GroupId group_id = 0; group_id < number_of_groups(); ++group_id) {
        file << number_of_items(group_id);
        for (ItemPos item_id = 0;
                item_id < number_of_items(group_id);
                ++item_id)
            file << " " << item(item_id).weight;
        file << std::endl;
    }
}
