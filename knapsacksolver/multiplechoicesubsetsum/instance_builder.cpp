#include "knapsacksolver/multiplechoicesubsetsum/instance_builder.hpp"

#include <iomanip>

using namespace knapsacksolver::multiplechoicesubsetsum;

void InstanceBuilder::add_item(
        GroupId group_id,
        Weight weight)
{
    ItemId item_id = instance_.items_.size();
    Item item;
    item.group_id = group_id;
    item.weight = weight;
    instance_.items_.push_back(item);
    while ((GroupId)instance_.groups_.size() <= group_id)
        instance_.groups_.push_back(Group());
    instance_.groups_[group_id].item_ids.push_back(item_id);
}

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
    GroupId number_of_groups;
    Weight weight;
    ItemPos group_number_of_items;

    file >> number_of_groups >> weight;
    set_capacity(weight);
    for (GroupId group_id = 0; group_id < number_of_groups; ++group_id) {
        file >> group_number_of_items;
        for (ItemPos item_id = 0;
                item_id < group_number_of_items;
                ++item_id) {
            file >> weight;
            add_item(group_id, weight);
        }
    }
}

Instance InstanceBuilder::build()
{
    return std::move(instance_);
}
