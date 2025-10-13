#include "knapsacksolver/algorithms/dynamic_programming_bellman.hpp"

#include "knapsacksolver/algorithm_formatter.hpp"
#include "knapsacksolver/sort.hpp"
#include "knapsacksolver/upper_bound.hpp"
#include "knapsacksolver/algorithms/upper_bound_dantzig.hpp"
#include "knapsacksolver/algorithms/greedy.hpp"

#include "optimizationtools/containers/partial_set.hpp"

#include <thread>

using namespace knapsacksolver;

////////////////////////////////////////////////////////////////////////////////
////////////////////// dynamic_programming_bellman_array ///////////////////////
////////////////////////////////////////////////////////////////////////////////

Output knapsacksolver::dynamic_programming_bellman_array(
        const Instance& instance,
        const Parameters& parameters)
{
    Output output(instance);
    AlgorithmFormatter algorithm_formatter(parameters, output);
    algorithm_formatter.start("Dynamic programming - Bellman - array - no solution");
    algorithm_formatter.print_header();

    std::vector<Profit> values(instance.capacity() + 1, 0);
    for (ItemId item_id = 0;
            item_id < instance.number_of_items();
            ++item_id) {
        const Item& item = instance.item(item_id);

        // Check time
        if (parameters.timer.needs_to_end()) {
            algorithm_formatter.end();
            return output;
        }

        // Update DP table
        for (Weight weight = instance.capacity();
                weight >= item.weight;
                --weight) {
            if (values[weight] < values[weight - item.weight] + item.profit)
                values[weight] = values[weight - item.weight] + item.profit;
        }

        // Update value.
        if (output.value < values[instance.capacity()]) {
            std::stringstream ss;
            ss << "it " << item_id;
            algorithm_formatter.update_value(
                    values[instance.capacity()],
                    ss.str());
        }
    }

    // Update bound.
    algorithm_formatter.update_bound(
            output.value,
            "algorithm end");

    algorithm_formatter.end();
    return output;
}

////////////////////////////////////////////////////////////////////////////////
////////////////// dynamic_programming_bellman_array_parallel //////////////////
////////////////////////////////////////////////////////////////////////////////

void dynamic_programming_bellman_array_parallel_worker(
        const Instance& instance,
        ItemPos item_id_start,
        ItemPos item_id_end,
        std::vector<Profit>::iterator values,
        const Parameters& parameters)
{
    for (ItemId item_id = item_id_start;
            item_id < item_id_end;
            ++item_id) {
        const Item& item = instance.item(item_id);

        // Check end.
        if (parameters.timer.needs_to_end())
            return;

        for (Weight weight = instance.capacity();
                weight >= item.weight;
                --weight) {
            if (*(values + weight) < *(values + weight - item.weight) + item.profit)
                *(values + weight) = *(values + weight - item.weight) + item.profit;
        }
    }
}

Output knapsacksolver::dynamic_programming_bellman_array_parallel(
        const Instance& instance,
        const Parameters& parameters)
{
    Output output(instance);
    AlgorithmFormatter algorithm_formatter(parameters, output);
    algorithm_formatter.start("Dynamic programming - Bellman parallel - array - only value");
    algorithm_formatter.print_header();

    // Check trivial cases.
    if (instance.total_item_weight() <= instance.capacity()) {
        Solution solution(instance);
        solution.fill();

        // Update solution.
        algorithm_formatter.update_solution(
                solution,
                "all items fit (solution)");
        // Update bound.
        algorithm_formatter.update_bound(
                output.value,
                "all items fit (bound)");

        algorithm_formatter.end();
        return output;
    }

    // Partition items and solve both knapsacks
    ItemId item_id_middle = (instance.number_of_items() - 1) / 2 + 1;
    std::vector<Profit> values1(instance.capacity() + 1, 0);
    std::thread thread(
            dynamic_programming_bellman_array_parallel_worker,
            std::ref(instance),
            0,
            item_id_middle,
            values1.begin(),
            parameters);
    std::vector<Profit> values2(instance.capacity() + 1, 0);
    dynamic_programming_bellman_array_parallel_worker(
            std::ref(instance),
            item_id_middle,
            instance.number_of_items(),
            values2.begin(),
            parameters);
    thread.join();
    if (parameters.timer.needs_to_end()) {
        algorithm_formatter.end();
        return output;
    }

    // Compute optimal value.
    Profit optimal_value = -1;
    for (Weight capacity = 0; capacity <= instance.capacity(); ++capacity) {
        Profit value = values1[capacity] + values2[instance.capacity() - capacity];
        if (optimal_value < value)
            optimal_value = value;
    }

    // Update value.
    algorithm_formatter.update_value(
            optimal_value,
            "algorithm end (value)");
    // Update bound.
    algorithm_formatter.update_bound(
            output.value,
            "algorithm end (bound)");

    algorithm_formatter.end();
    return output;
}

