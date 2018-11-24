#include "knapsack/lb_greedy/greedy.hpp"

using namespace knapsack;

Solution knapsack::sol_greedy(const Instance& ins, Info& info)
{
    info.verbose("*** greedy ***\n");
    assert(ins.break_item_found());

    Solution sol = *ins.break_solution();
    std::string best_algo = "Break";
    ItemPos b = ins.break_item();

    if (b < ins.last_item()) {
        Profit  p = 0;
        ItemPos j = -1;

        Weight rb = sol.remaining_capacity() - ins.item(b).w;
        for (ItemPos k=ins.first_item(); k<=b; ++k) {
            if (rb + ins.item(k).w >= 0 && ins.item(b).p - ins.item(k).p > p) {
                p = ins.item(b).p - ins.item(k).p;
                j = k;
            }
        }

        Weight rf = sol.remaining_capacity();
        for (ItemPos k=b+1; k<=ins.last_item(); ++k) {
            if (ins.item(k).w <= rf && ins.item(k).p > p) {
                p = ins.item(k).p;
                j = k;
            }
        }

        if (j == -1) {
            best_algo = "Break";
        } else if (j <= b) {
            best_algo = "Backward";
            sol.set(b, true);
            sol.set(j, false);
        } else {
            best_algo = "Forward";
            sol.set(j, true);
        }
    }

    info.verbose("Best algorithm: " + best_algo + "\n");
    info.pt.put("Algorithm.Best", best_algo);

    return algorithm_end(sol, info);
}

