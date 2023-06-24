#include "multiplechoicesubsetsumsolver/instance.hpp"

using namespace multiplechoicesubsetsumsolver;

Instance::Instance(
        std::string instance_path,
        std::string format)
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

void Instance::read_standard(std::ifstream& file)
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

void Instance::add_item(
        GroupId group_id,
        Weight weight)
{
    ItemId item_id = items_.size();
    Item item;
    item.group_id = group_id;
    item.weight = weight;
    items_.push_back(item);
    while ((GroupId)groups_.size() <= group_id)
        groups_.push_back(Group());
    groups_[group_id].item_ids.push_back(item_id);
}

std::ostream& Instance::print(
        std::ostream& os,
        int verbose) const
{
    if (verbose >= 1) {
        os
            << "Number of groups:  " << number_of_groups() << std::endl
            << "Number of items:   " << number_of_items() << std::endl
            << "Capacity:          " << capacity() << std::endl
            ;
    }

    if (verbose >= 2) {
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

    return os;
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

void multiplechoicesubsetsumsolver::init_display(
        const Instance& instance,
        optimizationtools::Info& info)
{
    info.os()
            << "=========================================" << std::endl
            << "    Multiple-Choice Subset Sum Solver    " << std::endl
            << "=========================================" << std::endl
            << std::endl
            << "Instance" << std::endl
            << "--------" << std::endl;
    instance.print(info.os(), info.verbosity_level());
    info.os() << std::endl;
}
