#include "knapsacksolver/algorithms/dynamic_programming_bellman.hpp"

#include "knapsacksolver/part_solution_1.hpp"
#include "knapsacksolver/algorithms/upper_bound_dembo.hpp"
#include "knapsacksolver/algorithms/upper_bound_dantzig.hpp"
#include "knapsacksolver/algorithms/greedy_nlogn.hpp"

#include <thread>

using namespace knapsacksolver;

#define INDEX(i,w) (i+1)*(c+1) + (w)

////////////////////////////////////////////////////////////////////////////////
////////////////////// dynamic_programming_bellman_array ///////////////////////
////////////////////////////////////////////////////////////////////////////////

Output knapsacksolver::dynamic_programming_bellman_array(
        const Instance& instance,
        Info info)
{
    init_display(instance, info);
    info.os()
            << "Algorithm" << std::endl
            << "---------" << std::endl
            << "Dynamic Programming - Bellman" << std::endl
            << std::endl
            << "Parameters" << std::endl
            << "----------" << std::endl
            << "Implementation:                  Array" << std::endl
            << "Method for retrieving solution:  No solution" << std::endl
            << std::endl;

    Output output(instance, info);
    Weight c = instance.capacity();
    std::vector<Profit> values(c + 1, 0);
    for (ItemPos j = 0; j < instance.number_of_items(); ++j) {
        // Check time
        if (!info.check_time())
            return output.algorithm_end(info);

        // Update DP table
        Weight wj = instance.item(j).w;
        Profit pj = instance.item(j).p;
        for (Weight w = c; w >= wj; w--)
            if (values[w] < values[w - wj] + pj)
                values[w] = values[w - wj] + pj;

        // Update lower bound
        if (output.lower_bound < values[c]) {
            std::stringstream ss;
            ss << "it " << j;
            output.update_lower_bound(values[c], ss, info);
        }
    }

    // Update upper bound
    output.update_upper_bound(
            values[c],
            std::stringstream("tree search completed"),
            info);

    return output.algorithm_end(info);
}

////////////////////////////////////////////////////////////////////////////////
////////////////// dynamic_programming_bellman_array_parallel //////////////////
////////////////////////////////////////////////////////////////////////////////

void dynamic_programming_bellman_array_parallel_worker(
        const Instance& instance,
        ItemPos n1,
        ItemPos n2,
        std::vector<Profit>::iterator values,
        Info info)
{
    for (ItemPos j = n1; j < n2; ++j) {
        if (!info.check_time())
            return;
        Weight wj = instance.item(j).w;
        Profit pj = instance.item(j).p;
        for (Weight w = instance.capacity(); w >= wj; w--)
            if (*(values + w) < *(values + w - wj) + pj)
                *(values + w) = *(values + w - wj) + pj;
    }
}

Output knapsacksolver::dynamic_programming_bellman_array_parallel(
        const Instance& instance,
        Info info)
{
    init_display(instance, info);
    info.os()
            << "Algorithm" << std::endl
            << "---------" << std::endl
            << "Dynamic Programming - Parallel Bellman" << std::endl
            << std::endl
            << "Parameters" << std::endl
            << "----------" << std::endl
            << "Implementation:                  Array" << std::endl
            << "Method for retrieving solution:  No solution" << std::endl
            << std::endl;

    Output output(instance, info);
    ItemIdx n = instance.number_of_items();

    // Trivial cases
    if (n == 0) {
        output.update_upper_bound(0, std::stringstream("no item"), info);
        return output.algorithm_end(info);
    } else if (n == 1) {
        output.update_lower_bound(
                instance.item(0).p,
                std::stringstream("one item"),
                info);
        output.update_upper_bound(
                instance.item(0).p,
                std::stringstream(""),
                info);
        return output.algorithm_end(info);
    }

    // Partition items and solve both knapsacks
    Weight c = instance.capacity();
    ItemIdx k = (n - 1) / 2 + 1;
    std::vector<Profit> values1(c + 1, 0);
    std::thread thread(
            dynamic_programming_bellman_array_parallel_worker,
            std::ref(instance),
            0,
            k,
            values1.begin(),
            info);
    std::vector<Profit> values2(c + 1, 0);
    dynamic_programming_bellman_array_parallel_worker(
            std::ref(instance),
            k,
            n,
            values2.begin(),
            info);
    thread.join();
    if (!info.check_time())
        return output.algorithm_end(info);

    // Compute optimum
    Profit opt = -1;
    for (Weight c1 = 0; c1 <= c; ++c1) {
        Profit z = values1[c1] + values2[c - c1];
        if (opt < z)
            opt = z;
    }

    output.update_lower_bound(
            opt,
            std::stringstream("tree search completed (lb)"),
            info);
    output.update_upper_bound(
            opt,
            std::stringstream("tree search completed (ub)"),
            info);
    return output.algorithm_end(info);
}

