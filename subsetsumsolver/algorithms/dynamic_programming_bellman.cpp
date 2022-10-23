#include "subsetsumsolver/algorithms/dynamic_programming_bellman.hpp"

using namespace subsetsumsolver;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

Output subsetsumsolver::dynamic_programming_bellman_array(
        const Instance& instance,
        Info info)
{
    init_display(instance, info);
    info.os()
            << "Algorithm" << std::endl
            << "---------" << std::endl
            << "Dynamic Programming - Bellman" << std::endl
            << std::endl
            << "Parameters" << std::endl
            << "----------" << std::endl
            << "Implementation:                  iterative" << std::endl
            << "Method for retrieving solution:  no solution" << std::endl
            << std::endl;

    Output output(instance, info);
    Weight c = instance.capacity();
    std::vector<uint8_t> values(c + 1, 0);
    values[0] = 1;
    for (ItemPos j = 0; j < instance.number_of_items(); ++j) {
        // Check time
        if (!info.check_time())
            return output.algorithm_end(info);

        // Update DP table
        Weight wj = instance.weight(j);
        for (Weight w = c; w >= wj; w--)
            if (values[w] < values[w - wj])
                values[w] |= values[w - wj];
    }

    Weight opt = 0;
    for (Weight w = c; w >= 0; w--) {
        if (values[w] == 1) {
            opt = w;
            break;
        }
    }
    // Update lower bound
    output.update_lower_bound(
            opt,
            std::stringstream("tree search completed"),
            info);
    // Update upper bound
    output.update_upper_bound(
            opt,
            std::stringstream("tree search completed"),
            info);

    return output.algorithm_end(info);
}
