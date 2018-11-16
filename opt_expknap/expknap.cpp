#include "knapsack/opt_expknap/expknap.hpp"

#include "knapsack/lb_greedy/greedy.hpp"
#include "knapsack/lb_greedynlogn/greedynlogn.hpp"
#include "knapsack/ub_dembo/dembo.hpp"
#include "knapsack/ub_dantzig/dantzig.hpp"
#include "knapsack/ub_surrogate/surrogate.hpp"

using namespace knapsack;

#define DBG(x)
//#define DBG(x) x

void update_bounds(const Instance& ins, Solution& sol_best, Profit& ub, SurrogateOut& so,
        ExpknapParams& params, StateIdx nodes, Info& info)
{
    if (params.ub_surrogate == nodes) {
        if (info.verbose)
            std::cout << "SURROGATE..." << std::flush;
        Info info_tmp;
        so = ub_surrogate(ins, sol_best.profit(), info_tmp);
        if (info.verbose)
            std::cout << " K " << so.bound << " S " << so.multiplier << std::flush;
        if (ub > so.ub) {
            ub = so.ub;
            if (info.verbose)
                std::cout << " " << ins.print_ub(ub) << std::endl;
        } else {
            if (info.verbose)
                std::cout << " NO IMPROVEMENT" << std::endl;
        }
    }

    if (params.solve_sur == nodes) {
        if (sol_best.profit() == ub)
            return;
        if (info.verbose)
            std::cout << "SOLVE SURROGATE..." << std::endl;
        assert(params.ub_surrogate >= 0 && params.ub_surrogate <= params.solve_sur);
        Instance ins_sur(ins);
        ins_sur.surrogate(so.multiplier, so.bound);
        params.ub_surrogate = -1;
        params.solve_sur = -1;
        Solution sol_sur = sopt_expknap(ins_sur, params, info);
        if (ub > sol_sur.profit()) {
            ub = sol_sur.profit();
            if (info.verbose)
                std::cout << "END SOLVE SURROGATE " << ins.print_ub(ub) << std::flush;
        }
        if (info.verbose)
            std::cout << " K " << sol_sur.item_number() << "/" << so.bound << std::flush;
        if (sol_sur.item_number() == so.bound) {
            sol_best = sol_sur;
            if (info.verbose)
                std::cout << " " << ins.print_lb(sol_best.profit()) << std::flush;
        }
        if (info.verbose)
            std::cout << std::endl;
    }

    if (params.lb_greedynlogn == nodes) {
        if (info.verbose)
            std::cout << "GREEDYNLOGN..." << std::flush;
        Info info_tmp;
        if (sol_best.update(sol_bestgreedynlogn(ins, info_tmp))) {
            if (info.verbose)
                std::cout << " " << ins.print_lb(sol_best.profit()) << std::endl;
        } else {
            if (info.verbose)
                std::cout << " NO IMPROVEMENT" << std::endl;
        }
    }
}

bool sopt_expknap_rec(Instance& ins,
        Solution& sol_curr, Solution& sol_best, Profit& u, SurrogateOut& so,
        ItemPos s, ItemPos t,
        ExpknapParams& params, StateIdx& nodes, Info& info)
{
    DBG(std::cout
        << "F " << ins.first_sorted_item()
        << " S " << s
        << " T " << t
        << " L " << ins.last_sorted_item()
        << std::endl;)

    DBG(bool opt_branch = true;
    for (ItemPos j=s+1; j<=t-1; ++j) {
        if (sol_curr.contains(j) != ins.optimal_solution()->contains(j)) {
            opt_branch = false;
            break;
        }
    })

    nodes++; // Increment node number
    update_bounds(ins, sol_best, u, so, params, nodes, info); // Update bounds

    bool improved = false;
    if (sol_curr.remaining_capacity() >= 0) {
        if (sol_best.update(sol_curr)) {
            if (info.verbose)
                std::cout << ins.print_lb(sol_best.profit()) << std::endl;
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
            DBG(if (opt_branch
                    && sol_best.profit() != ins.optimum()
                    && ub <= sol_best.profit()) {
                std::cout
                    << "UB " << ub
                    << " LB " << sol_best.profit()
                    << " OPT " << ins.optimum()
                    << std::endl;
                std::cout << sol_curr.print_bin() << std::endl;
                std::cout << ins.optimal_solution()->print_bin() << std::endl;
                assert(false);
            })
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
            DBG(if (opt_branch
                    && sol_best.profit() != ins.optimum()
                    && ub <= sol_best.profit()) {
                std::cout
                    << "P " << sol_curr.profit()
                    << "R " << sol_curr.remaining_capacity()
                    << " UB " << ub
                    << " LB " << sol_best.profit()
                    << " OPT " << ins.optimum()
                    << std::endl;
                std::cout << sol_curr.print_bin() << std::endl;
                std::cout << ins.optimal_solution()->print_bin() << std::endl;
                assert(false);
            })
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

Solution knapsack::sopt_expknap(Instance& ins, ExpknapParams& params, Info& info)
{
    (void)info;
    if (ins.item_number() == 0)
        return Solution(ins);

    ins.sort_partially();
    if (ins.break_item() == ins.last_item()+1)
        return *ins.break_solution();

    Solution sol_curr = *ins.break_solution();
    Info info_tmp;
    Solution sol_best = sol_greedy(ins, info_tmp);
    Profit ub = ub_dantzig(ins);
    if (info.verbose)
        std::cout
            << ins.print_lb(sol_best.profit()) << " "
            << ins.print_ub(ub) << std::endl;
    ItemPos b = ins.break_item();

    SurrogateOut so;
    StateIdx nodes = 0;
    update_bounds(ins, sol_best, ub, so, params, nodes, info); // Update bounds
    if (sol_best.profit() != ub) // If UB reached, then stop
        sopt_expknap_rec(ins, sol_curr, sol_best, ub, so, b-1, b, params, nodes, info);

    if (info.verbose)
        std::cout << "NODES " << std::scientific << (double)nodes << std::endl;
    assert(ins.check_sopt(sol_best));
    return sol_best;
}

#undef DBG
