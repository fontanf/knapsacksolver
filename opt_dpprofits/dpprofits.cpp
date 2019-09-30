#include "knapsack/opt_dpprofits/dpprofits.hpp"

#include "knapsack/ub_dembo/dembo.hpp"

#define INDEX(j,q) (j + 1) * (ub + 1) + (q)

using namespace knapsack;

/**************************** opt_dpprofits_array *****************************/

knapsack::Output knapsack::opt_dpprofits_array(const Instance& ins, Info info)
{
    VER(info, "*** dpprofits (array) ***" << std::endl);
    knapsack::Output output(ins);
    init_display(output.lower_bound, output.upper_bound, info);

    ItemIdx n = ins.item_number();
    if (n == 0) {
        update_ub(output, 0, std::stringstream("no item"), info);
        algorithm_end(output, info);
        return output;
    }

    // Initialize memory table
    Weight c = ins.total_capacity();
    ItemPos j_max = ins.max_efficiency_item(info);
    Profit ub = ub_0(ins, 0, 0, ins.total_capacity(), j_max);
    update_ub(output, ub, std::stringstream("initial upper bound"), info);
    std::vector<Weight> values(ub + 1, c + 1);

    // Compute optimal value
    values[0] = 0;
    for (ItemPos j=0; j<n; ++j) {
        // Check time
        if (!info.check_time()) {
            algorithm_end(output, info);
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
                    update_lb(output, q, ss, info);
                }
            }
        }
    }

    // Update upper bound
    update_ub(output, output.lower_bound, std::stringstream("tree search completed"), info);

    algorithm_end(output, info);
    return output;
}

/************************** sopt_dpprofits_array_all **************************/

knapsack::Output knapsack::sopt_dpprofits_array_all(const Instance& ins, Info info)
{
    VER(info, "*** dpprofits (array, all) ***" << std::endl);
    knapsack::Output output(ins);
    init_display(output.lower_bound, output.upper_bound, info);

    ItemIdx n = ins.item_number();
    if (n == 0) {
        update_ub(output, 0, std::stringstream("no item"), info);
        algorithm_end(output, info);
        return output;
    }

    // Initialize memory table
    Weight c = ins.total_capacity();
    ItemPos j_max = ins.max_efficiency_item(info);
    Profit ub = ub_0(ins, 0, 0, ins.total_capacity(), j_max);
    update_ub(output, ub, std::stringstream("initial upper bound"), info);
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
                    update_lb(output, q, ss, info);
                }
            }
        }
    }

    // Update upper bound
    update_ub(output, output.lower_bound, std::stringstream("tree search completed"), info);

    // Retrieve optimal solution
    Weight q = output.lower_bound;
    Solution sol(ins);
    for (ItemPos j=n-1; j>=0; --j) {
        if (values[INDEX(j, q)] != values[INDEX(j - 1, q)]) {
            q -= ins.item(j).p;
            sol.set(j, true);
        }
    }
    update_sol(output, sol, std::stringstream(), info);

    algorithm_end(output, info);
    return output;
}

