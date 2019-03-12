#include "knapsack/opt_expknap/expknap.hpp"

#include "knapsack/lb_greedy/greedy.hpp"
#include "knapsack/lb_greedynlogn/greedynlogn.hpp"
#include "knapsack/ub_dembo/dembo.hpp"
#include "knapsack/ub_dantzig/dantzig.hpp"
#include "knapsack/ub_surrogate/surrogate.hpp"

using namespace knapsack;

struct ExpknapRecData
{
    ExpknapRecData(Instance& ins, Info& info, ExpknapParams p):
        ins(ins), sol_curr(*ins.break_solution()), sol_best(ins), info(info), params(p) {  }
    Instance& ins;
    Solution sol_curr;
    Solution sol_best;
    Info& info;
    Profit u = -1;
    SurrogateOut so;
    ItemPos s;
    ItemPos t;
    ExpknapParams params;
    Cpt node_number = 0;
    Cpt sol_number = 0;
};

void update_bounds(ExpknapRecData& data)
{
    if (data.params.ub_surrogate == data.node_number) {
        Info info_tmp(data.info.logger);
        data.so = ub_surrogate(data.ins, data.sol_best.profit(), info_tmp);
        if (data.u > data.so.ub) {
            data.u = data.so.ub;
        } else {
        }
    }

    if (data.params.solve_sur == data.node_number) {
        if (data.sol_best.profit() == data.u)
            return;
        assert(data.params.ub_surrogate >= 0);
        assert(data.params.ub_surrogate <= data.params.solve_sur);
        Instance ins_sur(data.ins);
        ins_sur.surrogate(data.info, data.so.multiplier, data.so.bound);
        data.params.ub_surrogate = -1;
        data.params.solve_sur = -1;
        Info info_tmp(data.info.logger);
        Solution sol_sur = sopt_expknap(ins_sur, info_tmp, data.params);
        if (data.u > sol_sur.profit()) {
            data.u = sol_sur.profit();
        }
        if (sol_sur.item_number() == data.so.bound) {
            data.sol_best = sol_sur;
        }
    }

    if (data.params.lb_greedynlogn == data.node_number) {
        Info info_tmp(data.info.logger);
        Solution sol_tmp = sol_greedynlogn(data.ins, info_tmp);
        if (sol_tmp.profit() > data.sol_best.profit()) {
            data.sol_best = sol_tmp;
        } else {
        }
    }
}

void sopt_expknap_rec(ExpknapRecData& data)
{
    data.node_number++; // Increment node number
    LOG(data.info, "Node number " << data.node_number << " s " << data.s << " t " << data.t << std::endl);

    update_bounds(data); // Update bounds

    ItemPos s = data.s;
    ItemPos t = data.t;
    if (data.sol_curr.remaining_capacity() >= 0) {
        if (data.sol_curr.profit() > data.sol_best.profit())
            data.sol_best = data.sol_curr;
        for (;;t++) {
            // If UB reached, then stop
            if (data.sol_best.profit() == data.u)
                return;

            // Expand
            if (data.ins.int_right_size() > 0 && t > data.ins.last_sorted_item())
                data.ins.sort_right(data.sol_best.profit());

            // Bounding test
            Profit ub = ub_dembo(data.ins, t, data.sol_curr);
            if (ub <= data.sol_best.profit())
                return;

            // Recursive call
            data.sol_curr.set(t, true); // Add item t
            data.s = s;
            data.t = t+1;
            sopt_expknap_rec(data);
            data.sol_curr.set(t, false); // Remove item t
        }
    } else {
        for (;;s--) {
            // If UB reached, then stop
            if (data.sol_best.profit() == data.u)
                return;

            // Expand
            if (data.ins.int_left_size() > 0 && s < data.ins.first_sorted_item())
                data.ins.sort_left(data.sol_best.profit());

            // Bounding test
            Profit ub = ub_dembo_rev(data.ins, s, data.sol_curr);
            if (ub <= data.sol_best.profit())
                return;

            // Recursive call
            data.sol_curr.set(s, false); // Remove item s
            data.s = s-1;
            data.t = t;
            sopt_expknap_rec(data);
            data.sol_curr.set(s, true); // Add item s
        }
    }
    assert(false);
}

Solution knapsack::sopt_expknap(Instance& ins, Info& info, ExpknapParams params)
{
    VER(info, "*** expknap ***" << std::endl);

    if (ins.item_number() == 0) {
        LOG(info, "Empty instance." << std::endl);
        Solution sol(ins);
        return algorithm_end(sol, info);
    }

    ins.sort_partially(info);
    if (ins.break_item() == ins.last_item()+1) {
        LOG(info, "All items fit in the knapsack." << std::endl);
        Solution sol = *ins.break_solution();
        return algorithm_end(sol, info);
    }

    ExpknapRecData data(ins, info, params);

    Info info_tmp1(info.logger);
    data.sol_best = sol_greedy(ins, info_tmp1);
    Info info_tmp2(info.logger);
    data.u = ub_dantzig(ins, info_tmp2);

    VER(info, "lb " << data.sol_best.profit()
        << " ub " << data.u
        << " gap " << data.u - data.sol_best.profit() << std::endl);

    ItemPos b = ins.break_item();

    update_bounds(data); // Update bounds
    if (data.sol_best.profit() != data.u) { // If UB reached, then stop
        VER(info, "Branch..." << std::endl);
        data.s = b-1;
        data.t = b;
        sopt_expknap_rec(data);
    }

    PUT(info, "Algorithm.NodeNumber", data.node_number);
    VER(info, "Node number: " << data.node_number << std::endl;)
    return algorithm_end(data.sol_best, info);
}

