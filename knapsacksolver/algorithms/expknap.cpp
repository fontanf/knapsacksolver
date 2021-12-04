#include "knapsacksolver/algorithms/expknap.hpp"

#include "knapsacksolver/algorithms/greedy.hpp"
#include "knapsacksolver/algorithms/greedynlogn.hpp"
#include "knapsacksolver/algorithms/dembo.hpp"
#include "knapsacksolver/algorithms/dantzig.hpp"
#include "knapsacksolver/algorithms/surrelax.hpp"

using namespace knapsacksolver;

struct ExpknapInternalData
{
    ExpknapInternalData(Instance& instance, ExpknapOptionalParameters& p, ExpknapOutput& output):
        instance(instance), p(p), output(output), sol_curr(*instance.break_solution()) { }
    Instance& instance;
    ExpknapOptionalParameters& p;
    ExpknapOutput& output;
    Solution sol_curr;
    std::vector<std::thread> threads;
};

void expknap_update_bounds(ExpknapInternalData& d)
{
    if (d.p.surrelax >= 0 && d.p.surrelax <= d.output.number_of_node) {
        d.p.surrelax = -1;
        std::function<Output (Instance&, Info, bool*)> func
            = [&d](Instance& ins, Info info, bool* end)
            {
                ExpknapOptionalParameters p;
                p.info = info;
                p.greedy = d.p.greedy;
                p.greedynlogn = d.p.greedynlogn;
                p.surrelax = -1;
                p.combo_core = d.p.combo_core;
                p.end = end;
                p.stop_if_end = true;
                p.set_end = false;
                return expknap(ins, p);
            };
        d.threads.push_back(std::thread(solvesurrelax, SurrelaxData{
                    .instance = Instance::reset(d.instance),
                    .output   = d.output,
                    .func     = func,
                    .end      = d.p.end,
                    .info     = Info(d.p.info, true, "surrelax")}));
    }
    if (d.p.greedynlogn >= 0 && d.p.greedynlogn <= d.output.number_of_node) {
        d.p.greedynlogn = -1;
        auto gn_output = greedynlogn(d.instance);
        d.output.update_sol(gn_output.solution, std::stringstream("greedynlogn"), d.p.info);
    }
}

void expknap_rec(ExpknapInternalData& d, ItemPos s, ItemPos t)
{
    Info& info = d.p.info;
    d.output.number_of_node++; // Increment node number
    LOG_FOLD_START(info, "node number " << d.output.number_of_node
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
    expknap_update_bounds(d);

    if (d.sol_curr.remaining_capacity() >= 0) {
        // Update best solution
        if (d.output.solution.profit() < d.sol_curr.profit()) {
            std::stringstream ss;
            ss << "node " << d.output.number_of_node;
            d.output.update_sol(d.sol_curr, ss, info);
        }

        for (;;t++) {
            // Bounding test
            Profit ub = ub_dembo(d.instance, d.instance.bound_item_right(t, d.output.solution.profit() DBG(COMMA info)), d.sol_curr);
            LOG(info, "t " << t << " ub " << ub << " lb " << d.output.solution.profit());
            if (ub <= d.output.solution.profit()) {
                LOG_FOLD_END(info, " bound");
                return;
            }

            // Recursive call
            assert(t <= d.instance.last_item());
            LOG(info, " add (" << d.instance.item(t) << ")" << std::endl);
            d.sol_curr.set(t, true); // Add item t
            expknap_rec(d, s, t + 1);
            d.sol_curr.set(t, false); // Remove item t
        }
    } else {
        for (;;s--) {
            // Bounding test
            Profit ub = ub_dembo_rev(d.instance, d.instance.bound_item_left(s, d.output.solution.profit() DBG(COMMA info)), d.sol_curr);
            LOG(info, "s " << s << " ub " << ub << " lb " << d.output.solution.profit());
            if (ub <= d.output.solution.profit()) {
                LOG_FOLD_END(info, " bound");
                return;
            }

            // Recursive call
            assert(s >= d.instance.first_item());
            LOG(info, " remove (" << d.instance.item(s) << ")" << std::endl);
            d.sol_curr.set(s, false); // Remove item s
            expknap_rec(d, s - 1, t);
            d.sol_curr.set(s, true); // Add item s
        }
    }
    assert(false);
}

ExpknapOutput knapsacksolver::expknap(Instance& instance, ExpknapOptionalParameters p)
{
    VER(p.info, "*** expknap"
            << ((p.greedy)? " -g": "")
            << " -s " << p.surrelax
            << " -n " << p.greedynlogn
            << ((p.combo_core)? " -c": "")
            << " ***" << std::endl);

    LOG_FOLD_START(p.info, "*** expknap"
            << ((p.greedy)? " -g": "")
            << " -s " << p.surrelax
            << " -n " << p.greedynlogn
            << ((p.combo_core)? " -c": "")
            << " ***" << std::endl);

    bool end = false;
    if (p.end == NULL)
        p.end = &end;

    ExpknapOutput output(instance, p.info);

    if (instance.reduced_number_of_items() == 0) {
        output.update_ub(output.lower_bound, std::stringstream("no item (ub)"), p.info);
        LOG(p.info, "no item" << std::endl);
        return output.algorithm_end(p.info);
    }

    instance.sort_partially(DBG(p.info));
    if (instance.break_item() == instance.last_item() + 1) {
        if (output.lower_bound < instance.break_solution()->profit())
            output.update_sol(*instance.break_solution(), std::stringstream("all items fit (lb)"), p.info);
        output.update_ub(instance.break_solution()->profit(), std::stringstream("all items fit (ub)"), p.info);
        LOG_FOLD_END(p.info, "all items fit in the knapsack");
        return output.algorithm_end(p.info);
    }
    if (p.combo_core)
        instance.init_combo_core(DBG(p.info));

    // Compute initial lower bound
    Solution sol_tmp(instance);
    if (p.greedy) {
        auto g_output = greedy(instance);
        sol_tmp = g_output.solution;
    } else {
        sol_tmp = *instance.break_solution();
    }
    if (output.lower_bound < sol_tmp.profit())
        output.update_sol(sol_tmp, std::stringstream("initial solution"), p.info);

    // Compute initial upper bound
    Profit ub_tmp = ub_dantzig(instance);
    output.update_ub(ub_tmp, std::stringstream("dantzig upper bound"), p.info);

    if (output.solution.profit() == output.upper_bound)
        return output.algorithm_end(p.info);

    ExpknapInternalData d(instance, p, output);
    ItemPos b = instance.break_item();
    expknap_rec(d, b - 1, b);
    if (p.info.check_time())
        output.update_ub(output.lower_bound, std::stringstream("tree search completed (ub)"), p.info);

    if (p.set_end)
        *(p.end) = true;
    for (std::thread& thread: d.threads)
        thread.join();
    d.threads.clear();

    return output.algorithm_end(p.info);
}

