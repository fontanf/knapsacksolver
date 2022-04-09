#include "knapsacksolver/algorithms/dpprofits.hpp"

#include "knapsacksolver/algorithms/dembo.hpp"

#define INDEX(j,q) (j + 1) * (ub + 1) + (q)

using namespace knapsacksolver;

////////////////////////////////////////////////////////////////////////////////
/////////////////////////////// dpprofits_array ////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

Output knapsacksolver::dpprofits_array(
        const Instance& instance,
        Info info)
{
    init_display(instance, info);
    FFOT_VER(info,
               "Algorithm" << std::endl
            << "---------" << std::endl
            << "Dynamic Programming - By Profits" << std::endl
            << std::endl
            << "Parameters" << std::endl
            << "----------" << std::endl
            << "Implementation:                  iterative" << std::endl
            << "Method for retrieving solution:  no solution" << std::endl
            << std::endl);

    Output output(instance, info);

    ItemIdx n = instance.reduced_number_of_items();
    if (n == 0) {
        output.update_upper_bound(0, std::stringstream("no item"), info);
        return output.algorithm_end(info);
    }

    // Initialize memory table
    Weight c = instance.capacity();
    ItemPos j_max = instance.max_efficiency_item(FFOT_DBG(info));
    Profit ub = ub_0(instance, 0, 0, instance.capacity(), j_max);
    output.update_upper_bound(
            ub,
            std::stringstream("initial upper bound"),
            info);
    std::vector<Weight> values(ub + 1, c + 1);

    // Compute optimal value
    values[0] = 0;
    for (ItemPos j = 0; j < n; ++j) {
        // Check time
        if (!info.check_time())
            return output.algorithm_end(info);

        // Update DP table
        Profit pj = instance.item(j).p;
        Weight wj = instance.item(j).w;
        for (Profit q = ub; q >= pj; --q) {
            Weight w = (q == pj)? wj: values[q - pj] + wj;
            if (values[q] > w) {
                values[q] = w;
                // Update lower bound
                if (w <= c && output.lower_bound < q) {
                    std::stringstream ss;
                    ss << "it " << j;
                    output.update_lower_bound(q, ss, info);
                }
            }
        }
    }

    // Update upper bound
    output.update_upper_bound(
            output.lower_bound,
            std::stringstream("tree search completed"),
            info);

    return output.algorithm_end(info);
}

////////////////////////////////////////////////////////////////////////////////
///////////////////////////// dpprofits_array_all //////////////////////////////
////////////////////////////////////////////////////////////////////////////////

Output knapsacksolver::dpprofits_array_all(const Instance& instance, Info info)
{
    init_display(instance, info);
    FFOT_VER(info,
               "Algorithm" << std::endl
            << "---------" << std::endl
            << "Dynamic Programming - By Profits" << std::endl
            << std::endl
            << "Parameters" << std::endl
            << "----------" << std::endl
            << "Implementation:                  iterative" << std::endl
            << "Method for retrieving solution:  store all states" << std::endl
            << std::endl);

    Output output(instance, info);

    ItemIdx n = instance.reduced_number_of_items();
    if (n == 0) {
        output.update_upper_bound(0, std::stringstream("no item"), info);
        return output.algorithm_end(info);
    }

    // Initialize memory table
    Weight c = instance.capacity();
    ItemPos j_max = instance.max_efficiency_item(FFOT_DBG(info));
    Profit ub = ub_0(instance, 0, 0, instance.capacity(), j_max);
    output.update_upper_bound(
            ub,
            std::stringstream("initial upper bound"),
            info);
    StateIdx values_size = (n + 1) * (ub + 1);
    std::vector<Weight> values(values_size);

    // Compute optimal value
    values[0] = 0;
    for (Profit q = 1; q <= ub; ++q)
        values[INDEX(-1, q)] = c + 1;
    for (ItemPos j = 0; j < n; ++j) {
        Profit pj = instance.item(j).p;
        Profit wj = instance.item(j).w;
        for (Profit q = 0; q < pj; ++q)
            values[INDEX(j, q)] = values[INDEX(j - 1, q)];
        for (Profit q = pj; q <= ub; ++q) {
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
                    output.update_lower_bound(q, ss, info);
                }
            }
        }
    }

    // Update upper bound
    output.update_upper_bound(
            output.lower_bound,
            std::stringstream("tree search completed"),
            info);

    // Retrieve optimal solution
    Weight q = output.lower_bound;
    Solution sol(instance);
    for (ItemPos j = n - 1; j >= 0; --j) {
        if (values[INDEX(j, q)] != values[INDEX(j - 1, q)]) {
            q -= instance.item(j).p;
            sol.set(j, true);
        }
    }
    output.update_solution(sol, std::stringstream(), info);

    return output.algorithm_end(info);
}

