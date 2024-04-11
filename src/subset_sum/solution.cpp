#include "knapsacksolver/subset_sum/solution.hpp"

#include "optimizationtools/utils/utils.hpp"

using namespace knapsacksolver::subset_sum;

Solution::Solution(const Instance& instance):
    instance_(&instance),
    contains_(instance.number_of_items(), 0)
{ }

Solution::Solution(
        const Instance& instance,
        std::string certificate_path):
    Solution(instance)
{
    if (certificate_path.empty())
        return;
    std::ifstream file(certificate_path);
    if (!file.good()) {
        throw std::runtime_error(
                "Unable to open file \"" + certificate_path + "\".");
    }

    ItemPos number_of_items;
    ItemPos item_id;
    file >> number_of_items;
    for (ItemPos pos = 0; pos < number_of_items; ++pos) {
        file >> item_id;
        add(item_id);
    }
}

void Solution::add(ItemId item_id)
{
    contains_[item_id] = 1;
    number_of_items_++;
    weight_ += instance().weight(item_id);
}

void Solution::write(std::string certificate_path) const
{
    if (certificate_path.empty())
        return;
    std::ofstream file(certificate_path);
    if (!file.good()) {
        throw std::runtime_error(
                "Unable to open file \"" + certificate_path + "\".");
    }

    file << number_of_items() << std::endl;
    for (ItemId item_id = 0; item_id < instance().number_of_items(); ++item_id)
        if (contains(item_id))
            file << item_id << std::endl;
}

void Solution::format(
        std::ostream& os,
        int verbosity_level) const
{
    if (verbosity_level >= 1) {
        os
            << "Number of items:  " << optimizationtools::Ratio<ItemId>(number_of_items(), instance().number_of_items()) << std::endl
            << "Weight:           " << optimizationtools::Ratio<ItemId>(weight(), instance().capacity()) << std::endl
            << "Feasible:         " << feasible() << std::endl
            ;
    }

    if (verbosity_level >= 2) {
        os << std::endl
            << std::setw(12) << "Item"
            << std::endl
            << std::setw(12) << "----"
            << std::endl;
        for (ItemId item_id = 0;
                item_id < number_of_items();
                ++item_id) {
            os
                << std::setw(12) << item_id
                << std::endl;
        }
    }
}

nlohmann::json Solution::to_json() const
{
    return nlohmann::json {
        {"NumberOfItems", number_of_items()},
        {"Feasible", feasible()},
        {"Weight", weight()},
    };
}