////////////////////////////////////////////////////////////////////////////////
/////////////////////// dynamic_programming_bellman_rec ////////////////////////
////////////////////////////////////////////////////////////////////////////////

Profit dynamic_programming_bellman_rec_rec(
        const Instance& instance,
        std::vector<Profit>& values,
        ItemIdx j,
        Weight w,
        Info& info)
{
    if (!info.check_time())
        return -1;
    Weight c = instance.capacity();
    if (values[INDEX(j, w)] == -1) {
        if (j == -1) {
            values[INDEX(j, w)] = 0;
        } else if (instance.item(j).w > w) {
            Profit p1 = dynamic_programming_bellman_rec_rec(
                    instance, values, j - 1, w, info);
            values[INDEX(j, w)] = p1;
        } else {
            Profit p1 = dynamic_programming_bellman_rec_rec(
                    instance, values, j - 1, w, info);
            Profit p2 = instance.item(j).p
                + dynamic_programming_bellman_rec_rec(
                    instance, values, j - 1, w - instance.item(j).w, info);
            values[INDEX(j, w)] = std::max(p1, p2);
        }
    }
    return values[INDEX(j, w)];
}

Output knapsacksolver::dynamic_programming_bellman_rec(
        const Instance& instance,
        Info info)
{
    init_display(instance, info);
    info.os()
            << "Algorithm" << std::endl
            << "---------" << std::endl
            << "Dynamic Programming - Bellman" << std::endl
            << std::endl
            << "Parameters" << std::endl
            << "----------" << std::endl
            << "Implementation:                  Recursive" << std::endl
            << "Method for retrieving solution:  No solution" << std::endl
            << std::endl;

    Output output(instance, info);

    // Initialize memory table
    ItemPos n = instance.number_of_items();
    Weight  c = instance.capacity();
    StateIdx values_size = (n + 1) * (c + 1);
    std::vector<Profit> values(values_size, -1);

    // Compute recursively
    Profit opt = dynamic_programming_bellman_rec_rec(
            instance, values, n - 1, c, info);
    if (!info.check_time())
        return output.algorithm_end(info);
    output.update_lower_bound(
            opt,
            std::stringstream("tree search completed (lb)"),
            info);
    output.update_upper_bound(
            opt,
            std::stringstream("tree search completed (ub)"),
            info);

    // Retrieve optimal solution
    Weight w = c;
    Solution sol(instance);
    for (ItemPos j = n - 1; j >= 0; --j) {
        if (values[INDEX(j, w)] != values[INDEX(j - 1, w)]) {
            w -= instance.item(j).w;
            sol.set(j, true);
        }
    }
    output.update_solution(sol, std::stringstream(), info);

    return output.algorithm_end(info);
}

////////////////////////////////////////////////////////////////////////////////
//////////////////// dynamic_programming_bellman_array_all /////////////////////
////////////////////////////////////////////////////////////////////////////////

Output knapsacksolver::dynamic_programming_bellman_array_all(
        const Instance& instance,
        Info info)
{
    init_display(instance, info);
    info.os()
            << "Algorithm" << std::endl
            << "---------" << std::endl
            << "Dynamic Programming - Bellman" << std::endl
            << std::endl
            << "Parameters" << std::endl
            << "----------" << std::endl
            << "Implementation:                  Array" << std::endl
            << "Method for retrieving solution:  Store all states" << std::endl
            << std::endl;

    Output output(instance, info);

    // Initialize memory table
    ItemPos n = instance.number_of_items();
    Weight  c = instance.capacity();
    StateIdx values_size = (n + 1) * (c + 1);
    std::vector<Profit> values(values_size);

    // Compute optimal value
    std::fill(values.begin(), values.begin() + c + 1, 0);
    for (ItemPos j = 0; j < instance.number_of_items(); ++j) {
        // Check time
        if (!info.check_time())
            return output.algorithm_end(info);

        // Fill DP table
        Weight wj = instance.item(j).w;
        for (Weight w = 0; w < wj; ++w)
            values[INDEX(j, w)] = values[INDEX(j-1, w)];
        Profit pj = instance.item(j).p;
        for (Weight w = wj; w <= c; ++w) {
            Profit v0 = values[INDEX(j - 1 ,w)];
            Profit v1 = values[INDEX(j - 1, w - wj)] + pj;
            values[INDEX(j, w)] = std::max(v1, v0);
        }

        // Update lower bound
        if (output.lower_bound < values[INDEX(j, c)]) {
            std::stringstream ss;
            ss << "it " << j;
            output.update_lower_bound(values[INDEX(j, c)], ss, info);
        }
    }

    // Update upper bound
    output.update_upper_bound(
            output.lower_bound,
            std::stringstream("tree search completed"),
            info);

    // Retrieve optimal solution
    Weight w = c;
    Solution sol(instance);
    for (ItemPos j = n - 1; j >= 0; --j) {
        if (values[INDEX(j, w)] != values[INDEX(j - 1, w)]) {
            w -= instance.item(j).w;
            sol.set(j, true);
        }
    }
    output.update_solution(sol, std::stringstream(), info);

    return output.algorithm_end(info);
}

