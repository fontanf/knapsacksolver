#include "knapsack/opt_bellman/bellman.hpp"

#include "knapsack/lib/part_solution_1.hpp"

using namespace knapsack;

#define INDEX(i,w) (i+1)*(c+1) + (w)

void opts_bellman_array(const Instance& ins, std::vector<Profit>& values,
        ItemPos n1, ItemPos n2, Weight c)
{
    std::fill(values.begin(), values.begin()+c+1, 0);
    for (ItemPos j=n1; j<=n2; ++j) {
        Weight wj = ins.item(j).w;
        Profit pj = ins.item(j).p;
        for (Weight w=c; w>=wj; w--)
            if (values[w-wj] + pj > values[w])
                values[w] = values[w-wj] + pj;
    }
}

Profit knapsack::opt_bellman_array(const Instance& ins, Info& info)
{
    info.verbose("*** bellman (array) ***\n");

    Weight  c = ins.capacity();
    ItemPos n = ins.item_number();
    std::vector<Profit> values(c+1);
    opts_bellman_array(ins, values, 0, n-1, c);

    return algorithm_end(values[c], info);
}

/******************************************************************************/

Solution knapsack::sopt_bellman_array_all(const Instance& ins, Info& info)
{
    info.verbose("*** bellman (array, all) ***\n");

    // Initialize memory table
    ItemPos n = ins.item_number();
    Weight  c = ins.capacity();
    StateIdx values_size = (n+1)*(c+1);
    std::vector<Profit> values(values_size);

    // Compute optimal value
    std::fill(values.begin(), values.begin()+c+1, 0);
    for (ItemPos j=0; j<ins.total_item_number(); ++j) {
        Weight wj = ins.item(j).w;
        for (Weight w=0; w<wj; ++w)
            values[INDEX(j,w)] = values[INDEX(j-1,w)];
        Profit pj = ins.item(j).p;
        for (Weight w=wj; w<=ins.capacity(); ++w) {
            Profit v0 = values[INDEX(j-1,w)];
            Profit v1 = values[INDEX(j-1,w-wj)] + pj;
            values[INDEX(j,w)] = (v1 > v0)? v1: v0;
        }
    }
    Profit opt = values[values_size-1];

    // Retrieve optimal solution
    Solution sol(ins);
    ItemPos j = n-1;
    Weight  w = c;
    Profit  v = 0;
    while (v < opt) {
        Weight wj = ins.item(j).w;
        Profit pj = ins.item(j).p;
        Profit v0 = values[INDEX(j-1,w)];
        Profit v1 = (w < wj)? 0: values[INDEX(j-1,w-wj)] + pj;
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

Solution knapsack::sopt_bellman_array_one(const Instance& ins, Info& info)
{
    info.verbose("*** bellman (array, one) ***\n");

    ItemPos n = ins.item_number();
    Weight  c = ins.capacity();
    Solution sol(ins);

    if (n == 0)
        return algorithm_end(sol, info);

    std::vector<Profit> values(c+1); // Initialize memory table
    ItemPos it = 0;
    Profit opt = -1;
    Profit opt_local = -1;
    while (sol.profit() != opt) {
        it++;
        DBG(info.debug(STR1(it) + STR2(n) + STR2(opt_local) + "\n");)

        ItemPos last_item = -1;

        // Initialization
        std::fill(values.begin(), values.end(), 0);

        // Recursion
        for (ItemPos j=0; j<n; ++j) {
            Weight wj = ins.item(j).w;
            Profit pj = ins.item(j).p;

            // For w == c
            if (c >= wj && values[c-wj] + pj > values[c]) {
                values[c] = values[c-wj] + pj;
                last_item = j;
                if (values[c] == opt_local) {
                    DBG(info.debug("Optimal value reached (" + STR1(j) + ")");)
                    goto end;
                }
            }

            // For other values of w
            for (Weight w=c-1; w>=wj; w--) {
                if (values[w-wj] + pj > values[w]) {
                    values[w] = values[w-wj] + pj;
                    if (values[w] == opt_local) {
                        DBG(info.debug("Optimal value reached (" + STR1(j) + ")");)
                        last_item = j;
                        goto end;
                    }
                }
            }

        }
end:

        // If first iteration, memorize optimal value
        if (n == ins.item_number()) {
            opt = values[c];
            opt_local = opt;
            DBG(info.debug(STR1(opt));)
        }

        // Update solution and instance
        DBG(info.debug(STR4(add, last_item));)
        sol.set(last_item, true);
        c -= ins.item(last_item).w;
        opt_local -= ins.item(last_item).p;
        n = last_item;
        DBG(info.debug(STR4(p(S), sol.profit()) + "\n");)
    }

    info.verbose("Iterations: " + std::to_string(it) + "\n");
    info.pt.put("Algorithm.Iterations", it);
    return algorithm_end(sol, info);
}

/******************************************************************************/

Solution knapsack::sopt_bellman_array_part(const Instance& ins, Info& info, ItemPos k)
{
    info.verbose("*** bellman (array, part " + std::to_string(k) + ") ***\n");

    assert(0 <= k && k <= 64);
    ItemPos n = ins.item_number();
    Weight  c = ins.capacity();
    Solution sol(ins);

    if (n == 0)
        return algorithm_end(sol, info);

    std::vector<Profit> values(c+1); // Initialize memory table
    std::vector<PartSol1> bisols(c+1);
    ItemPos it = 0;
    Profit opt = -1;
    Profit opt_local = -1;
    while (sol.profit() != opt) {
        it++;
        DBG(info.debug(STR1(it) + STR2(n) + STR2(opt_local) + "\n");)

        PartSolFactory1 psolf(k, n-1, 0, n-1);
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
                if (values[w-wj] + pj > values[w]) {
                    values[w] = values[w-wj] + pj;
                    bisols[w] = psolf.add(bisols[w-wj], j);
                    if (values[w] == opt_local) {
                        DBG(info.debug("Optimal value reached (" + STR1(j) + ")");)
                        w_opt = w;
                        goto end;
                    }
                }
            }

        }
end:

        // If first iteration, memorize optimal value
        if (n == ins.item_number()) {
            opt = values[w_opt];
            opt_local = opt;
            DBG(info.debug(STR2(opt));)
        }

        DBG(info.debug(" partsol " + psolf.print(bisols[w_opt]));)

        // Update solution and instance
        sol.update_from_partsol(psolf, bisols[w_opt]);
        n -= psolf.size();
        c = ins.capacity() - sol.weight();
        opt_local = opt - sol.profit();
        DBG(info.debug(STR4(p(S), sol.profit()) + "\n");)
    }

    info.verbose("Iterations: " + std::to_string(it) + "\n");
    info.pt.put("Algorithm.Iterations", it);
    return algorithm_end(sol, info);
}

