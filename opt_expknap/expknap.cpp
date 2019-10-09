#include "knapsack/opt_expknap/expknap.hpp"

#include "knapsack/lb_greedy/greedy.hpp"
#include "knapsack/lb_greedynlogn/greedynlogn.hpp"
#include "knapsack/ub_dembo/dembo.hpp"
#include "knapsack/ub_dantzig/dantzig.hpp"

using namespace knapsack;

struct ExpknapInternalData
{
    ExpknapInternalData(Instance& ins, ExpknapOptionalParameters& p, ExpknapOutput& output):
        ins(ins), p(p), output(output), sol_curr(*ins.break_solution()) { }
    Instance& ins;
    ExpknapOptionalParameters& p;
    ExpknapOutput& output;
    Solution sol_curr;
    std::vector<std::thread> threads;
};

void sopt_expknap_update_bounds(ExpknapInternalData& d)
{
    if (d.p.surrogate >= 0 && d.p.surrogate <= d.output.node_number) {
        d.p.surrogate = -1;
        std::function<Output (Instance&, Info, bool*)> func
            = [&d](Instance& ins, Info info, bool* end)
            {
                ExpknapOptionalParameters p;
                p.info = info;
                p.greedy = d.p.greedy;
                p.greedynlogn = d.p.greedynlogn;
                p.surrogate = -1;
                p.combo_core = d.p.combo_core;
                p.end = end;
                p.stop_if_end = true;
                p.set_end = false;
                return sopt_expknap(ins, p);
            };
        d.threads.push_back(std::thread(ub_solvesurrelax, SurrelaxData{
                    .ins      = Instance::reset(d.ins),
                    .output   = d.output,
                    .func     = func,
                    .end      = d.p.end,
                    .info     = Info(d.p.info, true, "surrelax")}));
    }
    if (d.p.greedynlogn >= 0 && d.p.greedynlogn <= d.output.node_number) {
        d.p.greedynlogn = -1;
        auto gn_output = sol_greedynlogn(d.ins);
        d.output.update_sol(gn_output.solution, std::stringstream("greedynlogn"), d.p.info);
    }
}

void sopt_expknap_rec(ExpknapInternalData& d, ItemPos s, ItemPos t)
{
    Info& info = d.p.info;
    d.output.node_number++; // Increment node number
    LOG_FOLD_START(info, "node number " << d.output.node_number
            << " s " << s << " t " << t
            << " w " << d.sol_curr.weight() << " p " << d.sol_curr.profit()
            << std::endl);

    // Check end
    if (d.p.stop_if_end && *(d.p.end)) {
        LOG_FOLD_END(info, "end");
        return;
    }

    // Check time
    if (!info.check_time()) {
        if (d.p.set_end)
            *(d.p.end) = true;
        for (std::thread& thread: d.threads)
            thread.join();
        d.threads.clear();
        LOG_FOLD_END(info, "time");
        return;
    }

    // If UB reached, then stop
    if (d.output.solution.profit() == d.output.upper_bound) {
        LOG_FOLD_END(info, "lb == ub");
        return;
    }

    // Update bounds
    sopt_expknap_update_bounds(d);

    if (d.sol_curr.remaining_capacity() >= 0) {
        // Update best solution
        if (d.output.solution.profit() < d.sol_curr.profit()) {
            std::stringstream ss;
            ss << "node " << d.output.node_number;
            d.output.update_sol(d.sol_curr, ss, info);
        }

        for (;;t++) {
            // Bounding test
            Profit ub = ub_dembo(d.ins, d.ins.bound_item_right(t, d.output.solution.profit(), info), d.sol_curr);
            LOG(info, "t " << t << " ub " << ub << " lb " << d.output.solution.profit());
            if (ub <= d.output.solution.profit()) {
                LOG_FOLD_END(info, " bound");
                return;
            }

            // Recursive call
            assert(t <= d.ins.last_item());
            LOG(info, " add (" << d.ins.item(t) << ")" << std::endl);
            d.sol_curr.set(t, true); // Add item t
            sopt_expknap_rec(d, s, t + 1);
            d.sol_curr.set(t, false); // Remove item t
        }
    } else {
        for (;;s--) {
            // Bounding test
            Profit ub = ub_dembo_rev(d.ins, d.ins.bound_item_left(s, d.output.solution.profit(), info), d.sol_curr);
            LOG(info, "s " << s << " ub " << ub << " lb " << d.output.solution.profit());
            if (ub <= d.output.solution.profit()) {
                LOG_FOLD_END(info, " bound");
                return;
            }

            // Recursive call
            assert(s >= d.ins.first_item());
            LOG(info, " remove (" << d.ins.item(s) << ")" << std::endl);
            d.sol_curr.set(s, false); // Remove item s
            sopt_expknap_rec(d, s - 1, t);
            d.sol_curr.set(s, true); // Add item s
        }
    }
    assert(false);
}

