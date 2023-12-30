#include "knapsacksolver/knapsack/instance.hpp"

#include <iomanip>

using namespace knapsacksolver::knapsack;

void Instance::format(
        std::ostream& os,
        int verbosity_level) const
{
    if (verbosity_level >= 1) {
        os
            << "Number of items:      " << number_of_items() << std::endl
            << "Capacity:             " << capacity() << std::endl
            << "Highest item profit:  " << highest_item_profit() << std::endl
            << "Highest item weight:  " << highest_item_weight() << std::endl
            << "Total item profit:    " << total_item_profit() << std::endl
            << "Total item weight:    " << total_item_weight() << std::endl
            << "Weight ratio:         " << (double)total_item_weight() / capacity() << std::endl
            ;
    }

    if (verbosity_level >= 2) {
        os
            << std::endl
            << std::setw(12) << "Item"
            << std::setw(12) << "Weight"
            << std::setw(12) << "Profit"
            << std::setw(12) << "Eff."
            << std::endl
            << std::setw(12) << "----"
            << std::setw(12) << "------"
            << std::setw(12) << "------"
            << std::setw(12) << "----"
            << std::endl;
        for (ItemId item_id = 0; item_id < number_of_items(); ++item_id) {
            const Item& item = this->item(item_id);
            os
                << std::setw(12) << item_id
                << std::setw(12) << item.weight
                << std::setw(12) << item.profit
                << std::setw(12) << item.efficiency
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
    for (ItemId item_id = 0; item_id < number_of_items(); ++item_id) {
        file
            << item(item_id).weight
            << item(item_id).profit
            << std::endl;
    }
}
