#include "knapsack/opt_expknap/expknap.hpp"

#include "knapsack/lb_greedy/greedy.hpp"
#include "knapsack/lb_greedynlogn/greedynlogn.hpp"
#include "knapsack/ub_dembo/dembo.hpp"
#include "knapsack/ub_dantzig/dantzig.hpp"

using namespace knapsack;

void Expknap::update_bounds(Info& info)
{
    if (params_.ub_surrogate >= 0 && params_.ub_surrogate <= node_number_) {
        params_.ub_surrogate = -1;
        std::function<Solution (Instance&, Info, bool*)> func
            = [this](Instance& ins, Info info, bool* end) {
                return Expknap(ins, params_, end).run(info); };
        threads_.push_back(std::thread(ub_solvesurrelax, SurrelaxData{
                    .ins      = Instance::reset(instance_),
                    .lb       = NULL,
                    .sol_best = &sol_best_,
                    .ub       = ub_,
                    .func     = func,
                    .end      = end_,
                    .info     = Info(info, true, "surrelax")}));
    }
    if (params_.lb_greedynlogn >= 0 && params_.lb_greedynlogn <= node_number_) {
        params_.lb_greedynlogn = -1;
        Solution sol = sol_greedynlogn(instance_);
        if (sol_best_.profit() < sol.profit())
            update_sol(&sol_best_, NULL, ub_, sol, std::stringstream("greedynlogn"), info);
    }
}

void Expknap::rec(ItemPos s, ItemPos t, Info& info)
{
    node_number_++; // Increment node number
    LOG_FOLD_START(info, "node number " << node_number_
            << " s " << s << " t " << t
            << " w " << sol_curr_.weight() << " p " << sol_curr_.profit()
            << std::endl);

    if (*end_) {
        LOG_FOLD_END(info, "end");
        return;
    }
    if (!info.check_time()) { // Check time
        LOG_FOLD_END(info, "time");
        return;
    }
    if (sol_best_.profit() == ub_) { // If UB reached, then stop
        LOG_FOLD_END(info, "lb == ub");
        return;
    }
    update_bounds(info); // Update bounds

    if (sol_curr_.remaining_capacity() >= 0) {
        if (sol_best_.profit() < sol_curr_.profit()) {
            std::stringstream ss;
            ss << "node " << node_number_;
            update_sol(&sol_best_, NULL, ub_, sol_curr_, ss, info);
        }
        for (;;t++) {
            // Bounding test
            Profit ub = ub_dembo(instance_, instance_.bound_item_right(t, sol_best_.profit(), info), sol_curr_);
            LOG(info, "t " << t << " ub " << ub << " lb " << sol_best_.profit());
            if (ub <= sol_best_.profit()) {
                LOG_FOLD_END(info, " bound");
                return;
            }

            // Recursive call
            assert(t <= instance_.last_item());
            LOG(info, " add (" << instance_.item(t) << ")" << std::endl);
            sol_curr_.set(t, true); // Add item t
            rec(s, t + 1, info);
            sol_curr_.set(t, false); // Remove item t
        }
    } else {
        for (;;s--) {
            // Bounding test
            Profit ub = ub_dembo_rev(instance_, instance_.bound_item_left(s, sol_best_.profit(), info), sol_curr_);
            LOG(info, "s " << s << " ub " << ub << " lb " << sol_best_.profit());
            if (ub <= sol_best_.profit()) {
                LOG_FOLD_END(info, " bound");
                return;
            }

            // Recursive call
            assert(s >= instance_.first_item());
            LOG(info, " remove (" << instance_.item(s) << ")" << std::endl);
            sol_curr_.set(s, false); // Remove item s
            rec(s - 1, t, info);
            sol_curr_.set(s, true); // Add item s
        }
    }
    assert(false);
}

Solution Expknap::run(Info info)
{
    VER(info, "*** expknap ***" << std::endl);

    if (instance_.item_number() == 0) {
        LOG(info, "empty instance" << std::endl);
        Solution sol(instance_);
        return algorithm_end(sol, info);
    }

    instance_.sort_partially(info);
    if (instance_.break_item() == instance_.last_item()+1) {
        LOG(info, "all items fit in the knapsack" << std::endl);
        Solution sol = *instance_.break_solution();
        return algorithm_end(sol, info);
    }
    if (params_.combo_core)
        instance_.init_combo_core(info);

    // Initial bounds
    if (params_.lb_greedy < 0) {
        sol_best_ = *instance_.break_solution();
    } else {
        sol_best_ = sol_greedy(instance_);
    }
    ub_ = ub_dantzig(instance_);
    init_display(sol_best_.profit(), ub_, info);
    if (sol_best_.profit() == ub_)
        return algorithm_end(sol_best_, info);

    ItemPos b = instance_.break_item();
    sol_curr_ = *instance_.break_solution();
    rec(b - 1, b, info);
    if (info.check_time() && sol_best_.profit() != ub_)
        update_ub(sol_best_.profit(), ub_, sol_best_.profit(),
                std::stringstream("tree search completed"), info);
    if (!sur_)
        *end_ = true;
    for (std::thread& thread: threads_)
        thread.join();
    PUT(info, "Algorithm.NodeNumber", node_number_);
    return algorithm_end(sol_best_, info);
}

