#include "knapsack/opt_bab/bab.hpp"

#include "knapsack/ub_dembo/dembo.hpp"

using namespace knapsack;

#define DBG(x)
//#define DBG(x) x

Solution knapsack::sopt_bab(Instance& ins, Info& info)
{
    ItemIdx n = ins.item_number();
    Solution sol_curr(ins);
    if (n == 0)
        return sol_curr;

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
                if (i < 0)
                    goto end;
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
end:

    if (info.verbose)
        //std::cout << "NODES " << std::scientific << (double)node_number << std::endl;
        std::cout << "NODES " << node_number << std::endl;

    assert(ins.check_sopt(sol_best));
    return sol_best;
}

#undef DBG