ExpknapOutput knapsack::sopt_expknap(Instance& ins, ExpknapOptionalParameters p)
{
    VER(p.info, "*** expknap" << std::endl);
    if (!p.greedy)
        VER(p.info, " g false");
    if (p.surrogate != -1)
        VER(p.info, " s " << p.surrogate);
    if (p.greedynlogn != -1)
        VER(p.info, " gn " << p.greedynlogn);
    if (p.combo_core)
        VER(p.info, " cc " << p.combo_core);
    VER(p.info, " ***" << std::endl);

    LOG_FOLD_START(p.info, "expknap"
            << " g " << p.greedy
            << " gn " << p.greedynlogn
            << " s " << p.surrogate
            << " combo_core " << p.combo_core
            << std::endl);

    bool end = false;
    if (p.end == NULL)
        p.end = &end;

    ExpknapOutput output(ins, p.info);

    if (ins.item_number() == 0) {
        output.update_ub(output.lower_bound, std::stringstream("no item (ub)"), p.info);
        output.algorithm_end(p.info);
        LOG(p.info, "no item" << std::endl);
        return output;
    }

    ins.sort_partially(p.info);
    if (ins.break_item() == ins.last_item() + 1) {
        if (output.lower_bound < ins.break_solution()->profit())
            output.update_sol(*ins.break_solution(), std::stringstream("all items fit (lb)"), p.info);
        output.update_ub(ins.break_solution()->profit(), std::stringstream("all items fit (ub)"), p.info);
        output.algorithm_end(p.info);
        LOG_FOLD_END(p.info, "all items fit in the knapsack");
        return output;
    }
    if (p.combo_core)
        ins.init_combo_core(p.info);

    // Compute initial lower bound
    Solution sol_tmp(ins);
    if (p.greedy) {
        auto g_output = sol_greedy(ins);
        sol_tmp = g_output.solution;
    } else {
        sol_tmp = *ins.break_solution();
    }
    if (output.lower_bound < sol_tmp.profit())
        output.update_sol(sol_tmp, std::stringstream("initial solution"), p.info);

    // Compute initial upper bound
    Profit ub_tmp = ub_dantzig(ins);
    output.update_ub(ub_tmp, std::stringstream("dantzig upper bound"), p.info);

    if (output.solution.profit() == output.upper_bound) {
        output.algorithm_end(p.info);
        return output;
    }

    ExpknapInternalData d(ins, p, output);
    ItemPos b = ins.break_item();
    sopt_expknap_rec(d, b - 1, b);
    if (p.info.check_time())
        output.update_ub(output.lower_bound, std::stringstream("tree search completed (ub)"), p.info);

    if (p.set_end)
        *(p.end) = true;
    for (std::thread& thread: d.threads)
        thread.join();
    d.threads.clear();

    output.algorithm_end(p.info);
    PUT(p.info, "Algorithm", "NodeNumber", output.node_number);
    VER(p.info, "Node number: " << output.node_number << std::endl);
    return output;
}

