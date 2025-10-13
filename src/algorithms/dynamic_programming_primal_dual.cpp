#include "knapsacksolver/algorithms/dynamic_programming_primal_dual.hpp"

#include "knapsacksolver/instance_builder.hpp"
#include "knapsacksolver/algorithm_formatter.hpp"
#include "knapsacksolver/sort.hpp"
#include "knapsacksolver/upper_bound.hpp"
#include "knapsacksolver/algorithms/upper_bound_dantzig.hpp"
#include "knapsacksolver/algorithms/greedy.hpp"

#include "optimizationtools/containers/partial_set.hpp"

using namespace knapsacksolver;

namespace
{

struct DynamicProgrammingPrimalDualState
{
    /** Weight of the state. */
    Weight weight;

    /** Profit of the state. */
    Profit profit;

    /** Partial solution stored in the state. */
    optimizationtools::PartialSet partial_solution;
};

struct DynamicProgrammingPrimalDualInternalData
{
    DynamicProgrammingPrimalDualInternalData(
            const Instance& instance,
            const DynamicProgrammingPrimalDualParameters& parameters,
            AlgorithmFormatter& algorithm_formatter,
            DynamicProgrammingPrimalDualOutput& output):
        instance(instance),
        parameters(parameters),
        algorithm_formatter(algorithm_formatter),
        output(output),
        partial_sort(instance),
        partial_solution_factory(
                instance.number_of_items(),
                parameters.partial_solution_size) { }

    /** Instance. */
    const Instance& instance;

    /** Parameters. */
    const DynamicProgrammingPrimalDualParameters& parameters;

    /** Algorithm formatter. */
    AlgorithmFormatter& algorithm_formatter;

    /** Output. */
    DynamicProgrammingPrimalDualOutput& output;

    /** Partial sort. */
    PartialSort partial_sort;

    /** Partial solution factory. */
    optimizationtools::PartialSetFactory partial_solution_factory;

    ItemPos s;

    ItemPos t;

    /**
     * Maximum weight that can still be removed from a state.
     *
     * If the weight of a state is greater than
     *     instance.capacity() + remaining_weight
     * Then, it is infeasible.
     */
    Weight remaining_weight;

    /** Current states. */
    std::vector<DynamicProgrammingPrimalDualState> l0;

    /** States of the next iteration. */
    std::vector<DynamicProgrammingPrimalDualState> l;

