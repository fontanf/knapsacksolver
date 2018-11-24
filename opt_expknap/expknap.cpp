#include "knapsack/opt_expknap/expknap.hpp"

#include "knapsack/lb_greedy/greedy.hpp"
#include "knapsack/lb_greedynlogn/greedynlogn.hpp"
#include "knapsack/ub_dembo/dembo.hpp"
#include "knapsack/ub_dantzig/dantzig.hpp"
#include "knapsack/ub_surrogate/surrogate.hpp"

using namespace knapsack;

void update_bounds(const Instance& ins, Solution& sol_best, Profit& ub, SurrogateOut& so,
        ExpknapParams& params, StateIdx nodes, Info& info)
{
    if (params.ub_surrogate == nodes) {
        Info info_tmp;
        so = ub_surrogate(ins, sol_best.profit(), info_tmp);
        if (ub > so.ub) {
            ub = so.ub;
        } else {
        }
    }

    if (params.solve_sur == nodes) {
        if (sol_best.profit() == ub)
            return;
        assert(params.ub_surrogate >= 0 && params.ub_surrogate <= params.solve_sur);
        Instance ins_sur(ins);
        ins_sur.surrogate(so.multiplier, so.bound);
        params.ub_surrogate = -1;
        params.solve_sur = -1;
        Solution sol_sur = sopt_expknap(ins_sur, info, params);
        if (ub > sol_sur.profit()) {
            ub = sol_sur.profit();
        }
        if (sol_sur.item_number() == so.bound) {
            sol_best = sol_sur;
        }
    }

    if (params.lb_greedynlogn == nodes) {
        Info info_tmp;
        if (sol_best.update(sol_bestgreedynlogn(ins, info_tmp))) {
        } else {
        }
    }
}

bool sopt_expknap_rec(Instance& ins,
        Solution& sol_curr, Solution& sol_best, Profit& u, SurrogateOut& so,
        ItemPos s, ItemPos t,
        ExpknapParams& params, StateIdx& nodes, Info& info)
{
    nodes++; // Increment node number
    update_bounds(ins, sol_best, u, so, params, nodes, info); // Update bounds

    bool improved = false;
    if (sol_curr.remaining_capacity() >= 0) {
        if (sol_best.update(sol_curr)) {
            improved = true;
        }
        for (;;t++) {
            // If UB reached, then stop
            if (sol_best.profit() == u)
                return false;

            // Expand
            if (ins.int_right_size() > 0 && t > ins.last_sorted_item())
                ins.sort_right(sol_best.profit());

            // Bounding test
            Profit ub = ub_dembo(ins, t, sol_curr);
            if (ub <= sol_best.profit())
                return improved;

            // Recursive call
            sol_curr.set(t, true); // Add item t
            if (sopt_expknap_rec(ins, sol_curr, sol_best, u, so, s, t+1, params, nodes, info))
                improved = true;
            sol_curr.set(t, false); // Remove item t
        }
    } else {
        for (;;s--) {
            // If UB reached, then stop
            if (sol_best.profit() == u)
                return false;

            // Expand
            if (ins.int_left_size() > 0 && s < ins.first_sorted_item())
                ins.sort_left(sol_best.profit());

            // Bounding test
            Profit ub = ub_dembo_rev(ins, s, sol_curr);
            if (ub <= sol_best.profit())
                return improved;

            // Recursive call
            sol_curr.set(s, false); // Remove item s
            if (sopt_expknap_rec(ins, sol_curr, sol_best, u, so, s-1, t, params, nodes, info))
                improved = true;
            sol_curr.set(s, true); // Add item s
        }
    }
    assert(false);
    return improved;
}

Solution knapsack::sopt_expknap(Instance& ins, Info& info, ExpknapParams params)
{
    info.verbose("*** expknap ***\n");

    if (ins.item_number() == 0) {
        DBG(info.debug("Empty instance.\n");)
        Solution sol(ins);
        return algorithm_end(sol, info);
    }

    ins.sort_partially();
    if (ins.break_item() == ins.last_item()+1) {
        DBG(info.debug("All items fit in the knapsack.\n");)
        Solution sol = *ins.break_solution();
        return algorithm_end(sol, info);
    }

    Solution sol_curr = *ins.break_solution();

    Info info_tmp1;
    Solution sol_best = sol_greedy(ins, info_tmp1);

    Info info_tmp2;
    Profit ub = ub_dantzig(ins, info_tmp2);

    info.verbose(
            "lb " + std::to_string(sol_best.profit()) +
            " ub " + std::to_string(ub) +
            " gap " + std::to_string(ub - sol_best.profit()) +
            "\n");

    ItemPos b = ins.break_item();

    SurrogateOut so;
    StateIdx node_number = 0;
    update_bounds(ins, sol_best, ub, so, params, node_number, info); // Update bounds
    if (sol_best.profit() != ub) { // If UB reached, then stop
        info.verbose("Branch...\n");
        sopt_expknap_rec(ins, sol_curr, sol_best, ub, so, b-1, b, params, node_number, info);
    }

    info.pt.put("Algorithm.NodeNumber", node_number);
    info.verbose("Node number: " + Info::to_string(node_number) + "\n");
    return algorithm_end(sol_best, info);
}

