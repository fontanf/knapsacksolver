#include "subsetsumsolver/algorithms/dynamic_programming_balancing.hpp"

using namespace subsetsumsolver;

////////////////////////////////////////////////////////////////////////////////
///////////////////// dynamic_programming_balancing_array //////////////////////
////////////////////////////////////////////////////////////////////////////////

Output subsetsumsolver::dynamic_programming_balancing_array(
        const Instance& instance,
        Info info)
{
    init_display(instance, info);
    info.os()
            << "Algorithm" << std::endl
            << "---------" << std::endl
            << "Dynamic Programming - Balancing" << std::endl
            << std::endl
            << "Parameters" << std::endl
            << "----------" << std::endl
            << "Implementation:                  Array" << std::endl
            << "Method for retrieving solution:  No solution" << std::endl
            << std::endl;

    Output output(instance, info);
    Weight c = instance.capacity();
    Weight w_max = 0;
    for (ItemId j = 0; j < instance.number_of_items(); ++j)
        w_max = std::max(w_max, instance.weight(j));
    // Compute the break item and the weight of the break solution.
    Weight w_break = 0;
    ItemId j_break = 0;
    for (ItemId j = 0; j < instance.number_of_items(); ++j) {
        Weight wj = instance.weight(j);
        if (w_break + wj > c) {
            j_break = j;
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
    values_pred[w_break - offset] = j_break;

    for (ItemId j = j_break; j < instance.number_of_items(); ++j) {
        // Check time
        if (info.needs_to_end())
            return output.algorithm_end(info);

        Weight wj = instance.weight(j);
        for (Weight w = c - w_max + 1 - offset; w <= c + w_max - offset; ++w)
            values_next[w] = values_pred[w];
        for (Weight w = c - w_max + 1 - offset; w <= c - offset; ++w)
            values_next[w + wj] = std::max(values_next[w + wj], values_pred[w]);
        for (Weight w = c + wj - offset; w >= c + 1 - offset; --w) {
            for (ItemId j0 = values_pred[w]; j0 < values_next[w]; ++j0) {
                Weight wj0 = instance.weight(j0);
                values_next[w - wj0] = std::max(values_next[w - wj0], j);
            }
        }

        values_pred.swap(values_next);

        // If optimum reached, stop.
        if (values_pred[c - offset] != 0)
            break;
    }

    // Retrieve optimal value.
    Weight opt = 0;
    for (Weight w = c - offset; w >= c - w_max + 1 - offset; --w) {
        if (values_pred[w] != 0) {
            opt = w + offset;
            break;
        }
    }
    // Update lower bound.
    output.update_lower_bound(
            opt,
            std::stringstream("tree search completed"),
            info);
    // Update upper bound.
    output.update_upper_bound(
            opt,
            std::stringstream("tree search completed"),
            info);

    return output.algorithm_end(info);
}