    /** Best state. */
    DynamicProgrammingPrimalDualState best_state;
};

void add_item(DynamicProgrammingPrimalDualInternalData& data)
{
    const Instance& instance = data.instance;
    ItemId item_id = data.partial_sort.item_id(data.t);
    const Item& item = instance.item(item_id);
    //std::cout << "add_item"
    //    << " data.t " << data.t
    //    << " item_id " << item_id
    //    << std::endl;

    data.partial_solution_factory.add_element_to_factory(data.t);
    data.best_state.partial_solution = data.partial_solution_factory.remove(
            data.best_state.partial_solution,
            data.t);

    ItemId sx = data.partial_sort.bound_item_left(data.s, data.output.value);
    ItemId tx = data.partial_sort.bound_item_right(data.t + 1, data.output.value);

    Weight maximum_weight = instance.capacity()
        + data.remaining_weight
        - data.partial_sort.mandatory_items().weight();

    data.l.clear();
    std::vector<DynamicProgrammingPrimalDualState>::iterator it = data.l0.begin();
    std::vector<DynamicProgrammingPrimalDualState>::iterator it1 = data.l0.begin();
    Profit upper_bound_it = 0;
    while (it != data.l0.end() || it1 != data.l0.end()) {
        if (it == data.l0.end() || it->weight > it1->weight + item.weight) {

            // Check state weight.
            Weight weight = it1->weight + item.weight;
            if (weight > maximum_weight) {
                it1++;
                continue;
            }

            Profit profit = it1->profit + item.profit;
            if (!data.l.empty()
                    && profit <= data.l.back().profit) {
                it1++;
                continue;
            }

            // Check bound.
            Profit upper_bound_curr = (weight <= instance.capacity())?
                upper_bound(instance, profit, weight, tx):
                upper_bound_reverse(instance, profit, weight, sx);
            upper_bound_it = std::max(upper_bound_it, upper_bound_curr);
            if (upper_bound_curr <= data.output.value) {
                it1++;
                continue;
            }

            DynamicProgrammingPrimalDualState state{
                    weight,
                    profit,
                    data.partial_solution_factory.add(it1->partial_solution, data.t)};

            // Update lower bound
            if (state.weight <= instance.capacity()
                    && state.profit > data.output.value) {
                std::stringstream ss;
                ss << "it " << data.t - data.s << " (value)";
                data.algorithm_formatter.update_value(state.profit, ss.str());
                data.best_state = state;
            }

            if (!data.l.empty() && state.weight == data.l.back().weight) {
                data.l.back() = state;
            } else {
                data.l.push_back(state);
            }
            it1++;

        } else {

            // Check state weight.
            if (it->weight > maximum_weight) {
                it++;
                continue;
            }

            if (!data.l.empty() && it->profit <= data.l.back().profit) {
                it++;
                continue;
            }

            // Check bound.
            Profit upper_bound_curr = (it->weight <= instance.capacity())?
                upper_bound(instance, it->profit, it->weight, tx):
                upper_bound_reverse(instance, it->profit, it->weight, sx);
            upper_bound_it = std::max(upper_bound_it, upper_bound_curr);
            if (upper_bound_curr <= data.output.value) {
                it++;
                continue;
            }

            it->partial_solution = data.partial_solution_factory.remove(it->partial_solution, data.t);
            if (!data.l.empty() && it->weight == data.l.back().weight) {
                data.l.back() = *it;
            } else {
                data.l.push_back(*it);
            }
            ++it;
        }
    }

    upper_bound_it = std::max(upper_bound_it, data.output.value);
    if (data.output.bound > upper_bound_it) {
        std::stringstream ss;
        ss << "it " << data.t - data.s << " (bound)";
        data.algorithm_formatter.update_bound(upper_bound_it, ss.str());
    }

    data.l0.swap(data.l);
}

void remove_item(DynamicProgrammingPrimalDualInternalData& data)
{
    const Instance& instance = data.instance;
    ItemId item_id = data.partial_sort.item_id(data.s);
    const Item& item = instance.item(item_id);
    //std::cout << "remove_item"
    //    << " data.s " << data.s
    //    << " item_id " << item_id
    //    << std::endl;

    data.partial_solution_factory.add_element_to_factory(data.s);
    data.best_state.partial_solution = data.partial_solution_factory.add(
            data.best_state.partial_solution,
            data.s);

    ItemId sx = data.partial_sort.bound_item_left(data.s - 1, data.output.value);
    ItemId tx = data.partial_sort.bound_item_right(data.t, data.output.value);

    data.remaining_weight -= item.weight;
    Weight maximum_weight = instance.capacity()
        + data.remaining_weight
        - data.partial_sort.mandatory_items().weight();

    data.l.clear();
    std::vector<DynamicProgrammingPrimalDualState>::iterator it = data.l0.begin();
    std::vector<DynamicProgrammingPrimalDualState>::iterator it1 = data.l0.begin();
    Profit upper_bound_it = 0;
    while (it != data.l0.end() || it1 != data.l0.end()) {
        if (it1 == data.l0.end() || it->weight <= it1->weight - item.weight) {

            // Check state weight.
            if (it->weight > maximum_weight) {
                it++;
                continue;
            }

            if (!data.l.empty() && it->profit <= data.l.back().profit) {
                it++;
                continue;
            }

            Profit upper_bound_curr = (it->weight <= instance.capacity())?
                upper_bound(instance, it->profit, it->weight, tx):
                upper_bound_reverse(instance, it->profit, it->weight, sx);
            upper_bound_it = std::max(upper_bound_it, upper_bound_curr);
            if (upper_bound_curr <= data.output.value) {
                it++;
                continue;
            }

            it->partial_solution = data.partial_solution_factory.add(it->partial_solution, data.s);
            if (!data.l.empty() && it->weight == data.l.back().weight) {
                data.l.back() = *it;
            } else {
                data.l.push_back(*it);
            }
            ++it;

        } else {

            // Check state weight.
            Weight weight = it1->weight - item.weight;
            if (weight > maximum_weight) {
                it1++;
                continue;
            }

            Profit profit = it1->profit - item.profit;
            if (!data.l.empty()
                    && profit <= data.l.back().profit) {
                it1++;
                continue;
            }

            Profit upper_bound_curr = (weight <= instance.capacity())?
                upper_bound(instance, profit, weight, tx):
                upper_bound_reverse(instance, profit, weight, sx);
            upper_bound_it = std::max(upper_bound_it, upper_bound_curr);
            if (upper_bound_curr <= data.output.value) {
                it1++;
                continue;
            }

            DynamicProgrammingPrimalDualState state{
                    weight,
                    profit,
                    data.partial_solution_factory.remove(it1->partial_solution, data.s)};

            // Update lower bound
            if (state.weight <= instance.capacity()
                    && state.profit > data.output.value) {
                if (data.output.number_of_recursive_calls == 1) {
                    std::stringstream ss;
                    ss << "it " << data.t - data.s << " (value)";
                    data.algorithm_formatter.update_value(state.profit, ss.str());
                }
                data.best_state = state;
            }

            if (!data.l.empty() && state.weight == data.l.back().weight) {
                data.l.back() = state;
            } else {
                data.l.push_back(state);
            }
            it1++;
        }
    }

    upper_bound_it = std::max(upper_bound_it, data.output.value);
    if (data.output.bound > upper_bound_it) {
        std::stringstream ss;
        ss << "it " << data.t - data.s << " (bound)";
        data.algorithm_formatter.update_bound(upper_bound_it, ss.str());
    }

    data.l0.swap(data.l);
}

ItemPos dynamic_programming_primal_dual_find_state(
        DynamicProgrammingPrimalDualInternalData& data,
        bool right)
{
    const Instance& instance = data.instance;

    Profit lb0 = 0;
    ItemPos item_pos_best = -1;
    ItemPos first = (right)? data.t: 0;
    ItemPos last = (right)? instance.number_of_items() - 1: data.s;
    for (ItemPos item_pos = first; item_pos <= last; ++item_pos) {
        ItemId item_id = data.partial_sort.item_id(item_pos);
        const Item& item = instance.item(item_id);
        if (data.partial_sort.first_reduced_item_pos() <= item_pos
                && item_pos < data.partial_sort.first_sorted_item_pos())
            continue;
        if (data.partial_sort.last_sorted_item_pos() < item_pos
                && item_pos <= data.partial_sort.last_reduced_item_pos())
            continue;
        Weight weight = (right)?
            instance.capacity() - item.weight:
            instance.capacity() + item.weight;
        if (data.l0.front().weight > weight)
            continue;
        ItemPos f = 0;
        ItemPos l = data.l0.size() - 1; // l0_[l] > w
        while (f + 1 < l) {
            ItemPos m = (f + l) / 2;
            if (data.l0[m].weight >= weight) {
                l = m;
            } else {
                f = m;
            }
        }
        if (f != (StateId)data.l0.size() - 1
                && data.l0[f + 1].weight <= weight) {
            f++;
        }
        Profit lb = (right)?
            data.l0[f].profit + item.profit:
            data.l0[f].profit - item.profit;
        if (lb0 < lb) {
            item_pos_best = item_pos;
            lb0 = lb;
        }
    }
    return item_pos_best;
}

}

