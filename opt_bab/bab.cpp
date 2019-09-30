#include "knapsack/opt_bab/bab.hpp"

#include "knapsack/ub_dembo/dembo.hpp"
#include "knapsack/ub_dantzig/dantzig.hpp"
#include "knapsack/lb_greedynlogn/greedynlogn.hpp"

using namespace knapsack;

struct BabData
{
    const Instance& ins;
    Solution sol_curr;
    ItemPos j;
    knapsack::Output& output;
    bool sort;
    ItemPos j_max;
    std::vector<Weight> min_weight;
    Cpt node_number;
    Info& info;
};

void bab_rec(BabData& d)
{
    d.node_number++; // Increment node number

    if (!d.info.check_time()) // Check time
        return;

    if (d.output.lower_bound < d.sol_curr.profit()) {
        std::stringstream ss;
        ss << "node " << d.node_number;
        update_sol(d.output, d.sol_curr, ss, d.info);
    }

    for (ItemPos j=d.j; j<=d.ins.last_item(); j++) {
        if (d.ins.item(j).w > d.sol_curr.remaining_capacity())
            continue;
        if (d.min_weight[j] > d.sol_curr.remaining_capacity())
            return;

        if (!d.sort) { // Bounding test
            if (ub_0(d.ins, j, d.sol_curr, d.j_max) <= d.output.lower_bound)
                continue;
        } else {
            if (ub_dembo(d.ins, j, d.sol_curr) <= d.output.lower_bound)
                return;
        }

        // Recursive call
        assert(j <= d.ins.last_item());
        d.sol_curr.set(j, true);
        d.j = j + 1;
        bab_rec(d);
        d.sol_curr.set(j, false);
    }
}

knapsack::Output knapsack::sopt_bab(Instance& ins, bool sort, Info info)
{
    VER(info, "*** bab" << ((sort)? " (sort)": "") << " ***" << std::endl);
    knapsack::Output output(ins);
    init_display(output.lower_bound, output.upper_bound, info);

    ItemIdx n = ins.item_number();
    if (n == 0) {
        update_ub(output, 0, std::stringstream("no item"), info);
        algorithm_end(output, info);
        LOG_FOLD_END(info, "no item");
        return output;
    }

    ItemPos j_max = -1;
    if (sort) {
        ins.sort(info);
        if (ins.break_item() == ins.last_item() + 1) {
            if (output.lower_bound < ins.break_solution()->profit())
                update_sol(output, *ins.break_solution(), std::stringstream("all items fit"), info);
            update_ub(output, ins.break_solution()->profit(), std::stringstream(""), info);
            algorithm_end(output, info);
            LOG_FOLD_END(info, "all items fit in the knapsack");
            return output;
        }
        auto g_output = sol_greedynlogn(ins);
        if (output.lower_bound < g_output.lower_bound)
            update_sol(output, g_output.solution, std::stringstream("greedynlogn"), info);

        ins.reduce2(output.lower_bound, info);
        if (ins.capacity() < 0) {
            update_ub(output, output.lower_bound, std::stringstream("negative capacity after reduction"), info);
            algorithm_end(output, info);
            LOG_FOLD_END(info, "c < 0");
            return output;
        } else if (n == 0 || ins.capacity() == 0) {
            if (output.lower_bound < ins.reduced_solution()->profit())
                update_sol(output, *ins.reduced_solution(), std::stringstream("no item or null capacity after reduction"), info);
            update_ub(output, output.lower_bound, std::stringstream(""), info);
            algorithm_end(output, info);
            LOG_FOLD_END(info, "no item or null capacity after reduction");
            return output;
        } else if (ins.break_item() == ins.last_item() + 1) {
            if (output.lower_bound < ins.break_solution()->profit())
                update_sol(output, *ins.break_solution(), std::stringstream("all items fit in the knapsack after reduction"), info);
            update_ub(output, output.lower_bound, std::stringstream(""), info);
            algorithm_end(output, info);
            LOG_FOLD_END(info, "all items fit in the knapsack after reduction");
            return output;
        }
    } else {
        j_max = ins.max_efficiency_item(info);
    }

    Profit ub = (!sort)? ub_0(ins, 0, 0, ins.total_capacity(), j_max): ub_dantzig(ins);
    update_ub(output, ub, std::stringstream("initial upper bound"), info);

    BabData d {
        .ins = ins,
        .sol_curr = (ins.reduced_solution() == NULL)? Solution(ins): *ins.reduced_solution(),
        .j = ins.first_item(),
        .output = output,
        .sort = sort,
        .j_max = j_max,
        .min_weight = ins.min_weights(),
        .node_number = 0,
        .info = info,
    };
    bab_rec(d);
    if (info.check_time() && output.upper_bound > output.lower_bound)
        update_ub(output, output.lower_bound, std::stringstream("tree search completed"), info);

    algorithm_end(output, info);
    LOG_FOLD_END(info, "");
    return output;
}