////////////////////////////////////////////////////////////////////////////////
//////////////////// dynamic_programming_bellman_array_one /////////////////////
////////////////////////////////////////////////////////////////////////////////

Output knapsacksolver::dynamic_programming_bellman_array_one(
        const Instance& instance,
        Info info)
{
    init_display(instance, info);
    info.os()
            << "Algorithm" << std::endl
            << "---------" << std::endl
            << "Dynamic Programming - Bellman" << std::endl
            << std::endl
            << "Parameters" << std::endl
            << "----------" << std::endl
            << "Implementation:                  Array" << std::endl
            << "Method for retrieving solution:  Single line" << std::endl
            << std::endl;

    Output output(instance, info);

    ItemPos n = instance.number_of_items();
    Weight  c = instance.capacity();

    if (n == 0) {
        output.update_upper_bound(0, std::stringstream("no item"), info);
        return output.algorithm_end(info);
    }

    std::vector<Profit> values(c+1); // Initialize memory table
    ItemPos it = 0;
    Profit opt = -1;
    Profit opt_local = -1;
    Solution sol(instance);
    while (sol.profit() != opt) {
        it++;
        FFOT_LOG(info, "it " << it << " n " << n << " opt_local " << opt_local << std::endl);

        ItemPos last_item = -1;

        // Initialization
        std::fill(values.begin(), values.end(), 0);

        // Recursion
        for (ItemPos j = 0; j < n; ++j) {
            // Check time
            if (!info.check_time())
                return output.algorithm_end(info);

            Weight wj = instance.item(j).w;
            Profit pj = instance.item(j).p;

            // For w == c
            if (c >= wj && values[c - wj] + pj > values[c]) {
                values[c] = values[c - wj] + pj;
                last_item = j;
                if (values[c] == opt_local) {
                    FFOT_LOG(info, "Optimal value reached (j " << j << ")");
                    goto end;
                }
            }

            // For other values of w
            for (Weight w = c - 1; w >= wj; w--) {
                if (values[w - wj] + pj > values[w]) {
                    values[w] = values[w - wj] + pj;
                    if (values[w] == opt_local) {
                        FFOT_LOG(info, "Optimal value reached (j " << j << ")");
                        last_item = j;
                        goto end;
                    }
                }
            }

            // Update lower bound
            if (output.lower_bound < values[c]) {
                std::stringstream ss;
                ss << "it " << j;
                output.update_lower_bound(values[c], ss, info);
            }
        }
end:

        // If first iteration, memorize optimal value
        if (n == instance.number_of_items()) {
            // Update upper bound
            output.update_upper_bound(
                    values[c],
                    std::stringstream("tree search completed"),
                    info);
            opt = values[c];
            opt_local = opt;
            FFOT_LOG(info, "opt " << opt);
        }

        // Update solution and instancetance
        FFOT_LOG(info, " add " << last_item);
        sol.set(last_item, true);
        c -= instance.item(last_item).w;
        opt_local -= instance.item(last_item).p;
        n = last_item;
        FFOT_LOG(info, " p(S) " << sol.profit() << std::endl);
    }
    output.update_solution(sol, std::stringstream(), info);

    info.add_to_json("Algorithm", "Iterations", it);
    output.algorithm_end(info);
    info.os() << "Iterations: " << it << std::endl;
    return output;
}

////////////////////////////////////////////////////////////////////////////////
/////////////////// dynamic_programming_bellman_array_part /////////////////////
////////////////////////////////////////////////////////////////////////////////