////////////////////////////////////////////////////////////////////////////////
/////////////////////// dynamic_programming_bellman_rec ////////////////////////
////////////////////////////////////////////////////////////////////////////////

namespace
{

inline StateId compute_state_id(
        const Instance& instance,
        ItemId item_id,
        Weight weight)
{
    return (item_id + 1) * (instance.capacity() + 1) + weight;
}

}

Profit dynamic_programming_bellman_rec_rec(
        const Instance& instance,
        std::vector<Profit>& values,
        ItemId item_id,
        Weight weight,
        const Parameters& parameters)
{
    if (parameters.timer.needs_to_end())
        return -1;
    StateId state_id = compute_state_id(instance, item_id, weight);
    if (values[state_id] == -1) {
        if (item_id == -1) {
            values[state_id] = 0;
        } else if (instance.item(item_id).weight > weight) {
            Profit p1 = dynamic_programming_bellman_rec_rec(
                    instance,
                    values,
                    item_id - 1,
                    weight,
                    parameters);
            values[state_id] = p1;
        } else {
            Profit profit_1 = dynamic_programming_bellman_rec_rec(
                    instance,
                    values,
                    item_id - 1,
                    weight,
                    parameters);
            Profit profit_2 = instance.item(item_id).profit
                + dynamic_programming_bellman_rec_rec(
                    instance,
                    values,
                    item_id - 1,
                    weight - instance.item(item_id).weight,
                    parameters);
            values[state_id] = std::max(profit_1, profit_2);
        }
    }
    return values[state_id];
}

Output knapsacksolver::dynamic_programming_bellman_rec(
        const Instance& instance,
        const Parameters& parameters)
{
    Output output(instance);
    AlgorithmFormatter algorithm_formatter(parameters, output);
    algorithm_formatter.start("Dynamic programming - Bellman - recursive - only value");
    algorithm_formatter.print_header();

    // Initialize memory table
    StateId values_size = (instance.number_of_items() + 1) * (instance.capacity() + 1);
    std::vector<Profit> values(values_size, -1);

    // Compute recursively
    Profit optimal_value = dynamic_programming_bellman_rec_rec(
            instance,
            values,
            instance.number_of_items() - 1,
            instance.capacity(),
            parameters);
    if (parameters.timer.needs_to_end()) {
        algorithm_formatter.end();
        return output;
    }

    // Update value.
    algorithm_formatter.update_value(
            optimal_value,
            "algorithm end (value)");
    // Update bound.
    algorithm_formatter.update_bound(
            output.value,
            "algorithm end (bound)");

    // Retrieve optimal solution.
    Weight weight = instance.capacity();
    Solution solution(instance);
    for (ItemId item_id = instance.number_of_items() - 1;
            item_id >= 0;
            --item_id) {
        StateId state_id = compute_state_id(instance, item_id, weight);
        StateId state_id_prev = compute_state_id(instance, item_id - 1, weight);
        if (values[state_id] != values[state_id_prev]) {
            weight -= instance.item(item_id).weight;
            solution.add(item_id);
        }
    }
    algorithm_formatter.update_solution(
            solution,
            "algorithm end (solution)");

    algorithm_formatter.end();
    return output;
}

////////////////////////////////////////////////////////////////////////////////
//////////////////// dynamic_programming_bellman_array_all /////////////////////
////////////////////////////////////////////////////////////////////////////////

