#include "knapsacksolver/subsetsum/instance.hpp"

using namespace knapsacksolver::subsetsum;

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
    ItemId number_of_items;
    Weight weight;
    file >> number_of_items >> weight;
    set_capacity(weight);
    for (ItemPos item_id = 0; item_id < number_of_items; ++item_id) {
        file >> weight;
        add_item(weight);
    }
}

std::ostream& Instance::print(
        std::ostream& os,
        int verbose) const
{
    if (verbose >= 1) {
        os
            << "Number of items:  " << number_of_items() << std::endl
            << "Capacity:         " << capacity() << std::endl
            ;
    }

    if (verbose >= 2) {
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

    file << number_of_items() << " " << capacity() << std::endl;
    for (ItemId item_id = 0; item_id < number_of_items(); ++item_id)
        file << weight(item_id) << std::endl;
}

void knapsacksolver::subsetsum::init_display(
        const Instance& instance,
        optimizationtools::Info& info)
{
    info.os()
            << "====================================" << std::endl
            << "           KnapsackSolver           " << std::endl
            << "====================================" << std::endl
            << std::endl
            << "Problem" << std::endl
            << "--------" << std::endl
            << "Subset sum problem" << std::endl
            << std::endl
            << "Instance" << std::endl
            << "--------" << std::endl;
    instance.print(info.os(), info.verbosity_level());
    info.os() << std::endl;
}