Output knapsacksolver::dynamic_programming_bellman_array_part(
        const Instance& instance,
        ItemPos k,
        Info info)
{
    init_display(instance, info);
    info.os()
            << "Algorithm" << std::endl
            << "---------" << std::endl
            << "Dynamic Programming - Bellman" << std::endl
            << std::endl
            << "Parameters" << std::endl
            << "----------" << std::endl
            << "Implementation:                  Array" << std::endl
            << "Method for retrieving solution:  Partial solution in states" << std::endl
            << std::endl;

    Output output(instance, info);

    assert(0 <= k && k <= 64);
    ItemPos n = instance.number_of_items();
    Weight  c = instance.capacity();
    Solution sol(instance);

    if (n == 0) {
        output.update_upper_bound(0, std::stringstream("no item"), info);
        return output.algorithm_end(info);
    }

    std::vector<Profit> values(c + 1); // Initialize memory table
    std::vector<PartSol1> bisols(c + 1);
    ItemPos it = 0;
    Profit opt = -1;
    Profit opt_local = -1;
    while (sol.profit() != opt) {
        it++;
        FFOT_LOG(info, "it " << it << " n " << n << " opt_local " << opt_local << std::endl);

        PartSolFactory1 psolf(instance, k, n - 1, 0, n - 1);
        Weight w_opt = c;

        // Initialization
        std::fill(values.begin(), values.end(), 0);
        std::fill(bisols.begin(), bisols.end(), 0);

        // Recursion
        for (ItemPos j = 0; j < n; ++j) {
            if (!info.check_time())
                return output.algorithm_end(info);

            Weight wj = instance.item(j).w;
            Profit pj = instance.item(j).p;

            // For other values of w
            for (Weight w = c; w >= wj; w--) {
                if (values[w - wj] + pj > values[w]) {
                    values[w] = values[w - wj] + pj;
                    bisols[w] = psolf.add(bisols[w - wj], j);
                    if (values[w] == opt_local) {
                        FFOT_LOG(info, "Optimal value reached (j " << j << ")");
                        w_opt = w;
                        goto end;
                    }
                }
            }

            // Update lower bound
            if (output.lower_bound < values[c]) {
                std::stringstream ss;
                ss << "it " << j;
                output.update_lower_bound(values[c], ss, info);
            }
        }
end:

        // If first iteration, memorize optimal value
        if (n == instance.number_of_items()) {
            // Update upper bound
            output.update_upper_bound(
                    values[c],
                    std::stringstream("tree search completed"),
                    info);
            opt = values[w_opt];
            opt_local = opt;
            FFOT_LOG(info, "opt " << opt);
        }

        FFOT_LOG(info, " partsol " << psolf.print(bisols[w_opt]));

        // Update solution and instancetance
        psolf.update_solution(bisols[w_opt], sol);
        n -= psolf.size();
        c = instance.capacity() - sol.weight();
        opt_local = opt - sol.profit();
        FFOT_LOG(info, " p(S) " << sol.profit() << std::endl);
    }
    output.update_solution(sol, std::stringstream(), info);

    info.add_to_json("Algorithm", "Iterations", it);
    output.algorithm_end(info);
    info.os() << "Iterations: " << it << std::endl;
    return output;
}

////////////////////////////////////////////////////////////////////////////////
//////////////////// dynamic_programming_bellman_array_rec /////////////////////
////////////////////////////////////////////////////////////////////////////////

struct RecData
{
    const Instance& instance;
    ItemPos n1;
    ItemPos n2;
    Weight  c;
    Solution& sol;
    std::vector<Profit>::iterator values;
    Info& info;
};

void opts_dynamic_programming_bellman_array(
        const Instance& instance,
        ItemPos n1,
        ItemPos n2,
        Weight c,
        std::vector<Profit>::iterator values,
        Info& info)
{
    std::fill(values, values + c + 1, 0);
    for (ItemPos j = n1; j < n2; ++j) {
        if (!info.check_time())
            break;
        Weight wj = instance.item(j).w;
        Profit pj = instance.item(j).p;
        for (Weight w = c; w >= wj; w--)
            if (*(values + w) < *(values + w - wj) + pj)
                *(values + w) = *(values + w - wj) + pj;
    }
}