Output knapsacksolver::dynamic_programming_bellman_array_all(
        const Instance& instance,
        const Parameters& parameters)
{
    Output output(instance);
    AlgorithmFormatter algorithm_formatter(parameters, output);
    algorithm_formatter.start("Dynamic programming - Bellman - array - store all states");
    algorithm_formatter.print_header();

    // Initialize memory table
    StateId values_size = (instance.number_of_items() + 1) * (instance.capacity() + 1);
    std::vector<Profit> values(values_size);

    // Compute optimal value
    std::fill(values.begin(), values.begin() + instance.capacity() + 1, 0);
    for (ItemId item_id = 0;
            item_id < instance.number_of_items();
            ++item_id) {
        const Item& item = instance.item(item_id);

        // Check time
        if (parameters.timer.needs_to_end()) {
            algorithm_formatter.end();
            return output;
        }

        // Fill DP table
        for (Weight weight = 0; weight < item.weight; ++weight) {
            values[compute_state_id(instance, item_id, weight)]
                = values[compute_state_id(instance, item_id - 1, weight)];
        }
        for (Weight weight = item.weight;
                weight <= instance.capacity();
                ++weight) {
            Profit profit_0 = values[compute_state_id(instance, item_id - 1, weight)];
            Profit profit_1 = values[compute_state_id(instance, item_id - 1, weight - item.weight)] + item.profit;
            values[compute_state_id(instance, item_id, weight)] = std::max(profit_0, profit_1);
        }

        // Update lower bound
        if (output.value < values[compute_state_id(instance, item_id, instance.capacity())]) {
            std::stringstream ss;
            ss << "it " << item_id;
            algorithm_formatter.update_value(
                    values[compute_state_id(instance, item_id, instance.capacity())],
                    ss.str());
        }
    }

    // Update upper bound
    algorithm_formatter.update_bound(
            output.value,
            "algorithm end (bound)");

    // Retrieve optimal solution.
    Weight weight = instance.capacity();
    Solution solution(instance);
    for (ItemId item_id = instance.number_of_items() - 1;
            item_id >= 0;
            --item_id) {
        StateId state_id = compute_state_id(instance, item_id, weight);
        StateId state_id_prev = compute_state_id(instance, item_id - 1, weight);
        if (values[state_id] != values[state_id_prev]) {
            weight -= instance.item(item_id).weight;
            solution.add(item_id);
        }
    }
    algorithm_formatter.update_solution(
            solution,
            "algorithm end (solution)");

    algorithm_formatter.end();
    return output;
}

////////////////////////////////////////////////////////////////////////////////
//////////////////// dynamic_programming_bellman_array_one /////////////////////
////////////////////////////////////////////////////////////////////////////////

