#include "knapsack/opt_dpprofits/dpprofits.hpp"

#include "knapsack/ub_dembo/dembo.hpp"

#define INDEX(j,q) (j + 1) * (ub + 1) + (q)

using namespace knapsack;

/**************************** opt_dpprofits_array *****************************/

Output knapsack::opt_dpprofits_array(const Instance& ins, Info info)
{
    VER(info, "*** dpprofits (array) ***" << std::endl);
    Output output(ins, info);

    ItemIdx n = ins.reduced_item_number();
    if (n == 0) {
        output.update_ub(0, std::stringstream("no item"), info);
        output.algorithm_end(info);
        return output;
    }

    // Initialize memory table
    Weight c = ins.capacity();
    ItemPos j_max = ins.max_efficiency_item(info);
    Profit ub = ub_0(ins, 0, 0, ins.capacity(), j_max);
    output.update_ub(ub, std::stringstream("initial upper bound"), info);
    std::vector<Weight> values(ub + 1, c + 1);

    // Compute optimal value
    values[0] = 0;
    for (ItemPos j=0; j<n; ++j) {
        // Check time
        if (!info.check_time()) {
            output.algorithm_end(info);
            return output;
        }

        // Update DP table
        Profit pj = ins.item(j).p;
        Weight wj = ins.item(j).w;
        for (Profit q=ub; q>=pj; --q) {
            Weight w = (q == pj)? wj: values[q - pj] + wj;
            if (values[q] > w) {
                values[q] = w;
                // Update lower bound
                if (w <= c && output.lower_bound < q) {
                    std::stringstream ss;
                    ss << "it " << j;
                    output.update_lb(q, ss, info);
                }
            }
        }
    }

    // Update upper bound
    output.update_ub(output.lower_bound, std::stringstream("tree search completed"), info);

    output.algorithm_end(info);
    return output;
}

/************************** sopt_dpprofits_array_all **************************/

Output knapsack::sopt_dpprofits_array_all(const Instance& ins, Info info)
{
    VER(info, "*** dpprofits (array, all) ***" << std::endl);
    Output output(ins, info);

    ItemIdx n = ins.reduced_item_number();
    if (n == 0) {
        output.update_ub(0, std::stringstream("no item"), info);
        output.algorithm_end(info);
        return output;
    }

    // Initialize memory table
    Weight c = ins.capacity();
    ItemPos j_max = ins.max_efficiency_item(info);
    Profit ub = ub_0(ins, 0, 0, ins.capacity(), j_max);
    output.update_ub(ub, std::stringstream("initial upper bound"), info);
    StateIdx values_size = (n + 1) * (ub + 1);
    std::vector<Weight> values(values_size);

    // Compute optimal value
    values[0] = 0;
    for (Profit q=1; q<=ub; ++q)
        values[INDEX(-1, q)] = c + 1;
    for (ItemPos j=0; j<n; ++j) {
        Profit pj = ins.item(j).p;
        Profit wj = ins.item(j).w;
        for (Profit q=0; q<pj; ++q)
            values[INDEX(j, q)] = values[INDEX(j - 1, q)];
        for (Profit q=pj; q<=ub; ++q) {
            Weight v0 = values[INDEX(j - 1, q)];
            Weight v1 = (q == pj)? wj: values[INDEX(j - 1, q - pj)] + wj;
            values[INDEX(j, q)] = std::min(v1, v0);
            if (v0 <= v1) {
                values[INDEX(j, q)] = v0;
            } else {
                values[INDEX(j, q)] = v1;
                // Update lower bound
                if (v1 <= c && output.lower_bound < q) {
                    std::stringstream ss;
                    ss << "it " << j;
                    output.update_lb(q, ss, info);
                }
            }
        }
    }

    // Update upper bound
    output.update_ub(output.lower_bound, std::stringstream("tree search completed"), info);

    // Retrieve optimal solution
    Weight q = output.lower_bound;
    Solution sol(ins);
    for (ItemPos j=n-1; j>=0; --j) {
        if (values[INDEX(j, q)] != values[INDEX(j - 1, q)]) {
            q -= ins.item(j).p;
            sol.set(j, true);
        }
    }
    output.update_sol(sol, std::stringstream(), info);

    output.algorithm_end(info);
    return output;
}