void dynamic_programming_bellman_array_rec_rec(RecData d)
{
    ItemPos k = (d.n1 + d.n2 - 1) / 2 + 1;
    std::vector<Profit>::iterator values_2 = d.values + d.c + 1;
    FFOT_LOG(d.info, "n1 " << d.n1 << " k " << k << " n2 " << d.n2 << " c " << d.c << std::endl);

    opts_dynamic_programming_bellman_array(d.instance, d.n1, k, d.c, d.values, d.info);
    opts_dynamic_programming_bellman_array(d.instance, k, d.n2, d.c, values_2, d.info);

    Profit z_max  = -1;
    Weight c1_opt = 0;
    Weight c2_opt = 0;
    for (Weight c1 = 0; c1 <= d.c; ++c1) {
        Weight c2 = d.c - c1;
        Profit z = *(d.values + c1) + *(values_2 + c2);
        if (z > z_max) {
            z_max = z;
            c1_opt = c1;
            c2_opt = c2;
        }
    }
    assert(z_max != -1);
    FFOT_LOG(d.info, "z_max " << z_max << " c1_opt " << c1_opt << " c2_opt " << c2_opt << std::endl);

    if (d.n1 == k - 1)
        if (*(d.values + c1_opt) == d.instance.item(d.n1).p)
            d.sol.set(d.n1, true);
    if (k == d.n2 - 1)
        if (*(values_2 + c2_opt) == d.instance.item(k).p)
            d.sol.set(k, true);

    if (d.n1 != k - 1)
        dynamic_programming_bellman_array_rec_rec({
                .instance = d.instance,
                .n1 = d.n1,
                .n2 = k,
                .c = c1_opt,
                .sol = d.sol,
                .values = d.values,
                .info = d.info});
    if (k != d.n2 - 1)
        dynamic_programming_bellman_array_rec_rec({
                .instance = d.instance,
                .n1 = k,
                .n2 = d.n2,
                .c = c2_opt,
                .sol = d.sol,
                .values = d.values + 2 * c1_opt + k - d.n1,
                .info = d.info});
}

Output knapsacksolver::dynamic_programming_bellman_array_rec(
        const Instance& instance,
        Info info)
{
    init_display(instance, info);
    info.os()
            << "Algorithm" << std::endl
            << "---------" << std::endl
            << "Dynamic Programming - Bellman" << std::endl
            << std::endl
            << "Parameters" << std::endl
            << "----------" << std::endl
            << "Implementation:                  Array" << std::endl
            << "Method for retrieving solution:  Recursive scheme" << std::endl
            << std::endl;

    Output output(instance, info);

    if (instance.number_of_items() == 0) {
        output.update_upper_bound(0, std::stringstream("no item"), info);
        return output.algorithm_end(info);
    } else if (instance.number_of_items() == 1) {
        Solution sol(instance);
        sol.set(0, true);
        output.update_solution(
                sol,
                std::stringstream("one item (lb)"),
                info);
        output.update_upper_bound(
                sol.profit(),
                std::stringstream("one item (ub)"),
                info);
        return output.algorithm_end(info);
    }

    std::vector<Profit> values(2 * instance.capacity() + instance.number_of_items());
    Solution sol(instance);
    dynamic_programming_bellman_array_rec_rec({
        .instance = instance,
        .n1 = 0,
        .n2 = instance.number_of_items(),
        .c = instance.capacity(),
        .sol = sol,
        .values = values.begin(),
        .info = info
    });
    if (!info.check_time())
        return output.algorithm_end(info);

    output.update_solution(
            sol,
            std::stringstream("tree search completed (lb)"),
            info);
    output.update_upper_bound(
            sol.profit(),
            std::stringstream("tree search completed (ub)"),
            info);
    return output.algorithm_end(info);
}

////////////////////////////////////////////////////////////////////////////////
/////////////////////// dynamic_programming_bellman_list ///////////////////////
////////////////////////////////////////////////////////////////////////////////

struct BellmanState
{
    Weight w;
    Profit p;
};

std::ostream& operator<<(std::ostream& os, const BellmanState& s)
{
    os << "(" << s.w <<  "," << s.p << ")";
    return os;
}

std::ostream& operator<<(std::ostream& os, const std::vector<BellmanState>& l)
{
    std::copy(l.begin(), l.end(), std::ostream_iterator<BellmanState>(os, " "));
    return os;
}

