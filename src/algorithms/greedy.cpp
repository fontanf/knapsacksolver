#include "knapsacksolver/algorithms/greedy.hpp"

#include "knapsacksolver/algorithm_formatter.hpp"
#include "knapsacksolver/sort.hpp"

using namespace knapsacksolver;

Output knapsacksolver::greedy(
        const Instance& instance,
        const GreedyParameters& parameters)
{
    Output output(instance);
    AlgorithmFormatter algorithm_formatter(parameters, output);
    algorithm_formatter.start("Greedy");

    // Check trivial cases.
    if (instance.total_item_weight() <= instance.capacity()) {
        Solution solution(instance);
        solution.fill();

        // Update solution.
        algorithm_formatter.update_solution(solution, "all items fit");
        // Update bound.
        algorithm_formatter.update_bound(solution.profit(), "all items fit");

        algorithm_formatter.end();
        return output;
    }

    Solution solution_forward(instance);
    Solution solution_backward(instance);

    if (parameters.full_sort != nullptr) {
        solution_forward = parameters.full_sort->break_solution();
        solution_backward = parameters.full_sort->break_solution();
        solution_backward.add(parameters.full_sort->break_item_id());
    } else if (parameters.partial_sort != nullptr) {
        solution_forward = parameters.partial_sort->break_solution();
        solution_backward = parameters.partial_sort->break_solution();
        solution_backward.add(parameters.partial_sort->break_item_id());
    } else {
        PartialSort partial_sort(instance);
        solution_forward = partial_sort.break_solution();
        solution_backward = partial_sort.break_solution();
        solution_backward.add( partial_sort.break_item_id());
    }

    if (solution_forward.weight() > instance.capacity()) {
        throw std::logic_error("forward");
    }

    if (solution_backward.weight() < instance.capacity()) {
        throw std::logic_error("backward");
    }

    // Find the best item to add to the forward solution and the best item to
    // remove from the backward solution.
    ItemId item_id_best_forward = -1;
    ItemId item_id_best_backward = -1;
    for (ItemId item_id = 0;
            item_id < instance.number_of_items();
            ++item_id) {
        const Item& item = instance.item(item_id);
        // Forward.
        if (!solution_forward.contains(item_id)) {
            if (solution_forward.weight() + item.weight
                    <= instance.capacity()) {
                if (item_id_best_forward == -1
                        || instance.item(item_id_best_forward).profit
                        < item.profit) {
                    item_id_best_forward = item_id;
                }
            }
        }
        // Backward.
        if (solution_backward.contains(item_id)) {
            if (solution_forward.weight() - item.weight
                    <= instance.capacity()) {
                if (item_id_best_backward == -1
                        || instance.item(item_id_best_backward).profit
                        > item.profit) {
                    item_id_best_backward = item_id;
                }
            }
        }
    }
    if (item_id_best_forward != -1)
        solution_forward.add(item_id_best_forward);
    algorithm_formatter.update_solution(solution_forward, "forward");

    if (item_id_best_backward != -1) {
        solution_backward.remove(item_id_best_backward);
        algorithm_formatter.update_solution(solution_backward, "backward");
    }

    algorithm_formatter.end();
    return output;
}
