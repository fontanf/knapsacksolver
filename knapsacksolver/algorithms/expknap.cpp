#include "knapsacksolver/algorithms/expknap.hpp"

#include "knapsacksolver/algorithms/greedy.hpp"
#include "knapsacksolver/algorithms/greedynlogn.hpp"
#include "knapsacksolver/algorithms/dembo.hpp"
#include "knapsacksolver/algorithms/dantzig.hpp"
#include "knapsacksolver/algorithms/surrelax.hpp"

using namespace knapsacksolver;

struct ExpknapInternalData
{
    ExpknapInternalData(
            Instance& instance,
            ExpknapOptionalParameters& parameters,
            ExpknapOutput& output):
        instance(instance),
        parameters(parameters),
        output(output),
        sol_curr(*instance.break_solution()) { }

    Instance& instance;
    ExpknapOptionalParameters& parameters;
    ExpknapOutput& output;
    Solution sol_curr;
    std::vector<std::thread> threads;
};

void expknap_update_bounds(ExpknapInternalData& d)
{
    if (d.parameters.surrelax >= 0
            && d.parameters.surrelax <= d.output.number_of_node) {
        d.parameters.surrelax = -1;
        std::function<Output (Instance&, Info, bool*)> func
            = [&d](Instance& instance, Info info, bool* end)
            {
                ExpknapOptionalParameters parameters;
                parameters.info = info;
                parameters.greedy = d.parameters.greedy;
                parameters.greedynlogn = d.parameters.greedynlogn;
                parameters.surrelax = -1;
                parameters.combo_core = d.parameters.combo_core;
                parameters.end = end;
                parameters.stop_if_end = true;
                parameters.set_end = false;
                return expknap(instance, parameters);
            };
        d.threads.push_back(std::thread(
                    solvesurrelax,
                    Instance::reset(d.instance),
                    std::ref(d.output),
                    func,
                    d.parameters.end,
                    Info(d.parameters.info, true, "surrelax")));
    }
    if (d.parameters.greedynlogn >= 0
            && d.parameters.greedynlogn <= d.output.number_of_node) {
        d.parameters.greedynlogn = -1;
        auto gn_output = greedynlogn(d.instance);
        d.output.update_solution(
                gn_output.solution,
                std::stringstream("greedynlogn"),
                d.parameters.info);
    }
}

void expknap_rec(ExpknapInternalData& d, ItemPos s, ItemPos t)
{
    Info& info = d.parameters.info;
    d.output.number_of_node++; // Increment node number
    FFOT_LOG_FOLD_START(info, "node number " << d.output.number_of_node
            << " s " << s << " t " << t
            << " w " << d.sol_curr.weight() << " p " << d.sol_curr.profit()
            << std::endl);

    // Check end
    if (d.parameters.stop_if_end && *(d.parameters.end)) {
        FFOT_LOG_FOLD_END(info, "end");
        return;
    }

    // Check time
    if (!info.check_time()) {
        if (d.parameters.set_end)
            *(d.parameters.end) = true;
        for (std::thread& thread: d.threads)
            thread.join();
        d.threads.clear();
        FFOT_LOG_FOLD_END(info, "time");
        return;
    }

    // If UB reached, then stop
    if (d.output.solution.profit() == d.output.upper_bound) {
        FFOT_LOG_FOLD_END(info, "lb == ub");
        return;
    }

    // Update bounds
    expknap_update_bounds(d);

    if (d.sol_curr.remaining_capacity() >= 0) {
        // Update best solution
        if (d.output.solution.profit() < d.sol_curr.profit()) {
            std::stringstream ss;
            ss << "node " << d.output.number_of_node;
            d.output.update_solution(d.sol_curr, ss, info);
        }

        for (;;t++) {
            // Bounding test
            Profit ub = ub_dembo(d.instance, d.instance.bound_item_right(t, d.output.solution.profit() FFOT_DBG(FFOT_COMMA info)), d.sol_curr);
            FFOT_LOG(info, "t " << t << " ub " << ub << " lb " << d.output.solution.profit());
            if (ub <= d.output.solution.profit()) {
                FFOT_LOG_FOLD_END(info, " bound");
                return;
            }

            // Recursive call
            assert(t <= d.instance.last_item());
            FFOT_LOG(info, " add (" << d.instance.item(t) << ")" << std::endl);
            d.sol_curr.set(t, true); // Add item t
            expknap_rec(d, s, t + 1);
            d.sol_curr.set(t, false); // Remove item t
        }
    } else {
        for (;;s--) {
            // Bounding test
            Profit ub = ub_dembo_rev(
                    d.instance,
                    d.instance.bound_item_left(s, d.output.solution.profit() FFOT_DBG(FFOT_COMMA info)),
                    d.sol_curr);
            FFOT_LOG(info, "s " << s << " ub " << ub << " lb " << d.output.solution.profit());
            if (ub <= d.output.solution.profit()) {
                FFOT_LOG_FOLD_END(info, " bound");
                return;
            }

            // Recursive call
            assert(s >= d.instance.first_item());
            FFOT_LOG(info, " remove (" << d.instance.item(s) << ")" << std::endl);
            d.sol_curr.set(s, false); // Remove item s
            expknap_rec(d, s - 1, t);
            d.sol_curr.set(s, true); // Add item s
        }
    }
    assert(false);
}

