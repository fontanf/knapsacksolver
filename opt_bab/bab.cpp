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
    Solution& sol_best;
    Profit& ub;
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

    if (d.sol_best.profit() < d.sol_curr.profit()) {
        std::stringstream ss;
        ss << "node " << d.node_number;
        update_sol(d.sol_best, d.ub, d.sol_curr, ss, d.info);
    }

    for (ItemPos j=d.j; j<=d.ins.last_item(); j++) {
        if (d.ins.item(j).w > d.sol_curr.remaining_capacity())
            continue;
        if (d.min_weight[j] > d.sol_curr.remaining_capacity())
            return;

        if (!d.sort) { // Bounding test
            if (ub_0(d.ins, j, d.sol_curr, d.j_max) <= d.sol_best.profit())
                continue;
        } else {
            if (ub_dembo(d.ins, j, d.sol_curr) <= d.sol_best.profit())
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

Solution knapsack::sopt_bab(Instance& ins, bool sort, Info info)
{
    VER(info, "*** bab ***" << std::endl);

    ItemIdx n = ins.item_number();
    Solution sol(ins);
    if (n == 0)
        return algorithm_end(sol, info);

    ItemPos j_max = -1;
    if (sort) {
        ins.sort(info);
        if (ins.break_item() == ins.last_item() + 1) {
            LOG_FOLD_END(info, "all items fit in the knapsack");
            return algorithm_end(*ins.break_solution(), info);
        }
        sol = sol_greedynlogn(ins);
        ins.reduce2(sol.profit(), info);
        if (ins.capacity() < 0) {
            LOG_FOLD_END(info, "c < 0");
            return algorithm_end(sol, info);
        } else if (n == 0 || ins.capacity() == 0) {
            LOG_FOLD_END(info, "no item or null capacity after reduction");
            if (sol.profit() < ins.reduced_solution()->profit()) {
                return algorithm_end(*ins.reduced_solution(), info);
            } else {
                return algorithm_end(sol, info);
            }
        } else if (ins.break_item() == ins.last_item() + 1) {
            LOG_FOLD_END(info, "all items fit in the knapsack after reduction");
            if (sol.profit() < ins.break_solution()->profit()) {
                return algorithm_end(*ins.break_solution(), info);
            } else {
                return algorithm_end(sol, info);
            }
        }
    } else {
        j_max = ins.max_efficiency_item(info);
    }

    Profit ub = (!sort)? ub_0(ins, 0, 0, ins.total_capacity(), j_max): ub_dantzig(ins);
    init_display(sol.profit(), ub, info);

    BabData d {
        .ins = ins,
        .sol_curr = (ins.reduced_solution() == NULL)? Solution(ins): *ins.reduced_solution(),
        .j = ins.first_item(),
        .sol_best = sol,
        .ub = ub,
        .sort = sort,
        .j_max = j_max,
        .min_weight = ins.min_weights(),
        .node_number = 0,
        .info = info,
    };
    bab_rec(d);

    LOG_FOLD_END(info, "");
    return algorithm_end(sol, info);
}