Output knapsacksolver::dynamic_programming_bellman_list(
        Instance& instance,
        bool sort,
        Info info)
{
    init_display(instance, info);
    info.os()
            << "Algorithm" << std::endl
            << "---------" << std::endl
            << "Dynamic Programming - Bellman" << std::endl
            << std::endl
            << "Parameters" << std::endl
            << "----------" << std::endl
            << "Implementation:                  States" << std::endl
            << "Method for retrieving solution:  No solution" << std::endl
            << "Sort:                            " << sort << std::endl
            << std::endl;

    FFOT_LOG_FOLD_START(info, "bellman sort " << sort << std::endl);
    Output output(instance, info);

    Weight  c = instance.capacity();
    ItemPos n = instance.number_of_items();
    ItemPos j_max = instance.max_efficiency_item(FFOT_DBG(info));

    if (n == 0 || c == 0) {
        output.update_upper_bound(
                0,
                std::stringstream("no item or null capacity"),
                info);
        FFOT_LOG_FOLD_END(info, "no item or null capacity");
        return output.algorithm_end(info);
    }

    if (!sort && INT_FAST64_MAX / instance.item(j_max).p < instance.capacity()) {
        FFOT_LOG_FOLD_END(info, "");
        return output.algorithm_end(info);
    }

    if (sort) {
        instance.sort(FFOT_DBG(info));
        if (instance.break_item() == instance.last_item() + 1) {
            output.update_lower_bound(
                    instance.break_solution()->profit(),
                    std::stringstream("all items fit in the knapsack (lb)"),
                    info);
            output.update_upper_bound(
                    output.lower_bound,
                    std::stringstream("all item fit in the knapsack (ub)"),
                    info);
            FFOT_LOG_FOLD_END(info, "all items fit in the knapsack");
            return output.algorithm_end(info);
        }
        auto g_output = greedy_nlogn(instance);
        output.update_solution(
                g_output.solution,
                std::stringstream("greedy_nlogn"),
                info);

        instance.reduce2(output.lower_bound, info);
        if (instance.reduced_capacity() < 0) {
            output.update_upper_bound(
                    output.lower_bound,
                    std::stringstream("negative capacity after reduction"),
                    info);
            FFOT_LOG_FOLD_END(info, "c < 0");
            return output.algorithm_end(info);
        } else if (n == 0 || instance.reduced_capacity() == 0) {
            output.update_solution(
                    *instance.reduced_solution(),
                    std::stringstream("no item or null capacity after reduction (lb)"),
                    info);
            output.update_upper_bound(
                    output.lower_bound,
                    std::stringstream("no item or null capacity after reduction (ub)"),
                    info);
            FFOT_LOG_FOLD_END(info, "no item or null capacity after reduction");
            return output.algorithm_end(info);
        } else if (instance.break_item() == instance.last_item() + 1) {
            output.update_solution(
                    *instance.break_solution(),
                    std::stringstream("all items fit in the knapsack after reduction (lb)"),
                    info);
            output.update_upper_bound(
                    output.lower_bound,
                    std::stringstream("all items fit in the knapsack after reduction (ub)"),
                    info);
            FFOT_LOG_FOLD_END(info, "all items fit in the knapsack after reduction");
            return output.algorithm_end(info);
        }
    }

    Profit ub = (!sort)? upper_bound_0(instance, 0, 0, instance.capacity(), j_max):
        std::max(upper_bound_dantzig(instance), output.lower_bound);
    output.update_upper_bound(
            ub,
            std::stringstream("initial upper bound"),
            info);
    std::vector<BellmanState> l0{{
        .w = (instance.reduced_solution() == NULL)? 0: instance.reduced_solution()->weight(),
        .p = (instance.reduced_solution() == NULL)? 0: instance.reduced_solution()->profit()}};
    for (ItemPos j = instance.first_item(); j <= instance.last_item() && !l0.empty(); ++j) {
        // Check time
        if (!info.check_time()) {
            FFOT_LOG_FOLD_END(info, "no time left");
            return output.algorithm_end(info);
        }

        Profit ub_max = -1;
        Weight wj = instance.item(j).w;
        Profit pj = instance.item(j).p;
        std::vector<BellmanState> l;
        std::vector<BellmanState>::iterator it  = l0.begin();
        std::vector<BellmanState>::iterator it1 = l0.begin();
        while (it != l0.end() || it1 != l0.end()) {
            if (it1 != l0.end() && (it == l0.end() || it->w > it1->w + wj)) {
                BellmanState s1{it1->w + wj, it1->p + pj};
                if (s1.w > c)
                    break;

                Profit ub_curr = (!sort)?
                    upper_bound_0(instance, j+1, s1.p, c - s1.w, j_max):
                    upper_bound_dembo(instance, j+1, s1.p, c - s1.w);

                if (ub_curr > output.lower_bound && (l.empty() || s1.p > l.back().p)) {
                    // Update lower bound
                    if (output.lower_bound < s1.p) {
                        std::stringstream ss;
                        ss << "it " << j - instance.first_item();
                        output.update_lower_bound(s1.p, ss, info);
                    }

                    if (ub_max < ub_curr)
                        ub_max = ub_curr;
                    if (!l.empty() && s1.w == l.back().w) {
                        l.back() = s1;
                    } else {
                        l.push_back(s1);
                    }
                }
                it1++;
            } else {
                assert(it != l0.end());

                Profit ub_curr = (!sort)?
                    upper_bound_0(instance, j+1, it->p, c - it->w, j_max):
                    upper_bound_dembo(instance, j+1, it->p, c - it->w);

                if (ub_curr > output.lower_bound && (l.empty() || it->p > l.back().p)) {
                    if (ub_max < ub_curr)
                        ub_max = ub_curr;
                    if (!l.empty() && it->w == l.back().w) {
                        l.back() = *it;
                    } else {
                        l.push_back(*it);
                    }
                }
                ++it;
            }
        }
        l0 = std::move(l);

        // Update upper bound
        if (ub_max != -1 && output.upper_bound > ub_max) {
            std::stringstream ss;
            ss << "it " << j - instance.first_item();
            output.update_upper_bound(ub_max, ss, info);
        }
    }
    output.update_upper_bound(
            output.lower_bound,
            std::stringstream("tree search completed"),
            info);

    FFOT_LOG_FOLD_END(info, "");
    return output.algorithm_end(info);
}