const DynamicProgrammingPrimalDualOutput knapsacksolver::dynamic_programming_primal_dual(
        const Instance& instance,
        const DynamicProgrammingPrimalDualParameters& parameters)
{
    DynamicProgrammingPrimalDualOutput output(instance);
    AlgorithmFormatter algorithm_formatter(parameters, output);
    algorithm_formatter.start("Dynamic programming - primal-dual - partial");
    algorithm_formatter.print_header();

    // Check overflow because of the bounds.
    //    bound = profit / weight * capacity
    // And we want:
    //    bound < INT_MAX
    // Therefore:
    //    profit < INT_MAX / capacity * weight
    for (ItemId item_id = 0;
            item_id < instance.number_of_items();
            ++item_id) {
        const Item& item = instance.item(item_id);
        double profit_max = std::numeric_limits<Profit>::max() / instance.capacity() * item.weight;
        if (item.profit > profit_max) {
            throw std::invalid_argument("Profit too high");
        }
    }

    output.number_of_recursive_calls++;

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

    DynamicProgrammingPrimalDualInternalData data(instance, parameters, algorithm_formatter, output);

    // Get the greedy solution.
    GreedyParameters greedy_parameters;
    greedy_parameters.timer = parameters.timer;
    greedy_parameters.verbosity_level = 0;
    greedy_parameters.partial_sort = &data.partial_sort;
    auto greedy_output = greedy(instance, greedy_parameters);

    // Update solution.
    algorithm_formatter.update_solution(
            greedy_output.solution,
            "greedy");

    // Compute an initial bound.
    UpperBoundDantzigParameters upper_bound_dantzig_parameters;
    upper_bound_dantzig_parameters.timer = parameters.timer;
    upper_bound_dantzig_parameters.verbosity_level = 0;
    upper_bound_dantzig_parameters.partial_sort = &data.partial_sort;
    auto upper_bound_output = upper_bound_dantzig(
            instance,
            upper_bound_dantzig_parameters);

    // Update bound.
    algorithm_formatter.update_bound(
            upper_bound_output.bound,
            "dantzig upper bound");

    // Recursion.
    Weight w_bar = data.partial_sort.break_solution().weight();
    Profit p_bar = data.partial_sort.break_solution().profit();
    data.l0 = {{w_bar, p_bar, 0}};
    data.s = data.partial_sort.break_item_pos() - 1;
    data.t = data.partial_sort.break_item_pos();
    data.remaining_weight = w_bar;
    data.best_state = data.l0.front();
    StateId next_pairing = 1e4;
    while (!data.l0.empty()
            && (
                !data.partial_sort.is_intervals_left_empty()
                || data.s >= data.partial_sort.first_sorted_item_pos()
                || !data.partial_sort.is_intervals_right_empty()
                || data.t <= data.partial_sort.last_sorted_item_pos())) {

        // Check end.
        if (parameters.timer.needs_to_end()) {
            algorithm_formatter.end();
            return output;
        }

        if (output.value == output.bound)
            break;

        if (parameters.pairing
                && next_pairing <= (StateId)data.l0.size()) {
            next_pairing *= 10;

            if (!data.partial_sort.is_intervals_right_empty()
                    || data.t <= data.partial_sort.last_sorted_item_pos()) {
                ItemPos item_pos = dynamic_programming_primal_dual_find_state(data, true);
                if (item_pos != -1) {
                    data.partial_sort.move_item_to_core(item_pos, data.t);
                    add_item(data);
                    ++data.t;

                    // Check end.
                    if (parameters.timer.needs_to_end()) {
                        algorithm_formatter.end();
                        return output;
                    }

                    if (output.value == output.bound)
                        break;
                }
            }

            if (!data.partial_sort.is_intervals_left_empty()
                    || data.s >= data.partial_sort.first_sorted_item_pos()) {
                ItemPos item_pos = dynamic_programming_primal_dual_find_state(data, false);
                if (item_pos != -1) {
                    data.partial_sort.move_item_to_core(item_pos, data.s);

                    remove_item(data);
                    --data.s;

                    // Check end.
                    if (parameters.timer.needs_to_end()) {
                        algorithm_formatter.end();
                        return output;
                    }

                    if (output.value == output.bound)
                        break;
                }
            }
        }

        if (!data.partial_sort.is_intervals_right_empty()
                || data.t <= data.partial_sort.last_sorted_item_pos()) {
            add_item(data);
            data.t++;

            // Check end.
            if (parameters.timer.needs_to_end()) {
                algorithm_formatter.end();
                return output;
            }

            if (output.value == output.bound)
                break;
        }

        if (!data.partial_sort.is_intervals_left_empty()
                || data.s >= data.partial_sort.first_sorted_item_pos()) {
            remove_item(data);
            data.s--;

            // Check end.
            if (parameters.timer.needs_to_end()) {
                algorithm_formatter.end();
                return output;
            }

            if (output.value == output.bound)
                break;
        }
    }

    // Update bound.
    algorithm_formatter.update_bound(
            output.value,
            "algorithm end (bound)");

    if (output.solution.profit() == output.bound) {
        algorithm_formatter.end();
        return output;
    }

    Solution solution(instance);
    // Add mandatory items from the partial sort.
    for (ItemPos item_pos = 0;
            item_pos < data.partial_sort.first_sorted_item_pos();
            ++item_pos) {
        ItemId item_id = data.partial_sort.item_id(item_pos);
        solution.add(item_id);
    }
    for (ItemPos item_pos = data.partial_sort.first_sorted_item_pos();
            item_pos <= data.s;
            ++item_pos) {
        ItemId item_id = data.partial_sort.item_id(item_pos);
        solution.add(item_id);
    }
    // Add items from the partial solution of the best state.
    for (ItemPos item_pos = data.partial_sort.first_sorted_item_pos();
            item_pos <= data.partial_sort.last_sorted_item_pos();
            ++item_pos) {
        ItemId item_id = data.partial_sort.item_id(item_pos);
        if (data.partial_solution_factory.contains(
                    data.best_state.partial_solution,
                    item_pos)) {
            solution.add(item_id);
        }
    }
    if (!solution.feasible()) {
        throw std::runtime_error("Solution should be feasible here.");
    }

    if (solution.profit() == output.value) {
        // Update solution.
        algorithm_formatter.update_solution(
                solution,
                "algorithm end (solution)");

        algorithm_formatter.end();
        return output;
    }

    // Build new instance.
    InstanceBuilder sub_instance_builder;
    std::vector<ItemId> sub2original;
    Weight sub_capacity = data.best_state.weight - solution.weight();
    sub_instance_builder.set_capacity(sub_capacity);
    for (ItemPos item_pos = data.s + 1;
            item_pos <= data.partial_sort.last_sorted_item_pos();
            ++item_pos) {
        ItemId item_id = data.partial_sort.item_id(item_pos);
        const Item& item = instance.item(item_id);

        // Don't add the item if it was in the partial solutions.
        if (data.partial_solution_factory.contains(item_pos))
            continue;

        // Don't add too large items.
        if (item.weight > sub_capacity)
            continue;

        sub_instance_builder.add_item(item.profit, item.weight);
        sub2original.push_back(item_id);
    }
    Instance sub_instance = sub_instance_builder.build();

    // Solve sub-instance.
    DynamicProgrammingPrimalDualParameters sub_parameters;
    sub_parameters.timer = parameters.timer;
    sub_parameters.verbosity_level = 0;
    auto sub_output = dynamic_programming_primal_dual(sub_instance, sub_parameters);
    // Check end.
    if (parameters.timer.needs_to_end()) {
        algorithm_formatter.end();
        return output;
    }

    output.number_of_recursive_calls += sub_output.number_of_recursive_calls;

    // Check the number of recursive calls.
    if (instance.number_of_items() <= parameters.partial_solution_size
            && output.number_of_recursive_calls > 1) {
        throw std::runtime_error("Too many recursive calls.");
    }

    // Add items from the sub-instance solution.
    for (ItemId item_id = 0;
            item_id < sub_instance.number_of_items();
            ++item_id) {
        if (sub_output.solution.contains(item_id))
            solution.add(sub2original[item_id]);
    }
    if (solution.profit() != output.value) {
        throw std::runtime_error("Wrong final solution.");
    }

    // Update solution.
    algorithm_formatter.update_solution(
            solution,
            "algorithm end (solution)");

    algorithm_formatter.end();
    return output;
}
