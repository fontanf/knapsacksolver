#include "knapsacksolver/algorithms/upper_bound_dantzig.hpp"

#include "knapsacksolver/algorithm_formatter.hpp"
#include "knapsacksolver/upper_bound.hpp"

using namespace knapsacksolver;

Output knapsacksolver::upper_bound_dantzig(
        const Instance& instance,
        const UpperBoundDantzigParameters& parameters)
{
    Output output(instance);
    AlgorithmFormatter algorithm_formatter(parameters, output);
    algorithm_formatter.start("Dantzig upper bound");

    // Check trivial cases.
    if (instance.total_item_weight() <= instance.capacity()) {
        // Update bound.
        algorithm_formatter.update_bound(
                instance.total_item_profit(),
                "all items fit");

        algorithm_formatter.end();
        return output;
    }

    Profit upper_bound_curr = -1;
    if (parameters.full_sort != nullptr) {
        upper_bound_curr = upper_bound(
                instance,
                parameters.full_sort->break_solution().profit(),
                parameters.full_sort->break_solution().weight(),
                parameters.full_sort->break_item_id());
    } else if (parameters.partial_sort != nullptr) {
        upper_bound_curr = upper_bound(
                instance,
                parameters.partial_sort->break_solution().profit(),
                parameters.partial_sort->break_solution().weight(),
                parameters.partial_sort->break_item_id());
    } else {
        PartialSort partial_sort(instance);
        upper_bound_curr = upper_bound(
                instance,
                partial_sort.break_solution().profit(),
                partial_sort.break_solution().weight(),
                partial_sort.break_item_id());
    }

    // Update bound.
    algorithm_formatter.update_bound(
            upper_bound_curr,
            "algorithm end (bound)");

    algorithm_formatter.end();
    return output;
}