////////////////////////////////////////////////////////////////////////////////
//////////////////// dynamic_programming_bellman_list_rec //////////////////////
////////////////////////////////////////////////////////////////////////////////

struct BellmanListRecData
{
    const Instance& instance;
    ItemPos n1;
    ItemPos n2;
    Weight c;
    Solution& sol;
    ItemPos j_max;
    Info& info;
};

std::vector<BellmanState> opts_dynamic_programming_bellman_list(
        const Instance& instance,
        ItemPos n1,
        ItemPos n2,
        Weight c,
        ItemPos j_max,
        Info& info)
{
    FFOT_LOG_FOLD_START(info, "solve n1 " << n1 << " n2 " << n2 << " c " << c << std::endl);
    if (c == 0) {
        FFOT_LOG_FOLD_END(info, "c == 0");
        return {{0, 0}};
    }

    Profit lb = 0;
    std::vector<BellmanState> l0{{0, 0}};
    for (ItemPos j = n1; j < n2; ++j) {
        if (!info.check_time())
            break;
        Weight wj = instance.item(j).w;
        Profit pj = instance.item(j).p;
        FFOT_LOG(info, "j " << j << " wj " << wj << " pj " << pj << std::endl);
        std::vector<BellmanState> l;
        std::vector<BellmanState>::iterator it = l0.begin();
        std::vector<BellmanState>::iterator it1 = l0.begin();
        while (it != l0.end() || it1 != l0.end()) {
            if (it1 != l0.end() && (it == l0.end() || it->w > it1->w + wj)) {
                BellmanState s1{it1->w+wj, it1->p+pj};
                FFOT_LOG(info, "s1 " << s1);
                if (s1.w > c) {
                    FFOT_LOG(info, " too large" << std::endl);
                    break;
                }
                if (s1.p > l.back().p) {
                    if (s1.p > lb) // Update lower bound
                        lb = s1.p;
                    if (s1.w == l.back().w) {
                        FFOT_LOG(info, " replace" << std::endl);
                        l.back() = s1;
                    } else {
                        Profit ub = upper_bound_0(instance, j, s1.p, c - s1.w, j_max);
                        FFOT_LOG(info, " ub " << ub << " lb " << lb);
                        if (ub >= lb) {
                            l.push_back(s1);
                            FFOT_LOG(info, " added" << std::endl);
                        } else {
                            FFOT_LOG(info, " ×" << std::endl);
                        }
                    }
                } else {
                    FFOT_LOG(info, " ×" << std::endl);
                }
                it1++;
            } else {
                FFOT_LOG(info, "s0 " << *it);
                if (l.empty()) {
                    FFOT_LOG(info, " added" << std::endl);
                    l.push_back(*it);
                } else {
                    if (it->p > l.back().p) {
                        if (it->w == l.back().w) {
                            FFOT_LOG(info, " replace" << std::endl);
                            l.back() = *it;
                        } else {
                            FFOT_LOG(info, " added" << std::endl);
                            l.push_back(*it);
                        }
                    } else {
                        FFOT_LOG(info, " ×" << std::endl);
                    }
                }
                ++it;
            }
        }
        l0 = std::move(l);
    }
    FFOT_LOG_FOLD_END(info, "");
    return l0;
}

