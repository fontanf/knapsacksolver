#include "knapsack/opt_bellman/bellman.hpp"

#include "knapsack/lib/part_solution_1.hpp"
#include <omp.h>
#include <thread>

using namespace knapsack;

#define INDEX(i,w) (i+1)*(c+1) + (w)

Profit knapsack::opt_bellman_array(const Instance& ins, Info info)
{
    VER(info, "*** bellman (array) ***" << std::endl);
    Weight c = ins.total_capacity();
    std::vector<Profit> values(c + 1, 0);
    for (ItemPos j=0; j<ins.total_item_number(); ++j) {
        if (!info.check_time())
            break;
        Weight wj = ins.item(j).w;
        Profit pj = ins.item(j).p;
        for (Weight w=c; w>=wj; w--)
            if (values[w] < values[w - wj] + pj)
                values[w] = values[w - wj] + pj;
    }
    return algorithm_end(values[c], info);
}

/******************************************************************************/

void opts_bellmanpar_array(const Instance& ins, ItemPos n1, ItemPos n2,
        std::vector<Profit>::iterator values, Info info)
{
    for (ItemPos j=n1; j<n2; ++j) {
        if (!info.check_time())
            break;
        Weight wj = ins.item(j).w;
        Profit pj = ins.item(j).p;
        for (Weight w=ins.total_capacity(); w>=wj; w--)
            if (*(values + w) < *(values + w - wj) + pj)
                *(values + w) = *(values + w - wj) + pj;
    }
}

Profit knapsack::opt_bellmanpar_array(const Instance& ins, Info info)
{
    VER(info, "*** bellmanpar (array) ***" << std::endl);

    ItemIdx n = ins.total_item_number();
    if (n == 0) {
        return algorithm_end(0, info);
    } else if (n == 1) {
        return algorithm_end(ins.item(0).p, info);
    }

    Weight c = ins.total_capacity();
    ItemIdx k = (n - 1) / 2 + 1;

    std::vector<Profit> values1(c + 1, 0);
    std::thread thread(opts_bellmanpar_array, std::ref(ins), 0, k, values1.begin(), info);
    std::vector<Profit> values2(c + 1, 0);
    opts_bellmanpar_array(std::ref(ins), k, n, values2.begin(), info);
    thread.join();
    if (!info.check_time())
        return -1;

    Profit opt = -1;
    for (Weight c1=0; c1<=c; ++c1) {
        Profit z = values1[c1] + values2[c - c1];
        if (opt < z)
            opt = z;
    }
    return opt;
}

/******************************************************************************/

Solution knapsack::sopt_bellman_array_all(const Instance& ins, Info info)
{
    VER(info, "*** bellman (array, all) ***" << std::endl);

    // Initialize memory table
    ItemPos n = ins.total_item_number();
    Weight  c = ins.total_capacity();
    StateIdx values_size = (n + 1) * (c + 1);
    std::vector<Profit> values(values_size);

    // Compute optimal value
    std::fill(values.begin(), values.begin() + c + 1, 0);
    for (ItemPos j=0; j<ins.total_item_number(); ++j) {
        if (!info.check_time())
            break;
        Weight wj = ins.item(j).w;
        for (Weight w=0; w<wj; ++w)
            values[INDEX(j, w)] = values[INDEX(j-1, w)];
        Profit pj = ins.item(j).p;
        for (Weight w=wj; w<=c; ++w) {
            Profit v0 = values[INDEX(j - 1 ,w)];
            Profit v1 = values[INDEX(j - 1, w - wj)] + pj;
            values[INDEX(j, w)] = (v1 > v0)? v1: v0;
        }
    }
    Profit opt = values.back();

    // Retrieve optimal solution
    Solution sol(ins);
    ItemPos j = n - 1;
    Weight  w = c;
    Profit  v = 0;
    while (v < opt) {
        Weight wj = ins.item(j).w;
        Profit pj = ins.item(j).p;
        Profit v0 = values[INDEX(j - 1, w)];
        Profit v1 = (w < wj)? 0: values[INDEX(j - 1, w - wj)] + pj;
        if (v1 > v0) {
            v += pj;
            w -= wj;
            sol.set(j, true);
        }
        j--;
    }
    return algorithm_end(sol, info);
}

