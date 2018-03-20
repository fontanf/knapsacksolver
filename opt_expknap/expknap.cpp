#include "expknap.hpp"

#include "../lb_greedy/greedy.hpp"
#include "../ub_dembo/dembo.hpp"

#define DBG(x)
//#define DBG(x) x

bool sopt_expknap_rec(Instance& ins, Solution& sol_curr, Solution& sol_best, ItemPos s, ItemPos t, Info* info)
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

    bool improved = false;
    if (sol_curr.remaining_capacity() >= 0) {
        if (sol_best.update(sol_curr)) {
            if (Info::verbose(info))
                std::cout << ins.print_lb(sol_best.profit()) << std::endl;
            improved = true;
        }
        for (;;t++) {
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
            if (sopt_expknap_rec(ins, sol_curr, sol_best, s, t+1, info))
                improved = true;
            sol_curr.set(t, false); // Remove item t
        }
    } else {
        for (;;s--) {
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
            if (sopt_expknap_rec(ins, sol_curr, sol_best, s-1, t, info))
                improved = true;
            sol_curr.set(s, true); // Add item s
        }
    }
    assert(false);
    return improved;
}

Solution sopt_expknap(Instance& ins, Info* info)
{
    (void)info;
    if (ins.item_number() == 0)
        return Solution(ins);

    ins.sort_partially();
    if (ins.break_item() == ins.last_item()+1)
        return *ins.break_solution();

    Solution sol_curr = *ins.break_solution();
    Solution sol_best = sol_bestgreedy(ins);
    ItemPos b = ins.break_item();
    ins.set_first_last_item();
    sopt_expknap_rec(ins, sol_curr, sol_best, b-1, b, info);
    return sol_best;
}

#undef DBG