void dynamic_programming_bellman_list_rec_rec(BellmanListRecData d)
{
    ItemPos k = (d.n1 + d.n2 - 1) / 2 + 1;
    FFOT_LOG_FOLD_START(d.info, "rec n1 " << d.n1 << " n2 " << d.n2 << " k " << k << " c " << d.c << std::endl);

    std::vector<BellmanState> l1 = opts_dynamic_programming_bellman_list(
            d.instance, d.n1, k, d.c, d.j_max, d.info);
    std::vector<BellmanState> l2 = opts_dynamic_programming_bellman_list(
            d.instance, k, d.n2, d.c, d.j_max, d.info);
    FFOT_LOG(d.info, "l1.size() " << l1.size() << " l2.size() " << l2.size() << std::endl);

    Profit z_max  = -1;
    Weight i1_opt = 0;
    Weight i2_opt = 0;
    StateIdx i2 = l2.size() - 1;
    for (StateIdx i1 = 0; i1 < (StateIdx)l1.size(); ++i1) {
        while (l1[i1].w + l2[i2].w > d.c)
            i2--;
        FFOT_LOG(d.info, "i1 " << i1 << " l1[i1].w " << l1[i1].w << " i2 " << i2 << " l2[i2].w " << l2[i2].w << std::endl);
        assert(i2 >= 0);
        Profit z = l1[i1].p + l2[i2].p;
        if (z_max < z) {
            z_max = z;
            i1_opt = i1;
            i2_opt = i2;
        }
    }
    StateIdx i1 = l1.size() - 1;
    for (StateIdx i2 = 0; i2 < (StateIdx)l2.size(); ++i2) {
        while (l2[i2].w + l1[i1].w > d.c)
            i1--;
        Profit z = l1[i1].p + l2[i2].p;
        if (z_max < z) {
            z_max = z;
            i1_opt = i1;
            i2_opt = i2;
        }
    }
    FFOT_LOG(d.info, "z_max " << z_max << std::endl );

    if (d.n1 == k - 1)
        if (l1[i1_opt].p == d.instance.item(d.n1).p)
            d.sol.set(d.n1, true);
    if (k == d.n2 - 1)
        if (l2[i2_opt].p == d.instance.item(k).p)
            d.sol.set(k, true);

    if (d.n1 != k - 1)
        dynamic_programming_bellman_list_rec_rec({
                .instance = d.instance,
                .n1 = d.n1,
                .n2 = k,
                .c = l1[i1_opt].w,
                .sol = d.sol,
                .j_max = d.j_max,
                .info = d.info});
    if (k != d.n2 - 1)
        dynamic_programming_bellman_list_rec_rec({
                .instance = d.instance,
                .n1 = k,
                .n2 = d.n2,
                .c = l2[i2_opt].w,
                .sol = d.sol,
                .j_max = d.j_max,
                .info = d.info});

    FFOT_LOG_FOLD_END(d.info, "");
}

Output knapsacksolver::dynamic_programming_bellman_list_rec(
        const Instance& instance,
        Info info)
{
    info.os()
            << "Algorithm" << std::endl
            << "---------" << std::endl
            << "Dynamic Programming - Bellman" << std::endl
            << std::endl
            << "Parameters" << std::endl
            << "----------" << std::endl
            << "Implementation:                  States" << std::endl
            << "Method for retrieving solution:  Recursive scheme" << std::endl
            << std::endl;

    FFOT_LOG_FOLD_START(info, "*** bellman (list, rec) ***" << std::endl);
    Output output(instance, info);
    ItemPos n = instance.number_of_items();

    if (n == 0) {
        output.update_upper_bound(0, std::stringstream("no item (ub)"), info);
        return output.algorithm_end(info);
    } else if (n == 1) {
        Solution sol(instance);
        sol.set(0, true);
        output.update_solution(
                sol,
                std::stringstream("one item (lb)"),
                info);
        output.update_upper_bound(
                sol.profit(),
                std::stringstream("one item (ub)"),
                info);
        FFOT_LOG_FOLD_END(info, "");
        return output.algorithm_end(info);
    }

    ItemPos j_max = instance.max_efficiency_item(FFOT_DBG(info));
    if (INT_FAST64_MAX / instance.item(j_max).p < instance.capacity()) {
        FFOT_LOG_FOLD_END(info, "");
        return output.algorithm_end(info);
    }

    Solution sol(instance);
    dynamic_programming_bellman_list_rec_rec({
        .instance = instance,
        .n1 = 0,
        .n2 = instance.number_of_items(),
        .c = instance.capacity(),
        .sol = sol,
        .j_max = j_max,
        .info = info});
    if (!info.check_time()) {
        FFOT_LOG_FOLD_END(info, "");
        return output.algorithm_end(info);
    }

    output.update_solution(
            sol,
            std::stringstream("tree search completed (lb)"),
            info);
    output.update_upper_bound(
            sol.profit(),
            std::stringstream("tree search completed (ub)"),
            info);
    FFOT_LOG_FOLD_END(info, "");
    return output.algorithm_end(info);
}

