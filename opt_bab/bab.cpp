#include "knapsack/opt_bab/bab.hpp"

#include "knapsack/ub_dembo/dembo.hpp"

using namespace knapsack;

Solution knapsack::sopt_bab(Instance& ins, Info info)
{
    VER(info, "*** bab ***" << std::endl);

    ItemIdx n = ins.item_number();
    Solution sol_curr(ins);
    if (n == 0)
        return algorithm_end(sol_curr, info);

    ItemPos j_max = ins.max_efficiency_item(info);
    std::vector<Weight> min_weight = ins.min_weights();

    Solution sol_best(ins);
    Cpt node_number = 0;
    for (ItemIdx i=0; ; ++i) {
        if (!info.check_time())
            return algorithm_end(sol_best, info);

        // Leaf test
        if (i == n || sol_curr.remaining_capacity() < min_weight[i]) {
            do { // Backtrack
                i--;
                if (i < 0) {
                    PUT(info, "Algorithm.NodeNumber", node_number);
                    return algorithm_end(sol_best, info);
                }
            } while (!sol_curr.contains(i));
        }

        if (!sol_curr.contains(i) // Add item
                && sol_curr.remaining_capacity() >= ins.item(i).w
                && ub_0(ins, i+1, // Upper bound test
                    sol_curr.profit()             + ins.item(i).p,
                    sol_curr.remaining_capacity() - ins.item(i).w, j_max)
                > sol_best.profit()) {
            sol_curr.set(i, true);
            if (sol_curr.profit() > sol_best.profit())
                sol_best = sol_curr; // Update best solution
            node_number++;
        } else { // Remove item
            sol_curr.set(i, false);
        }
    }
}

