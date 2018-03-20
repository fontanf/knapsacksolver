#include "expknap.hpp"

#include "../lb_greedy/greedy.hpp"
#include "../lb_greedynlogn/greedynlogn.hpp"
#include "../ub_dembo/dembo.hpp"
#include "../ub_dantzig/dantzig.hpp"
#include "../ub_surrogate/surrogate.hpp"

#define DBG(x)
//#define DBG(x) x

void update_bounds(const Instance& ins, Solution& sol_best, Profit& ub,
        ExpknapParams& params, StateIdx nodes, Info* info)
{
    if (params.ub_surrogate == nodes) {
        if (Info::verbose(info))
            std::cout << "SURROGATE..." << std::flush;
        auto so = ub_surrogate(ins, {{0, ins.total_item_number()-1}}, sol_best.profit());
        if (ub > so.ub) {
            ub = so.ub;
            if (Info::verbose(info))
                std::cout << " " << ins.print_ub(ub) << std::endl;
        } else {
            if (Info::verbose(info))
                std::cout << " NO IMPROVEMENT" << std::endl;
        }
    }
    if (params.lb_greedynlogn == nodes) {
        if (Info::verbose(info))
            std::cout << "GREEDYNLOGN..." << std::flush;
        if (sol_best.update(sol_bestgreedynlogn(ins))) {
            if (Info::verbose(info))
                std::cout << " " << ins.print_lb(sol_best.profit()) << std::endl;
        } else {
            if (Info::verbose(info))
                std::cout << " NO IMPROVEMENT" << std::endl;
        }
    }
}

bool sopt_expknap_rec(Instance& ins,
        Solution& sol_curr, Solution& sol_best, Profit& u,
        ItemPos s, ItemPos t,
        ExpknapParams& params, StateIdx& nodes, Info* info)
{
    DBG(std::cout
        << "F " << ins.first_item()
        << " S " << s
        << " T " << t
        << " L " << ins.last_item()
        << std::endl;)

    DBG(bool opt_branch = true;
    for (ItemPos j=s+1; j<=t-1; ++j) {
        if (sol_curr.contains(j) != ins.optimal_solution()->contains(j)) {
            opt_branch = false;
            break;
        }
    })

    nodes++; // Increment node number
    update_bounds(ins, sol_best, u, params, nodes, info); // Update bounds

    bool improved = false;
    if (sol_curr.remaining_capacity() >= 0) {
        if (sol_best.update(sol_curr)) {
            if (Info::verbose(info))
                std::cout << ins.print_lb(sol_best.profit()) << std::endl;
            improved = true;
        }
        for (;;t++) {
            // If UB reached, then stop
            if (sol_best.profit() == u)
                return false;

            // Expand
            if (ins.int_right_size() > 0 && t > ins.last_item())
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
            if (sopt_expknap_rec(ins, sol_curr, sol_best, u, s, t+1, params, nodes, info))
                improved = true;
            sol_curr.set(t, false); // Remove item t
        }
    } else {
        for (;;s--) {
            // If UB reached, then stop
            if (sol_best.profit() == u)
                return false;

            // Expand
            if (ins.int_left_size() > 0 && s < ins.first_item())
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
            if (sopt_expknap_rec(ins, sol_curr, sol_best, u, s-1, t, params, nodes, info))
                improved = true;
            sol_curr.set(s, true); // Add item s
        }
    }
    assert(false);
    return improved;
}

Solution sopt_expknap(Instance& ins, ExpknapParams& params, Info* info)
{
    (void)info;
    if (ins.item_number() == 0)
        return Solution(ins);

    ins.sort_partially();
    if (ins.break_item() == ins.last_item()+1)
        return *ins.break_solution();

    Solution sol_curr = *ins.break_solution();
    Solution sol_best = sol_bestgreedy(ins);
    Profit ub = ub_dantzig(ins);
    if (Info::verbose(info))
        std::cout
            << ins.print_lb(sol_best.profit()) << " "
            << ins.print_ub(ub) << std::endl;
    ItemPos b = ins.break_item();
    ins.set_first_last_item();

    StateIdx nodes = 0;
    update_bounds(ins, sol_best, ub, params, nodes, info); // Update bounds
    if (sol_best.profit() != ub) // If UB reached, then stop
        sopt_expknap_rec(ins, sol_curr, sol_best, ub, b-1, b, params, nodes, info);

    if (Info::verbose(info))
        std::cout << "NODES " << std::scientific << (double)nodes << std::endl;
    assert(ins.check_sopt(sol_best));
    return sol_best;
}

#undef DBG