const DynamicProgrammingBellmanArrayOneOutput knapsacksolver::dynamic_programming_bellman_array_one(
        const Instance& instance,
        const Parameters& parameters)
{
    DynamicProgrammingBellmanArrayOneOutput output(instance);
    AlgorithmFormatter algorithm_formatter(parameters, output);
    algorithm_formatter.start("Dynamic programming - Bellman - array - single line");
    algorithm_formatter.print_header();

    // Check trivial cases.
    if (instance.total_item_weight() <= instance.capacity()) {
        Solution solution(instance);
        solution.fill();

        // Update solution.
        algorithm_formatter.update_solution(
                solution,
                "all items fit (solution)");
        // Update bound.
        algorithm_formatter.update_bound(
                output.value,
                "all items fit (bound)");

        algorithm_formatter.end();
        return output;
    }

    // Initialize memory table.
    std::vector<Profit> values(instance.capacity() + 1);

    Profit optimal_value = -1;
    Profit optimal_value_local = -1;
    Solution solution(instance);
    ItemId last_item_id = instance.number_of_items() - 1;
    Weight remaining_capacity = instance.capacity();
    while (solution.profit() != optimal_value) {
        output.number_of_iterations++;

        // Initialization
        std::fill(values.begin(), values.end(), 0);
        ItemId new_last_item_id = -1;

        // Recursion
        for (ItemId item_id = 0;
                item_id <= last_item_id;
                ++item_id) {
            const Item& item = instance.item(item_id);

            if (item.weight > remaining_capacity)
                continue;

            // Check time
            if (parameters.timer.needs_to_end()) {
                algorithm_formatter.end();
                return output;
            }

            if (values[remaining_capacity - item.weight] + item.profit > values[remaining_capacity]) {
                values[remaining_capacity] = values[remaining_capacity - item.weight] + item.profit;
                new_last_item_id = item_id;
                if (values[remaining_capacity] == optimal_value_local) {
                    goto end;
                }
            }

            // For other values of w
            for (Weight weight = remaining_capacity;
                    weight >= item.weight;
                    --weight) {
                if (values[weight - item.weight] + item.profit > values[weight])
                    values[weight] = values[weight - item.weight] + item.profit;
            }

            // Update value.
            if (output.value < values[remaining_capacity]) {
                std::stringstream ss;
                ss << "it " << item_id;
                algorithm_formatter.update_value(values[remaining_capacity], ss.str());
            }
        }
end:

        // If first iteration, memorize optimal value
        if (output.number_of_iterations == 1) {
            optimal_value = values[instance.capacity()];

            // Update value.
            algorithm_formatter.update_value(
                    optimal_value,
                    "algorithm end (value)");
            // Update bound.
            algorithm_formatter.update_bound(
                    output.value,
                    "algorithm end (bound)");
        }

        // Update recursion parameters.
        solution.add(new_last_item_id);
        remaining_capacity -= instance.item(new_last_item_id).weight;
        optimal_value_local -= instance.item(new_last_item_id).profit;
        last_item_id = new_last_item_id - 1;
    }

    // Update solution.
    algorithm_formatter.update_solution(
            solution,
            "algorithm end (solution)");

    algorithm_formatter.end();
    return output;
}

////////////////////////////////////////////////////////////////////////////////
/////////////////// dynamic_programming_bellman_array_part /////////////////////
////////////////////////////////////////////////////////////////////////////////

const DynamicProgrammingBellmanArrayPartOutput knapsacksolver::dynamic_programming_bellman_array_part(
        const Instance& instance,
        const DynamicProgrammingBellmanArrayPartParameters& parameters)
{
    DynamicProgrammingBellmanArrayPartOutput output(instance);
    AlgorithmFormatter algorithm_formatter(parameters, output);
    algorithm_formatter.start("Dynamic programming - Bellman - array - partial");
    algorithm_formatter.print_header();

    // Check trivial cases.
    if (instance.total_item_weight() <= instance.capacity()) {
        Solution solution(instance);
        solution.fill();

        // Update solution.
        algorithm_formatter.update_solution(
                solution,
                "all items fit (solution)");
        // Update bound.
        algorithm_formatter.update_bound(
                output.value,
                "all items fit (bound)");

        algorithm_formatter.end();
        return output;
    }

    // Initialize memory table
    std::vector<Profit> values(instance.capacity() + 1);
    std::vector<optimizationtools::PartialSet> partial_solutions(instance.capacity() + 1);

    Profit optimal_value = -1;
    Profit optimal_value_local = -1;
    Weight remaining_capacity = instance.capacity();
    Solution solution(instance);
    ItemId first_item_id = 0;
    ItemId last_item_id = instance.number_of_items() - 1;
    while (solution.profit() != optimal_value) {
        output.number_of_iterations++;

        optimizationtools::PartialSetFactory partial_set_factory(
                instance.number_of_items(),
                parameters.partial_solution_size);
        for (ItemId item_id = first_item_id;
                item_id <= last_item_id;
                ++item_id) {
            partial_set_factory.add_element_to_factory(item_id);
        }

        Weight optimal_weight = remaining_capacity;

        // Initialization
        std::fill(values.begin(), values.end(), 0);
        std::fill(partial_solutions.begin(), partial_solutions.end(), 0);
        ItemId new_last_item_id = last_item_id;

        // Recursion
        for (ItemId item_id = first_item_id;
                item_id <= last_item_id;
                ++item_id) {
            const Item& item = instance.item(item_id);

            // Check end.
            if (parameters.timer.needs_to_end()) {
                algorithm_formatter.end();
                return output;
            }

            // For other values of w
            for (Weight weight = remaining_capacity;
                    weight >= item.weight;
                    --weight) {
                if (values[weight - item.weight] + item.profit > values[weight]) {
                    values[weight] = values[weight - item.weight] + item.profit;
                    partial_solutions[weight] = partial_set_factory.add(
                            partial_solutions[weight - item.weight],
                            item_id);
                    if (values[weight] == optimal_value_local) {
                        optimal_weight = weight;
                        new_last_item_id = item_id;
                        goto end;
                    }
                }
            }

            // Update lower bound
            if (output.value < values[remaining_capacity]) {
                std::stringstream ss;
                ss << "it " << item_id;
                algorithm_formatter.update_value(
                        values[remaining_capacity],
                        ss.str());
            }
        }
end:

        // If first iteration, memorize optimal value
        if (first_item_id == 0) {
            optimal_value = values[optimal_weight];

            // Update value.
            algorithm_formatter.update_value(
                    optimal_value,
                    "algorithm end (value)");
            // Update bound.
            algorithm_formatter.update_bound(
                    output.value,
                    "algorithm end (bound)");
        }

        // Update recursion parameters.
        for (ItemId item_id = 0;
                item_id < instance.number_of_items();
                ++item_id) {
            if (partial_set_factory.contains(
                        partial_solutions[optimal_weight],
                        item_id)) {
                solution.add(item_id);
            }
        }
        if (!solution.feasible()) {
            throw std::runtime_error("!solution.feasible()");
        }
        if (solution.profit() == output.bound)
            break;

        first_item_id += parameters.partial_solution_size;
        last_item_id = new_last_item_id;
        remaining_capacity = instance.capacity() - solution.weight();
        optimal_value_local = optimal_value - solution.profit();
    }

    // Update solution.
    algorithm_formatter.update_solution(
            solution,
            "algorithm end (solution)");

    algorithm_formatter.end();
    return output;
}