/******************************************************************************/

Solution knapsack::sopt_bellman_array_one(const Instance& ins, Info info)
{
    VER(info, "*** bellman (array, one) ***" << std::endl);

    ItemPos n = ins.total_item_number();
    Weight  c = ins.total_capacity();
    Solution sol(ins);

    if (n == 0)
        return algorithm_end(sol, info);

    std::vector<Profit> values(c+1); // Initialize memory table
    ItemPos it = 0;
    Profit opt = -1;
    Profit opt_local = -1;
    while (sol.profit() != opt) {
        it++;
        LOG(info, "it " << it << " n " << n << " opt_local " << opt_local << std::endl);

        ItemPos last_item = -1;

        // Initialization
        std::fill(values.begin(), values.end(), 0);

        // Recursion
        for (ItemPos j=0; j<n; ++j) {
            Weight wj = ins.item(j).w;
            Profit pj = ins.item(j).p;

            // For w == c
            if (c >= wj && values[c - wj] + pj > values[c]) {
                values[c] = values[c - wj] + pj;
                last_item = j;
                if (values[c] == opt_local) {
                    LOG(info, "Optimal value reached (j " << j << ")");
                    goto end;
                }
            }

            // For other values of w
            for (Weight w=c-1; w>=wj; w--) {
                if (values[w - wj] + pj > values[w]) {
                    values[w] = values[w - wj] + pj;
                    if (values[w] == opt_local) {
                        LOG(info, "Optimal value reached (j " << j << ")");
                        last_item = j;
                        goto end;
                    }
                }
            }

        }
end:

        // If first iteration, memorize optimal value
        if (n == ins.total_item_number()) {
            opt = values[c];
            opt_local = opt;
            LOG(info, "opt " << opt);
        }

        // Update solution and instance
        LOG(info, " add " << last_item);
        sol.set(last_item, true);
        c -= ins.item(last_item).w;
        opt_local -= ins.item(last_item).p;
        n = last_item;
        LOG(info, " p(S) " << sol.profit() << std::endl);
    }

    VER(info, "Iterations: " << it << std::endl);
    PUT(info, "Algorithm.Iterations", it);
    return algorithm_end(sol, info);
}

/******************************************************************************/

Solution knapsack::sopt_bellman_array_part(const Instance& ins, ItemPos k, Info info)
{
    VER(info, "*** bellman (array, part " << k << ") ***" << std::endl);

    assert(0 <= k && k <= 64);
    ItemPos n = ins.total_item_number();
    Weight  c = ins.total_capacity();
    Solution sol(ins);

    if (n == 0)
        return algorithm_end(sol, info);

    std::vector<Profit> values(c + 1); // Initialize memory table
    std::vector<PartSol1> bisols(c + 1);
    ItemPos it = 0;
    Profit opt = -1;
    Profit opt_local = -1;
    while (sol.profit() != opt) {
        it++;
        LOG(info, "it " << it << " n " << n << " opt_local " << opt_local << std::endl);

        PartSolFactory1 psolf(ins, k, n - 1, 0, n - 1);
        Weight w_opt = c;

        // Initialization
        std::fill(values.begin(), values.end(), 0);
        std::fill(bisols.begin(), bisols.end(), 0);

        // Recursion
        for (ItemPos j=0; j<n; ++j) {
            Weight wj = ins.item(j).w;
            Profit pj = ins.item(j).p;

            // For other values of w
            for (Weight w=c; w>=wj; w--) {
                if (values[w - wj] + pj > values[w]) {
                    values[w] = values[w - wj] + pj;
                    bisols[w] = psolf.add(bisols[w - wj], j);
                    if (values[w] == opt_local) {
                        LOG(info, "Optimal value reached (j " << j << ")");
                        w_opt = w;
                        goto end;
                    }
                }
            }

        }
end:

        // If first iteration, memorize optimal value
        if (n == ins.total_item_number()) {
            opt = values[w_opt];
            opt_local = opt;
            LOG(info, "opt " << opt);
        }

        LOG(info, " partsol " << psolf.print(bisols[w_opt]));

        // Update solution and instance
        psolf.update_solution(bisols[w_opt], sol);
        n -= psolf.size();
        c = ins.total_capacity() - sol.weight();
        opt_local = opt - sol.profit();
        LOG(info, " p(S) " << sol.profit() << std::endl);
    }

    VER(info, "Iterations: " << it << std::endl);
    PUT(info, "Algorithm.Iterations", it);
    return algorithm_end(sol, info);
}