/******************************************************************************/

struct RecData
{
    RecData(const Instance& ins, Info& info):
        ins(ins), n1(0), n2(ins.item_number()-1),
        c(ins.capacity()), sol_curr(ins), info(info)
    {
        values1.resize(c+1);
        values2.resize(c+1);
    }
    const Instance& ins;
    ItemPos n1;
    ItemPos n2;
    Weight  c;
    Solution sol_curr;
    std::vector<Profit> values1;
    std::vector<Profit> values2;
    Info& info;
};

void sopt_bellman_array_rec_rec(RecData& d)
{
    DBG(d.info.debug("Rec" + STR2(d.n1) + STR2(d.n2) + STR2(d.c) + "\n");)
    ItemPos k = (d.n1 + d.n2) / 2;
    DBG(d.info.debug(STR1(k) + "\n");)
    ItemPos n1 = d.n1;
    ItemPos n2 = d.n2;
    opts_bellman_array(d.ins, d.values1, d.n1, k, d.c);
    opts_bellman_array(d.ins, d.values2, k+1, d.n2, d.c);

    Profit z_max  = -1;
    Profit z2_opt = -1;
    Weight c1_opt = 0;
    Weight c2_opt = 0;
    for (Weight c1=0; c1<=d.c; ++c1) {
        Weight c2 = d.c - c1;
        Profit z = d.values1[c1] + d.values2[c2];
        if (z > z_max) {
            z_max = z;
            c1_opt = c1;
            c2_opt = c2;
            z2_opt = d.values2[c2];
        }
    }
    assert(z_max != -1);
    DBG(d.info.debug(STR1(c1_opt) + STR2(c2_opt) + "\n");)

    if (k == n1) {
        DBG(d.info.debug("k == n1");)
        if (d.values1[c1_opt] == d.ins.item(n1).p) {
            DBG(d.info.debug(STR4(set, n1));)
            d.sol_curr.set(n1, true);
        }
        DBG(d.info.debug("\n");)
    } else {
        d.n2 = k;
        d.c  = c1_opt;
        DBG(d.info.debug("\n");)
        sopt_bellman_array_rec_rec(d);
        DBG(d.info.debug("\n<- Rec" + STR2(n1) + STR2(n2) + "\n");)
    }

    if (k+1 == n2) {
        DBG(d.info.debug("k+1 == n2");)
        if (z2_opt == d.ins.item(n2).p) {
            DBG(d.info.debug(STR4(set, n2));)
            d.sol_curr.set(n2, true);
        }
        DBG(d.info.debug("\n");)
    } else {
        d.n1 = k+1;
        d.n2 = n2;
        d.c  = c2_opt;
        DBG(d.info.debug("\n");)
        sopt_bellman_array_rec_rec(d);
        DBG(d.info.debug("\n<- Rec" + STR2(n1) + STR2(n2) + "\n");)
    }
}

Solution knapsack::sopt_bellman_array_rec(const Instance& ins, Info& info)
{
    info.verbose("*** bellman (array, rec) ***\n");

    if (ins.item_number() == 0) {
        Solution sol(ins);
        return algorithm_end(sol, info);
    }

    if (ins.item_number() == 1) {
        Solution sol(ins);
        sol.set(0, true);
        return algorithm_end(sol, info);
    }

    RecData data(ins, info);
    sopt_bellman_array_rec_rec(data);
    return algorithm_end(data.sol_curr, info);
}

