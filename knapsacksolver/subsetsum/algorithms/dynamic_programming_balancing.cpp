#include "knapsacksolver/subsetsum/algorithms/dynamic_programming_balancing.hpp"

#include "knapsacksolver/subsetsum/algorithm_formatter.hpp"

using namespace knapsacksolver::subsetsum;

////////////////////////////////////////////////////////////////////////////////
///////////////////// dynamic_programming_balancing_array //////////////////////
////////////////////////////////////////////////////////////////////////////////

const Output knapsacksolver::subsetsum::dynamic_programming_balancing_array(
        const Instance& instance,
        const Parameters& parameters)
{
    Output output(instance);
    AlgorithmFormatter algorithm_formatter(parameters, output);
    algorithm_formatter.start("Dynamic programming - balancing - array");
    algorithm_formatter.print_header();

    Weight c = instance.capacity();
    Weight w_max = 0;
    for (ItemId item_id = 0; item_id < instance.number_of_items(); ++item_id)
        w_max = std::max(w_max, instance.weight(item_id));
    // Compute the break item and the weight of the break solution.
    Weight w_break = 0;
    ItemId break_item_id = 0;
    for (ItemId item_id = 0;
            item_id < instance.number_of_items();
            ++item_id) {
        Weight wj = instance.weight(item_id);
        if (w_break + wj > c) {
            break_item_id = item_id;
            break;
        }
        w_break += wj;
    }
    // Create and initialize the tables.
    std::vector<ItemId> values_pred(w_max * 2, -1);
    std::vector<ItemId> values_next(w_max * 2, -1);
    Weight offset = c - w_max + 1;
    for (Weight w = c - w_max + 1 - offset; w <= c - offset; ++w)
        values_pred[w] = 0;
    for (Weight w = c + 1 - offset; w <= c + w_max - offset; ++w)
        values_pred[w] = 1;
    values_pred[w_break - offset] = break_item_id;

    for (ItemId item_id = break_item_id;
            item_id < instance.number_of_items();
            ++item_id) {

        // Check time
        if (parameters.timer.needs_to_end()) {
            algorithm_formatter.end();
            return output;
        }

        Weight wj = instance.weight(item_id);
        for (Weight w = c - w_max + 1 - offset; w <= c + w_max - offset; ++w)
            values_next[w] = values_pred[w];
        for (Weight w = c - w_max + 1 - offset; w <= c - offset; ++w)
            values_next[w + wj] = std::max(values_next[w + wj], values_pred[w]);
        for (Weight w = c + wj - offset; w >= c + 1 - offset; --w) {
            for (ItemId item_id_0 = values_pred[w];
                    item_id_0 < values_next[w];
                    ++item_id_0) {
                Weight wj0 = instance.weight(item_id_0);
                values_next[w - wj0] = std::max(values_next[w - wj0], item_id);
            }
        }

        values_pred.swap(values_next);

        // If optimum reached, stop.
        if (values_pred[c - offset] != 0)
            break;
    }

    // Retrieve optimal value.
    Weight optimal_value = 0;
    for (Weight weight = c - offset;
            weight >= c - w_max + 1 - offset;
            --weight) {
        if (values_pred[weight] != 0) {
            optimal_value = weight + offset;
            break;
        }
    }
    // Update lower bound.
    algorithm_formatter.update_value(
            optimal_value,
            "algorithm end (value)");
    // Update upper bound.
    algorithm_formatter.update_bound(
            optimal_value,
            "algorithm end (bound)");

    algorithm_formatter.end();
    return output;
}
