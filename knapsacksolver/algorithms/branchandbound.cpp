#include "knapsacksolver/algorithms/branchandbound.hpp"

#include "knapsacksolver/algorithms/dembo.hpp"
#include "knapsacksolver/algorithms/dantzig.hpp"
#include "knapsacksolver/algorithms/greedynlogn.hpp"

using namespace knapsacksolver;

struct BranchAndBoundData
{
    const Instance& instance;
    Solution sol_curr;
    ItemPos j;
    Output& output;
    bool sort;
    ItemPos j_max;
    std::vector<Weight> min_weight;
    Counter number_of_node;
    Info& info;
};

void branchandbound_rec(BranchAndBoundData& d)
{
    d.number_of_node++; // Increment node number

    if (!d.info.check_time()) // Check time
        return;

    if (d.output.lower_bound < d.sol_curr.profit()) {
        std::stringstream ss;
        ss << "node " << d.number_of_node;
        d.output.update_solution(d.sol_curr, ss, d.info);
    }

    for (ItemPos j = d.j; j <= d.instance.last_item(); j++) {
        if (d.instance.item(j).w > d.sol_curr.remaining_capacity())
            continue;
        if (d.min_weight[j] > d.sol_curr.remaining_capacity())
            return;

        if (!d.sort) { // Bounding test
            if (ub_0(d.instance, j, d.sol_curr, d.j_max) <= d.output.lower_bound)
                continue;
        } else {
            if (ub_dembo(d.instance, j, d.sol_curr) <= d.output.lower_bound)
                return;
        }

        // Recursive call
        assert(j <= d.instance.last_item());
        d.sol_curr.set(j, true);
        d.j = j + 1;
        branchandbound_rec(d);
        d.sol_curr.set(j, false);
    }
}

Output knapsacksolver::branchandbound(Instance& instance, bool sort, Info info)
{
    FFOT_VER(info, "*** branchandbound" << ((sort)? " (sort)": "") << " ***" << std::endl);
    Output output(instance, info);

    ItemIdx n = instance.reduced_number_of_items();
    if (n == 0) {
        output.update_upper_bound(0, std::stringstream("no item"), info);
        FFOT_LOG_FOLD_END(info, "no item");
        return output.algorithm_end(info);
    }

    ItemPos j_max = -1;
    if (sort) {
        instance.sort(FFOT_DBG(info));
        if (instance.break_item() == instance.last_item() + 1) {
            if (output.lower_bound < instance.break_solution()->profit()) {
                output.update_solution(
                        *instance.break_solution(),
                        std::stringstream("all items fit"),
                        info);
            }
            output.update_upper_bound(
                    instance.break_solution()->profit(),
                    std::stringstream(""),
                    info);
            FFOT_LOG_FOLD_END(info, "all items fit in the knapsack");
            return output.algorithm_end(info);
        }
        auto g_output = greedynlogn(instance);
        if (output.lower_bound < g_output.lower_bound) {
            output.update_solution(
                    g_output.solution,
                    std::stringstream("greedynlogn"),
                    info);
        }

        instance.reduce2(output.lower_bound, info);
        if (instance.reduced_capacity() < 0) {
            output.update_upper_bound(
                    output.lower_bound,
                    std::stringstream("negative capacity after reduction"),
                    info);
            FFOT_LOG_FOLD_END(info, "c < 0");
            return output.algorithm_end(info);
        } else if (n == 0 || instance.reduced_capacity() == 0) {
            if (output.lower_bound < instance.reduced_solution()->profit()) {
                output.update_solution(
                        *instance.reduced_solution(),
                        std::stringstream("no item or null capacity after reduction"),
                        info);
            }
            output.update_upper_bound(
                    output.lower_bound,
                    std::stringstream(""),
                    info);
            FFOT_LOG_FOLD_END(info, "no item or null capacity after reduction");
            return output.algorithm_end(info);
        } else if (instance.break_item() == instance.last_item() + 1) {
            if (output.lower_bound < instance.break_solution()->profit()) {
                output.update_solution(
                        *instance.break_solution(),
                        std::stringstream("all items fit in the knapsack after reduction"),
                        info);
            }
            output.update_upper_bound(
                    output.lower_bound,
                    std::stringstream(""),
                    info);
            FFOT_LOG_FOLD_END(info, "all items fit in the knapsack after reduction");
            return output.algorithm_end(info);
        }
    } else {
        j_max = instance.max_efficiency_item(FFOT_DBG(info));
    }

    Profit ub = (!sort)? ub_0(instance, 0, 0, instance.capacity(), j_max):
        std::max(ub_dantzig(instance), output.lower_bound);
    output.update_upper_bound(
            ub,
            std::stringstream("initial upper bound"),
            info);

    BranchAndBoundData d {
        .instance = instance,
        .sol_curr = (instance.reduced_solution() == NULL)? Solution(instance): *instance.reduced_solution(),
        .j = instance.first_item(),
        .output = output,
        .sort = sort,
        .j_max = j_max,
        .min_weight = instance.min_weights(),
        .number_of_node = 0,
        .info = info,
    };
    branchandbound_rec(d);
    if (info.check_time() && output.upper_bound > output.lower_bound) {
        output.update_upper_bound(
                output.lower_bound,
                std::stringstream("tree search completed"),
                info);
    }

    FFOT_LOG_FOLD_END(info, "");
    return output.algorithm_end(info);
}

