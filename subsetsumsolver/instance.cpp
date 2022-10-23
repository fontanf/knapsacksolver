#include "subsetsumsolver/instance.hpp"

using namespace subsetsumsolver;

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
    ItemId n;
    Weight w;
    file >> n >> w;
    set_capacity(w);
    for (ItemPos j = 0; j < n; ++j) {
        file >> w;
        add_item(w);
    }
}

std::ostream& Instance::print(
        std::ostream& os,
        int verbose) const
{
    if (verbose >= 1) {
        os
            << "Number of items:          " << number_of_items() << std::endl
            << "Capacity:                 " << capacity() << std::endl
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
        for (ItemId j = 0; j < number_of_items(); ++j) {
            os
                << std::setw(12) << j
                << std::setw(12) << weight(j)
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
    for (ItemId j = 0; j < number_of_items(); ++j)
        file << weight(j) << std::endl;
}

void subsetsumsolver::init_display(
        const Instance& instance,
        optimizationtools::Info& info)
{
    info.os()
            << "=============================" << std::endl
            << "      Subset Sum Solver      " << std::endl
            << "=============================" << std::endl
            << std::endl
            << "Instance" << std::endl
            << "--------" << std::endl;
    instance.print(info.os(), info.verbosity_level());
    info.os() << std::endl;
}
