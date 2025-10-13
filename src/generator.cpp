#include "knapsacksolver/generator.hpp"

#include "knapsacksolver/instance_builder.hpp"

#include <cmath>

using namespace knapsacksolver;

Instance knapsacksolver::generate_u(
        ItemPos number_of_items,
        Weight maximum_weight,
        Profit maximum_profit,
        double capacity_ratio,
        std::mt19937_64& generator)
{
    InstanceBuilder instance_builder;

    // Add items.
    Weight weight_max = 0;
    Weight weight_sum = 0;
    for (ItemPos pos = 0; pos < number_of_items; ++pos) {
        std::uniform_int_distribution<Weight> distribution_weight(1, maximum_weight);
        std::uniform_int_distribution<Profit> distribution_profit(1, maximum_profit);
        Weight weight = distribution_weight(generator);
        Profit profit = distribution_profit(generator);
        instance_builder.add_item(profit, weight);

        weight_max = std::max(weight_max, weight);
        weight_sum += weight;
    }

    // Compute capacity.
    Weight capacity = std::max(
            weight_max,
            (Weight)(capacity_ratio * weight_sum));
    instance_builder.set_capacity(capacity);

    return instance_builder.build();
}