////////////////////////////////////////////////////////////////////////////////
//////////////////// dynamic_programming_bellman_array_rec /////////////////////
////////////////////////////////////////////////////////////////////////////////

struct RecData
{
    /** Instance. */
    const Instance& instance;

    /** Parameters. */
    const Parameters& parameters;

    /** Optimal solution, filled during the algorithm. */
    Solution& solution;

    /** First item. */
    ItemPos item_id_1;

    /** Last item. */
    ItemPos item_id_2;

    /** Current capacity. */
    Weight capacity;

    /** Temporary structure to store values. */
    std::vector<Profit>::iterator values;
};

std::vector<Profit> opts_dynamic_programming_bellman_array(
        const Instance& instance,
        const Parameters& parameters,
        ItemPos item_id_1,
        ItemPos item_id_2,
        Weight capacity)
{
    std::vector<Profit> values(capacity + 1, 0);
    for (ItemId item_id = item_id_1; item_id < item_id_2; ++item_id) {
        if (parameters.timer.needs_to_end())
            break;
        const Item& item = instance.item(item_id);
        for (Weight weight = capacity; weight >= item.weight; weight--)
            if (values[weight] < values[weight - item.weight] + item.profit)
                values[weight] = values[weight - item.weight] + item.profit;
    }
    return values;
}

void dynamic_programming_bellman_array_rec_rec(
        const Instance& instance,
        const Parameters& parameters,
        Solution& solution,
        ItemPos item_id_1,
        ItemPos item_id_2,
        Weight capacity)
{
    ItemPos item_id_middle = (item_id_1 + item_id_2 - 1) / 2 + 1;

    auto values_1 = opts_dynamic_programming_bellman_array(
            instance,
            parameters,
            item_id_1,
            item_id_middle,
            capacity);
    auto values_2 = opts_dynamic_programming_bellman_array(
            instance,
            parameters,
            item_id_middle,
            item_id_2,
            capacity);

    Profit value_max  = -1;
    Weight optimal_capacity_1 = 0;
    Weight optimal_capacity_2 = 0;
    for (Weight capacity_1 = 0; capacity_1 <= capacity; ++capacity_1) {
        Weight capacity_2 = capacity - capacity_1;
        Profit value = values_1[capacity_1] + values_2[capacity_2];
        if (value > value_max) {
            value_max = value;
            optimal_capacity_1 = capacity_1;
            optimal_capacity_2 = capacity_2;
        }
    }

    if (item_id_1 == item_id_middle - 1)
        if (values_1[optimal_capacity_1] == instance.item(item_id_1).profit)
            solution.add(item_id_1);
    if (item_id_middle == item_id_2 - 1)
        if (values_2[optimal_capacity_2] == instance.item(item_id_middle).profit)
            solution.add(item_id_middle);

    if (item_id_1 != item_id_middle - 1) {
        dynamic_programming_bellman_array_rec_rec(
                instance,
                parameters,
                solution,
                item_id_1,
                item_id_middle,
                optimal_capacity_1);
    }
    if (item_id_middle != item_id_2 - 1) {
        dynamic_programming_bellman_array_rec_rec(
                instance,
                parameters,
                solution,
                item_id_middle,
                item_id_2,
                optimal_capacity_2);
    }
}