/******************************************************************************/

struct RecData
{
    const Instance& ins;
    ItemPos n1;
    ItemPos n2;
    Weight  c;
    Solution& sol;
    std::vector<Profit>::iterator values;
    Info info;
};

void opts_bellman_array(const Instance& ins, ItemPos n1, ItemPos n2, Weight c,
        std::vector<Profit>::iterator values)
{
    std::fill(values, values + c + 1, 0);
    for (ItemPos j=n1; j<n2; ++j) {
        Weight wj = ins.item(j).w;
        Profit pj = ins.item(j).p;
        for (Weight w=c; w>=wj; w--)
            if (*(values + w) < *(values + w - wj) + pj)
                *(values + w) = *(values + w - wj) + pj;
    }
}

void sopt_bellman_array_rec_rec(RecData d)
{
    ItemPos k = (d.n1 + d.n2 - 1) / 2 + 1;
    std::vector<Profit>::iterator values_2 = d.values + d.c + 1;
    LOG(d.info, "n1 " << d.n1 << " k " << k << " n2 " << d.n2 << " c " << d.c << std::endl);

    opts_bellman_array(d.ins, d.n1, k, d.c, d.values);
    opts_bellman_array(d.ins, k, d.n2, d.c, values_2);

    Profit z_max  = -1;
    Weight c1_opt = 0;
    Weight c2_opt = 0;
    for (Weight c1=0; c1<=d.c; ++c1) {
        Weight c2 = d.c - c1;
        Profit z = *(d.values + c1) + *(values_2 + c2);
        if (z > z_max) {
            z_max = z;
            c1_opt = c1;
            c2_opt = c2;
        }
    }
    assert(z_max != -1);
    LOG(d.info, "z_max " << z_max << " c1_opt " << c1_opt << " c2_opt " << c2_opt << std::endl);

    if (d.n1 == k - 1)
        if (*(d.values + c1_opt) == d.ins.item(d.n1).p)
            d.sol.set(d.n1, true);
    if (k == d.n2 - 1)
        if (*(values_2 + c2_opt) == d.ins.item(k).p)
            d.sol.set(k, true);

    if (d.n1 != k - 1)
        sopt_bellman_array_rec_rec({
                .ins = d.ins,
                .n1 = d.n1,
                .n2 = k,
                .c = c1_opt,
                .sol = d.sol,
                .values = d.values,
                .info = d.info
                });
    if (k != d.n2 - 1)
        sopt_bellman_array_rec_rec({
                .ins = d.ins,
                .n1 = k,
                .n2 = d.n2,
                .c = c2_opt,
                .sol = d.sol,
                .values = d.values + 2 * c1_opt + k - d.n1,
                .info = d.info
                });
}

Solution knapsack::sopt_bellman_array_rec(const Instance& ins, Info info)
{
    VER(info, "*** bellman (array, rec) ***" << std::endl);

    if (ins.total_item_number() == 0) {
        Solution sol(ins);
        return algorithm_end(sol, info);
    }

    if (ins.total_item_number() == 1) {
        Solution sol(ins);
        sol.set(0, true);
        return algorithm_end(sol, info);
    }

    Solution sol(ins);
    std::vector<Profit> values(2 * ins.total_capacity() + ins.total_item_number());
    sopt_bellman_array_rec_rec({
        .ins = ins,
        .n1 = 0,
        .n2 = ins.total_item_number(),
        .c = ins.total_capacity(),
        .sol = sol,
        .values = values.begin(),
        .info = info
    });
    return algorithm_end(sol, info);
}

