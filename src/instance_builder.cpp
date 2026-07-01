#include "knapsacksolver/instance_builder.hpp"

#include "optimizationtools/utils/utils.hpp"

#include <fstream>

using namespace knapsacksolver;

void InstanceBuilder::add_item(
        Profit profit,
        Weight weight)
{
    Item item;
    item.profit = profit;
    item.weight = weight;
    instance_.items_.push_back(item);
};

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
    } else if (format == "pisinger") {
        read_pisinger(file);
    } else if (format == "jooken") {
        read_jooken(file);
    } else if (format == "subset_sum_standard") {
        read_subset_sum_standard(file);
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
    Profit profit;
    file >> number_of_items >> weight;
    set_capacity(weight);
    for (ItemPos item_id = 0; item_id < number_of_items; ++item_id) {
        file >> profit >> weight;
        add_item(profit, weight);
    }
}

void InstanceBuilder::read_pisinger(std::ifstream& file)
{
    ItemId number_of_items;
    Weight capacity;
    std::string tmp;
    file >> tmp >> tmp >> number_of_items >> tmp >> capacity >> tmp >> tmp >> tmp >> tmp;
    set_capacity(capacity);

    getline(file, tmp);
    for (ItemPos item_id = 0; item_id < number_of_items; ++item_id) {
        getline(file, tmp);
        std::vector<std::string> line = optimizationtools::split(tmp, ',');
        Profit profit = std::stol(line[1]);
        Weight weight = std::stol(line[2]);
        add_item(profit, weight);
    }
}

void InstanceBuilder::read_jooken(std::ifstream& file)
{
    ItemId number_of_items;
    file >> number_of_items;

    ItemId tmp;
    Weight weight;
    Profit profit;
    for (ItemPos item_id = 0; item_id < number_of_items; ++item_id) {
        file >> tmp >> profit >> weight;
        add_item(profit, weight);
    }

    Weight capacity;
    file >> capacity;
    set_capacity(capacity);
}

void InstanceBuilder::read_subset_sum_standard(std::ifstream& file)
{
    ItemId number_of_items;
    Weight capacity;
    file >> number_of_items >> capacity;

    set_capacity(capacity);

    Weight weight = -1;
    for (ItemId item_id = 0; item_id < number_of_items; ++item_id) {
        file >> weight;
        add_item(weight, weight);
    }
}

Instance InstanceBuilder::build()
{
    // Check knapsack capacity.
    if (instance_.capacity() < 0) {
        throw std::invalid_argument(
                "The knapsack capacity must be positive.");
    }

    // Check that profit are positive and weights are positive and smaller than
    // the capacity. Also compute the highest item weight, needed below.
    for (ItemId item_id = 0;
            item_id < instance_.number_of_items();
            ++item_id) {
        const Item& item = instance_.items_[item_id];
        if (item.profit <= 0) {
            throw std::invalid_argument(
                    "Items must have strictly positive profits.");
        }
        if (item.weight <= 0) {
            throw std::invalid_argument(
                    "Items must have strictly positive weights.");
        }
        if (item.weight > instance_.capacity()) {
            throw std::invalid_argument(
                    "The weight of an item must be smaller than the knapsack capacity..");
        }
        instance_.highest_item_weight_ = std::max(instance_.highest_item_weight_, item.weight);
    }

    // Check no overflow because of the bounds computed by 'upper_bound' and
    // 'upper_bound_reverse', which multiply an item's profit by a weight gap
    // that can be as large as 'capacity + 2 * highest_item_weight' (the
    // weight of a state can exceed the capacity by up to two item weights,
    // see PartialSort::sort_next_right_interval).
    Weight bound_weight = instance_.capacity();
    if (instance_.highest_item_weight_
            > (std::numeric_limits<Weight>::max() - bound_weight) / 2) {
        throw std::overflow_error(
                "Overflow while computing the profit/weight bound.");
    }
    bound_weight += 2 * instance_.highest_item_weight_;
    for (ItemId item_id = 0;
            item_id < instance_.number_of_items();
            ++item_id) {
        const Item& item = instance_.items_[item_id];
        if (bound_weight > 0
                && item.profit > std::numeric_limits<Profit>::max() / bound_weight) {
            throw std::overflow_error(
                    "Overflow: 'item.profit * (capacity + 2 * highest_item_weight)' "
                    "must not exceed the maximum profit value.");
        }
    }

    // Compute total item profit and weight.
    for (ItemId item_id = 0;
            item_id < instance_.number_of_items();
            ++item_id) {
        const Item& item = instance_.items_[item_id];
        instance_.highest_item_profit_ = std::max(instance_.highest_item_profit_, item.profit);
        if (item.profit > std::numeric_limits<Profit>::max() - instance_.total_item_profit_) {
            throw std::overflow_error(
                    "Overflow while computing the total item profit.");
        }
        instance_.total_item_profit_ += item.profit;
        if (item.weight > std::numeric_limits<Weight>::max() - instance_.total_item_weight_) {
            throw std::overflow_error(
                    "Overflow while computing the total item weight.");
        }
        instance_.total_item_weight_ += item.weight;
    }

    // Compute item efficiencies.
    for (ItemId item_id = 0;
            item_id < instance_.number_of_items();
            ++item_id) {
        Item& item = instance_.items_[item_id];
        item.efficiency = (double)item.profit / item.weight;
    }

    // Compute highest efficiency item.
    instance_.highest_efficiency_item_id_ = -1;
    for (ItemId item_id = 0;
            item_id < instance_.number_of_items();
            ++item_id) {
        const Item& item = instance_.item(item_id);
        if (instance_.highest_efficiency_item_id_ == -1
                || item.efficiency
                > instance_.item(instance_.highest_efficiency_item_id_).efficiency) {
            instance_.highest_efficiency_item_id_ = item_id;
        }
    }

    return std::move(instance_);
}

