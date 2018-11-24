#include "knapsack/opt_bab/bab.hpp"

#include "knapsack/ub_dembo/dembo.hpp"

using namespace knapsack;

Solution knapsack::sopt_bab(Instance& ins, Info& info)
{
    info.verbose("*** bab ***\n");

    ItemIdx n = ins.item_number();
    Solution sol_curr(ins);
    if (n == 0)
        return algorithm_end(sol_curr, info);

    // Compute min weight table
    std::vector<Weight> min_weight(n);
    min_weight[n-1] = ins.item(n-1).w;
    for (ItemIdx i=n-2; i>=0; --i)
        min_weight[i] = std::min(ins.item(i).w, min_weight[i+1]);

    Solution sol_best(ins);
    StateIdx node_number = 0;
    for (ItemIdx i=0; ; ++i) {
        // Leaf test
        if (i == n || sol_curr.remaining_capacity() < min_weight[i]) {
            do { // Backtrack
                i--;
                if (i < 0) {
                    info.verbose("Node number: " + Info::to_string(node_number) = "\n");
                    info.pt.put("Algorithm.NodeNumber", node_number);
                    return algorithm_end(sol_best, info);
                }
            } while (!sol_curr.contains(i));
        }

        if (!sol_curr.contains(i) // Add item
                && sol_curr.remaining_capacity() >= ins.item(i).w
                && ub_0(ins, i+1, // Upper bound test
                    sol_curr.profit()             + ins.item(i).p,
                    sol_curr.remaining_capacity() - ins.item(i).w)
                > sol_best.profit()) {
            sol_curr.set(i, true);
            sol_best.update(sol_curr); // Update best solution
            node_number++;
        } else { // Remove item
            sol_curr.set(i, false);
        }
    }
}

