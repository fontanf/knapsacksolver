#include "knapsacksolver/subsetsum/instance_builder.hpp"

#include <iomanip>

using namespace knapsacksolver::subsetsum;

void InstanceBuilder::read(
        const std::string& instance_path,
        const std::string& format)
{
    std::ifstream file(instance_path);
    if (!file.good()) {
        throw std::runtime_error(
                "Unable to open file \"" + instance_path + "\".");
    }

    if (format == "standard" || format == "") {
        read_standard(file);
    } else {
        throw std::invalid_argument(
                "Unknown instance format \"" + format + "\".");
    }

    file.close();
}

void InstanceBuilder::read_standard(std::ifstream& file)
{
    ItemId number_of_items;
    Weight weight;
    file >> number_of_items >> weight;
    set_capacity(weight);
    for (ItemPos item_id = 0; item_id < number_of_items; ++item_id) {
        file >> weight;
        add_item(weight);
    }
}

Instance InstanceBuilder::build()
{
    return std::move(instance_);
}