void InstanceFromFloatProfitsBuilder::add_item(
        double profit,
        Weight weight)
{
    if (profit != profit) {
        throw std::invalid_argument(
                "Item profits must not be NaN.");
    }

    profits_double_.push_back(profit);
    Item item;
    item.profit = -1;
    item.weight = weight;
    instance_.items_.push_back(item);
};

Instance InstanceFromFloatProfitsBuilder::build()
{
    if (instance_.number_of_items() == 0)
        return std::move(instance_);

    // Check overflow because of total profit.
    //     multiplier * total_profit_double < INT_MAX
    //     multiplier < INT_MAX / total_profit_double
    double total_item_profit = 0;
    for (double profit: profits_double_)
        total_item_profit += profit;
    double highest_possible_multiplier = (double)std::numeric_limits<Profit>::max() / total_item_profit;

    // Check highest possible profit to avoid overflows when sorting.
    double highest_item_profit = 0;
    for (double profit: profits_double_)
        highest_item_profit = std::max(highest_item_profit, profit);
    Weight highest_item_weight = 0;
    for (ItemId item_id = 0;
            item_id < instance_.number_of_items();
            ++item_id) {
        const Item& item = instance_.item(item_id);
        highest_item_weight = std::max(highest_item_weight, item.weight);
    }
    Profit highest_possible_item_profit = std::numeric_limits<Profit>::max() / highest_item_weight;
    highest_possible_multiplier = std::min(
            highest_possible_multiplier,
            highest_possible_item_profit / highest_item_profit);

    // Check no overflow because of the bounds computed by 'upper_bound' and
    // 'upper_bound_reverse', which multiply an item's profit by a weight gap
    // that can be as large as 'capacity + 2 * highest_item_weight' (the
    // weight of a state can exceed the capacity by up to two item weights,
    // see PartialSort::sort_next_right_interval).
    // We want to ensure that:
    //     profit * bound_weight < INT_MAX
    // Since
    //     profit = profit_double * multiplier
    // That is:
    //     multiplier < INT_MAX / bound_weight / profit_double
    Weight bound_weight = instance_.capacity_;
    if (highest_item_weight
            > (std::numeric_limits<Weight>::max() - bound_weight) / 2) {
        throw std::overflow_error(
                "Overflow while computing the profit/weight bound.");
    }
    bound_weight += 2 * highest_item_weight;
    for (ItemId item_id = 0;
            item_id < instance_.number_of_items();
            ++item_id) {
        highest_possible_multiplier = std::min(
                highest_possible_multiplier,
                (double)(std::numeric_limits<Profit>::max())
                / bound_weight
                / profits_double_[item_id]);
    }

    // Compute multiplier.
    double multiplier = 1;
    while (2 * multiplier < highest_possible_multiplier)
        multiplier *= 2;
    while (multiplier > highest_possible_multiplier)
        multiplier /= 2;

    // The bounds on 'multiplier' above are computed in double precision and
    // don't account for the rounding of individual profits below (nor for
    // the "round up to 1" rule for tiny profits). At this magnitude, doubles
    // don't have enough precision to guarantee those bounds hold exactly, so
    // instead of trusting them, verify the actual rounded integer profits do
    // not overflow when summed, and that no single profit overflows once
    // multiplied by the capacity (as required by 'upper_bound'), halving the
    // multiplier and retrying if either check fails.
    Profit total_item_profit_int;
    for (;;) {
        total_item_profit_int = 0;
        bool overflow = false;
        for (ItemId item_id = 0;
                item_id < instance_.number_of_items();
                ++item_id) {
            Profit profit = std::round(profits_double_[item_id] * multiplier);
            if (profit == 0)
                profit = 1;
            if (profit > std::numeric_limits<Profit>::max() - total_item_profit_int) {
                overflow = true;
                break;
            }
            if (bound_weight > 0
                    && profit > std::numeric_limits<Profit>::max() / bound_weight) {
                overflow = true;
                break;
            }
            total_item_profit_int += profit;
        }
        if (!overflow)
            break;
        multiplier /= 2;
    }

    // Compute integer profits.
    for (ItemId item_id = 0;
            item_id < instance_.number_of_items();
            ++item_id) {
        Profit profit = std::round(profits_double_[item_id] * multiplier);
        if (profit == 0)
            profit = 1;
        instance_.items_[item_id].profit = profit;
    }

    // Check knapsack capacity.
    if (instance_.capacity() < 0) {
        throw std::invalid_argument(
                "The knapsack capacity must be positive.");
    }

    // Check that profit are positive and weights are positive and smaller than
    // the capacity.
    for (ItemId item_id = 0;
            item_id < instance_.number_of_items();
            ++item_id) {
        const Item& item = instance_.items_[item_id];
        if (item.profit <= 0) {
            throw std::invalid_argument(
                    "Items must have strictly positive profits.");
        }
        if (item.weight <= 0) {
            throw std::invalid_argument(
                    "Items must have strictly positive weights.");
        }
        if (item.weight > instance_.capacity()) {
            throw std::invalid_argument(
                    "The weight of an item must be smaller than the knapsack capacity..");
        }
        if (bound_weight > 0
                && item.profit > std::numeric_limits<Profit>::max() / bound_weight) {
            throw std::overflow_error(
                    "Overflow: 'item.profit * (capacity + 2 * highest_item_weight)' "
                    "must not exceed the maximum profit value.");
        }
    }

    // Compute total item profit and weight.
    for (ItemId item_id = 0;
            item_id < instance_.number_of_items();
            ++item_id) {
        const Item& item = instance_.items_[item_id];
        instance_.highest_item_profit_ = std::max(instance_.highest_item_profit_, item.profit);
        instance_.highest_item_weight_ = std::max(instance_.highest_item_weight_, item.weight);
        if (item.profit > std::numeric_limits<Profit>::max() - instance_.total_item_profit_) {
            throw std::overflow_error(
                    "Overflow while computing the total item profit.");
        }
        instance_.total_item_profit_ += item.profit;
        if (item.weight > std::numeric_limits<Weight>::max() - instance_.total_item_weight_) {
            throw std::overflow_error(
                    "Overflow while computing the total item weight.");
        }
        instance_.total_item_weight_ += item.weight;
    }

    // Compute item efficiencies.
    for (ItemId item_id = 0;
            item_id < instance_.number_of_items();
            ++item_id) {
        Item& item = instance_.items_[item_id];
        item.efficiency = (double)item.profit / item.weight;
    }

    // Compute highest efficiency item.
    instance_.highest_efficiency_item_id_ = -1;
    for (ItemId item_id = 0;
            item_id < instance_.number_of_items();
            ++item_id) {
        const Item& item = instance_.item(item_id);
        if (instance_.highest_efficiency_item_id_ == -1
                || item.efficiency
                > instance_.item(instance_.highest_efficiency_item_id_).efficiency) {
            instance_.highest_efficiency_item_id_ = item_id;
        }
    }

    return std::move(instance_);
}