ExpknapOutput knapsacksolver::expknap(
        Instance& instance,
        ExpknapOptionalParameters parameters)
{
    init_display(instance, parameters.info);
    FFOT_VER(parameters.info,
               "Algorithm" << std::endl
            << "---------" << std::endl
            << "Expknap" << std::endl
            << std::endl
            << "Parameters" << std::endl
            << "----------" << std::endl
            << "Greedy:                 " << parameters.greedy << std::endl
            << "Surrogate relaxation:   " << parameters.surrelax << std::endl
            << "Combo core:             " << parameters.combo_core << std::endl
            << std::endl);

    FFOT_LOG_FOLD_START(parameters.info, "*** expknap"
            << ((parameters.greedy)? " -g": "")
            << " -s " << parameters.surrelax
            << " -n " << parameters.greedynlogn
            << ((parameters.combo_core)? " -c": "")
            << " ***" << std::endl);

    bool end = false;
    if (parameters.end == NULL)
        parameters.end = &end;

    ExpknapOutput output(instance, parameters.info);

    if (instance.reduced_number_of_items() == 0) {
        output.update_upper_bound(
                output.lower_bound,
                std::stringstream("no item (ub)"),
                parameters.info);
        FFOT_LOG(parameters.info, "no item" << std::endl);
        return output.algorithm_end(parameters.info);
    }

    instance.sort_partially(FFOT_DBG(parameters.info));
    if (instance.break_item() == instance.last_item() + 1) {
        if (output.lower_bound < instance.break_solution()->profit()) {
            output.update_solution(
                    *instance.break_solution(),
                    std::stringstream("all items fit (lb)"),
                    parameters.info);
        }
        output.update_upper_bound(
                instance.break_solution()->profit(),
                std::stringstream("all items fit (ub)"),
                parameters.info);
        FFOT_LOG_FOLD_END(parameters.info, "all items fit in the knapsack");
        return output.algorithm_end(parameters.info);
    }
    if (parameters.combo_core)
        instance.init_combo_core(FFOT_DBG(parameters.info));

    // Compute initial lower bound
    Solution sol_tmp(instance);
    if (parameters.greedy) {
        auto g_output = greedy(instance);
        sol_tmp = g_output.solution;
    } else {
        sol_tmp = *instance.break_solution();
    }
    if (output.lower_bound < sol_tmp.profit()) {
        output.update_solution(
                sol_tmp,
                std::stringstream("initial solution"),
                parameters.info);
    }

    // Compute initial upper bound
    Profit ub_tmp = ub_dantzig(instance);
    output.update_upper_bound(
            ub_tmp,
            std::stringstream("dantzig upper bound"),
            parameters.info);

    if (output.solution.profit() == output.upper_bound)
        return output.algorithm_end(parameters.info);

    ExpknapInternalData d(instance, parameters, output);
    ItemPos b = instance.break_item();
    expknap_rec(d, b - 1, b);
    if (parameters.info.check_time()) {
        output.update_upper_bound(
                output.lower_bound,
                std::stringstream("tree search completed (ub)"),
                parameters.info);
    }

    if (parameters.set_end)
        *(parameters.end) = true;
    for (std::thread& thread: d.threads)
        thread.join();
    d.threads.clear();

    return output.algorithm_end(parameters.info);
}

