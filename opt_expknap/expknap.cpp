#include "knapsack/opt_expknap/expknap.hpp"

#include "knapsack/lb_greedy/greedy.hpp"
#include "knapsack/lb_greedynlogn/greedynlogn.hpp"
#include "knapsack/ub_dembo/dembo.hpp"
#include "knapsack/ub_dantzig/dantzig.hpp"

using namespace knapsack;

void Expknap::surrogate(Instance ins, Info info)
{
    SurrogateOut so = ub_surrogate(ins, sol_best_.profit());
    if (ub_ > so.ub)
        update_ub(sol_best_.profit(), ub_, so.ub,
                std::stringstream("surrogate relaxation"), info);

    if (sol_best_.profit() == ub_) {
        info.output->mutex_sol.lock();
        *end_ = true;
        info.output->mutex_sol.unlock();
        return;
    }

    Instance ins_sur(instance_);
    ins_sur.surrogate(info, so.multiplier, so.bound);
    Solution sol_sur = Expknap(ins_sur, params_).run();
    if (ub_ > sol_sur.profit())
        update_ub(sol_best_.profit(), ub_, sol_sur.profit(),
                std::stringstream("surrogate ins resolution"), info);
    if (sol_sur.item_number() == so.bound)
        update_sol(sol_best_, ub_, sol_sur,
                std::stringstream("surrogate ins resolution"), info);
}

void Expknap::update_bounds(Info& info)
{
    if (params_.ub_surrogate >= 0 && params_.ub_surrogate <= node_number_) {
        params_.ub_surrogate = -1;
        threads_.push_back(std::thread(&Expknap::surrogate, this,
                    Instance::reset(instance_), Info(info, true, false)));
    }
    if (params_.lb_greedynlogn >= 0 && params_.lb_greedynlogn <= node_number_) {
        params_.lb_greedynlogn = -1;
        Solution sol = sol_greedynlogn(instance_);
        if (sol_best_.profit() < sol.profit()) {
            update_sol(sol_best_, ub_, sol,
                    std::stringstream("greedynlogn"), info);
        }
    }
}

void Expknap::rec(Info& info)
{
    node_number_++; // Increment node number
    LOG_FOLD_START(info, "node number " << node_number_ << " s " << s_ << " t " << t_ << std::endl);

    if (*end_) {
        LOG_FOLD_END(info, "end");
        return;
    }
    if (info.elapsed_time() > params_.time_limit) {
        LOG_FOLD_END(info, "time");
        return;
    }
    update_bounds(info); // Update bounds

    ItemPos s = s_;
    ItemPos t = t_;
    if (sol_curr_.remaining_capacity() >= 0) {
        if (sol_curr_.profit() > sol_best_.profit()) {
            std::stringstream ss;
            ss << "node " << node_number_;
            update_sol(sol_best_, ub_, sol_curr_, ss, info);
        }
        for (;;t++) {
            // If UB reached, then stop
            if (sol_best_.profit() == ub_) {
                LOG_FOLD_END(info, "lb == ub");
                return;
            }

            // Expand
            if (instance_.int_right_size() > 0 && t > instance_.last_sorted_item())
                instance_.sort_right(sol_best_.profit());

            // Bounding test
            Profit ub = ub_dembo(instance_, t, sol_curr_);
            if (ub <= sol_best_.profit()) {
                LOG_FOLD_END(info, "bound");
                return;
            }

            // Recursive call
            sol_curr_.set(t, true); // Add item t
            t_ = t + 1;
            s_ = s;
            rec(info);
            sol_curr_.set(t, false); // Remove item t
        }
    } else {
        for (;;s--) {
            // If UB reached, then stop
            if (sol_best_.profit() == ub_) {
                LOG_FOLD_END(info, "lb == ub");
                return;
            }

            // Expand
            if (instance_.int_left_size() > 0 && s < instance_.first_sorted_item())
                instance_.sort_left(sol_best_.profit());

            // Bounding test
            Profit ub = ub_dembo_rev(instance_, s, sol_curr_);
            if (ub <= sol_best_.profit()) {
                LOG_FOLD_END(info, "bound");
                return;
            }

            // Recursive call
            sol_curr_.set(s, false); // Remove item s
            s_ = s - 1;
            t_ = t;
            rec(info);
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

    sol_best_ = sol_greedy(instance_);
    ub_ = ub_dantzig(instance_);
    init_display(sol_best_.profit(), ub_, info);

    ItemPos b = instance_.break_item();

    update_bounds(info); // Update bounds
    if (sol_best_.profit() != ub_) { // If UB reached, then stop
        sol_curr_ = *instance_.break_solution();
        s_ = b - 1;
        t_ = b;
        rec(info);
        PUT(info, "Algorithm.NodeNumber", node_number_);
        VER(info, "s " << s_ << " - t " << t_ << std::endl;)
        VER(info, "Node number " << node_number_ << std::endl;)
    }

    *end_ = true;
    return algorithm_end(sol_best_, info);
}