Output knapsacksolver::dynamic_programming_bellman_array_rec(
        const Instance& instance,
        const Parameters& parameters)
{
    Output output(instance);
    AlgorithmFormatter algorithm_formatter(parameters, output);
    algorithm_formatter.start("Dynamic programming - Bellman - array - recursive scheme");
    algorithm_formatter.print_header();
    FFOT_DBG(std::shared_ptr<optimizationtools::Logger> logger = parameters.get_logger();)

    // Check trivial cases.
    if (instance.total_item_weight() <= instance.capacity()) {
        Solution solution(instance);
        solution.fill();

        // Update solution.
        algorithm_formatter.update_solution(
                solution,
                "all items fit (solution)");
        // Update bound.
        algorithm_formatter.update_bound(
                output.value,
                "all items fit (bound)");

        algorithm_formatter.end();
        return output;
    }

    // Start recursion.
    Solution solution(instance);
    dynamic_programming_bellman_array_rec_rec(
            instance,
            parameters,
            solution,
            0,
            instance.number_of_items(),
            instance.capacity());
    if (parameters.timer.needs_to_end()) {
        algorithm_formatter.end();
        return output;
    }

    // Update solution.
    algorithm_formatter.update_solution(
            solution,
            "algorithm end (solution)");
    // Update bound.
    algorithm_formatter.update_bound(
            output.value,
            "algorithm end (bound)");

    algorithm_formatter.end();
    return output;
}

////////////////////////////////////////////////////////////////////////////////
/////////////////////// dynamic_programming_bellman_list ///////////////////////
////////////////////////////////////////////////////////////////////////////////

struct BellmanState
{
    Weight weight;
    Profit profit;
};

std::ostream& operator<<(std::ostream& os, const BellmanState& s)
{
    os << "(" << s.weight <<  "," << s.profit << ")";
    return os;
}

std::ostream& operator<<(std::ostream& os, const std::vector<BellmanState>& l)
{
    std::copy(l.begin(), l.end(), std::ostream_iterator<BellmanState>(os, " "));
    return os;
}

