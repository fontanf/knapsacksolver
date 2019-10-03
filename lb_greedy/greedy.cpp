#include "knapsack/lb_greedy/greedy.hpp"

using namespace knapsack;

Output knapsack::sol_greedy(const Instance& ins, Info info)
{
    LOG_FOLD_START(info, "sol_greedy" << std::endl);
    VER(info, "*** greedy ***" << std::endl);
    Output output(ins, info);

    assert(ins.break_item() != -1);

    output.update_sol(*ins.break_solution(), std::stringstream("break"), info);
    std::string best_algo = "break";
    LOG(info, "break " << output.solution.profit() << std::endl);
    ItemPos b = ins.break_item();

    if (b < ins.last_item()) {
        Profit  p = 0;
        ItemPos j = -1;

        // Backward greedy
        Weight rb = output.solution.remaining_capacity() - ins.item(b).w;
        for (ItemPos k=ins.first_item(); k<=b; ++k) {
            if (rb + ins.item(k).w >= 0 && ins.item(b).p - ins.item(k).p > p) {
                p = ins.item(b).p - ins.item(k).p;
                j = k;
            }
        }

        // Forward greedy
        Weight rf = output.solution.remaining_capacity();
        for (ItemPos k=b+1; k<=ins.last_item(); ++k) {
            if (ins.item(k).w <= rf && ins.item(k).p > p) {
                p = ins.item(k).p;
                j = k;
            }
        }

        if (j == -1) {
        } else if (j <= b) {
            best_algo = "backward";
            Solution sol = output.solution;
            sol.set(b, true);
            sol.set(j, false);
            output.update_sol(sol, std::stringstream("backward"), info);
        } else {
            best_algo = "forward";
            Solution sol = output.solution;
            sol.set(j, true);
            output.update_sol(sol, std::stringstream("forward"), info);
        }
    }

    PUT(info, "Algorithm", "Best", best_algo);
    LOG_FOLD_END(info, "sol_greedy " << output.solution.profit());
    output.algorithm_end(info);
    return output;
}