Output knapsacksolver::dynamic_programming_bellman_list(
        const Instance& instance,
        const DynamicProgrammingBellmanListParameters& parameters)
{
    Output output(instance);
    AlgorithmFormatter algorithm_formatter(parameters, output);
    algorithm_formatter.start("Dynamic programming - Bellman - states - only value");
    algorithm_formatter.print_header();
    FFOT_DBG(std::shared_ptr<optimizationtools::Logger> logger = parameters.get_logger();)

    // Check trivial cases.
    if (instance.total_item_weight() <= instance.capacity()) {
        Solution solution(instance);
        solution.fill();

        // Update solution.
        algorithm_formatter.update_solution(
                solution,
                "all items fit (solution)");
        // Update bound.
        algorithm_formatter.update_bound(
                output.value,
                "all items fit (bound)");

        algorithm_formatter.end();
        return output;
    }

    // Find break item.
    std::unique_ptr<PartialSort> partial_sort;
    std::unique_ptr<FullSort> full_sort;
    if (parameters.sort) {
        full_sort = std::unique_ptr<FullSort>(new FullSort(instance));
    } else {
        partial_sort = std::unique_ptr<PartialSort>(new PartialSort(instance));
    }

    // Get the greedy solution.
    GreedyParameters greedy_parameters;
    greedy_parameters.timer = parameters.timer;
    greedy_parameters.verbosity_level = 0;
    if (parameters.sort) {
        greedy_parameters.full_sort = full_sort.get();
    } else {
        greedy_parameters.partial_sort = partial_sort.get();
    }
    auto greedy_output = greedy(instance, greedy_parameters);

    // Update solution.
    algorithm_formatter.update_solution(
            greedy_output.solution,
            "greedy");

    // Compute an initial bound.
    UpperBoundDantzigParameters upper_bound_dantzig_parameters;
    upper_bound_dantzig_parameters.timer = parameters.timer;
    upper_bound_dantzig_parameters.verbosity_level = 0;
    if (parameters.sort) {
        upper_bound_dantzig_parameters.full_sort = full_sort.get();
    } else {
        upper_bound_dantzig_parameters.partial_sort = partial_sort.get();
    }
    auto upper_bound_output = upper_bound_dantzig(
            instance,
            upper_bound_dantzig_parameters);

    // Update bound.
    algorithm_formatter.update_bound(
            upper_bound_output.bound,
            "dantzig upper bound");

    // Main recursion.
    std::vector<BellmanState> l0{{0, 0}};
    for (ItemPos item_id = 0;
            item_id < instance.number_of_items() && !l0.empty();
            ++item_id) {
        const Item& item = (!parameters.sort)?
            instance.item(item_id):
            instance.item(full_sort->item_id(item_id));
        ItemId bound_item_id = (item_id == instance.number_of_items() - 1)?
            -1:
            (!parameters.sort)?
            instance.highest_efficiency_item_id():
            full_sort->item_id(item_id + 1);

        // Check end.
        if (parameters.timer.needs_to_end()) {
            algorithm_formatter.end();
            return output;
        }

        Profit upper_bound_it = -1;
        std::vector<BellmanState> l;
        std::vector<BellmanState>::iterator it = l0.begin();
        std::vector<BellmanState>::iterator it1 = l0.begin();
        while (it != l0.end() || it1 != l0.end()) {
            if (it1 != l0.end()
                    && (it == l0.end() || it->weight > it1->weight + item.weight)) {

                BellmanState s1{it1->weight + item.weight, it1->profit + item.profit};

                // Check capacity.
                if (s1.weight > instance.capacity())
                    break;

                Profit upper_bound_curr = upper_bound(
                        instance,
                        s1.profit,
                        s1.weight,
                        bound_item_id);

                if (upper_bound_curr > output.value
                        && (l.empty() || s1.profit > l.back().profit)) {

                    // Update value.
                    if (output.value < s1.profit) {
                        std::stringstream ss;
                        ss << "it " << item_id;
                        algorithm_formatter.update_value(s1.profit, ss.str());
                    }

                    // Update current bound.
                    if (upper_bound_it < upper_bound_curr)
                        upper_bound_it = upper_bound_curr;

                    if (!l.empty() && s1.weight == l.back().weight) {
                        l.back() = s1;
                    } else {
                        l.push_back(s1);
                    }
                }
                it1++;
            } else {
                assert(it != l0.end());

                Profit upper_bound_curr = upper_bound(
                        instance,
                        it->profit,
                        it->weight,
                        bound_item_id);

                if (upper_bound_curr > output.value
                        && (l.empty() || it->profit > l.back().profit)) {

                    // Update current bound.
                    if (upper_bound_it < upper_bound_curr)
                        upper_bound_it = upper_bound_curr;

                    if (!l.empty() && it->weight == l.back().weight) {
                        l.back() = *it;
                    } else {
                        l.push_back(*it);
                    }
                }
                ++it;
            }
        }
        l0 = std::move(l);

        // Update bound.
        upper_bound_it = std::max(upper_bound_it, output.value);
        if (output.bound > upper_bound_it) {
            std::stringstream ss;
            ss << "it " << item_id;
            algorithm_formatter.update_bound(upper_bound_it, ss.str());
        }
    }
    // Update bound.
    algorithm_formatter.update_bound(
            output.value,
            "algorithm end (bound)");

    algorithm_formatter.end();
    return output;
}
