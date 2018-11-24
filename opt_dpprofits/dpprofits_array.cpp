#include "knapsack/opt_dpprofits/dpprofits.hpp"

#include "knapsack/ub_dembo/dembo.hpp"

#define INDEX(j,q) (j+1)*(ub+1) + (q)

using namespace knapsack;

Profit knapsack::opt_dpprofits_array(Instance& ins, Info& info)
{
    info.verbose("*** dpprofits (array) ***\n");

    ItemIdx n = ins.item_number();
    Weight  c = ins.capacity();
    if (n == 0)
        return algorithm_end(0, info);

    // Initialize memory table
    Profit ub = ub_0(ins, 0, 0, ins.capacity());
    std::vector<Weight> values(ub+1,c+1);

    // Compute optimal value
    values[0] = 0;
    for (ItemPos j=0; j<n; ++j) {
        Profit pj = ins.item(j).p;
        Weight wj = ins.item(j).w;
        for (Profit q=ub; q>=0; --q) {
            if (q < pj)
                continue;
            Weight w = (q == pj)? wj: values[q-pj] + wj;
            if (w < values[q])
                values[q] = w;
        }
    }

    // Retrieve optimal value
    Profit opt = 0;
    for (Profit q=0; q<=ub; ++q)
        if (values[q] <= c)
            opt = q;
    return algorithm_end(opt, info);
}

/******************************************************************************/

Solution knapsack::sopt_dpprofits_array_all(Instance& ins, Info& info)
{
    info.verbose("*** dpprofits (array, all) ***\n");

    ItemIdx n = ins.item_number();
    Weight  c = ins.capacity();
    if (n == 0) {
        Solution sol(ins);
        return algorithm_end(sol, info);
    }

    // Initialize memory table
    Profit ub = ub_0(ins, 0, 0, ins.capacity());
    StateIdx values_size = (n+1)*(ub+1);
    std::vector<Weight> values(values_size);

    // Compute optimal value
    values[0] = 0;
    for (Profit q=1; q<=ub; ++q)
        values[INDEX(-1,q)] = c+1;
    for (ItemPos j=0; j<n; ++j) {
        Profit pj = ins.item(j).p;
        Profit wj = ins.item(j).w;
        for (Profit q=0; q<=ub; ++q) {
            if (q < pj) {
                values[INDEX(j,q)] = values[INDEX(j-1,q)];
                continue;
            }
            Weight v0 = values[INDEX(j-1,q)];
            Weight v1 = (q == pj)? wj: values[INDEX(j-1,q-pj)] + wj;
            values[INDEX(j,q)] = (v1 < v0)? v1: v0;
        }
    }

    // Retrieve optimal value
    Profit opt = 0;
    for (Profit q=0; q<=ub; ++q)
        if (values[INDEX(n-1,q)] <= c)
            opt = q;
    assert(ins.check_opt(opt));

    // Retrieve optimal solution
    ItemPos j = n-1;
    Profit  q = opt;
    Weight  w = values[INDEX(j,opt)];
    Solution sol = *ins.reduced_solution();
    while (w > 0) {
        Weight wj = ins.item(j).w;
        Profit pj = ins.item(j).p;
        Weight v0 = values[INDEX(j-1,q)];
        Weight v1 = (q < pj)? ins.capacity() + 1: values[INDEX(j-1,q-pj)] + wj;
        if (v1 < v0) {
            q -= pj;
            w -= wj;
            sol.set(j, true);
        }
        j--;
    }
    assert(ins.check_sopt(sol));
    return algorithm_end(sol, info);
}

/******************************************************************************/

Solution knapsack::sopt_dpprofits_array_one(Instance& ins, Info& info)
{
    info.verbose("*** dpprofits (array, one) ***\n");
    Solution sol(ins);
    info.verbose("Not yet implemented.\n");
    return algorithm_end(sol, info);
}

/******************************************************************************/

Solution knapsack::sopt_dpprofits_array_part(Instance& ins, ItemPos k, Info& info)
{
    info.verbose("*** dpprofits (array, part " + std::to_string(k) + ") ***\n");
    Solution sol(ins);
    info.verbose("Not yet implemented.\n");
    return algorithm_end(sol, info);
}

/******************************************************************************/

Solution knapsack::sopt_dpprofits_array_rec(Instance& ins, Info& info)
{
    info.verbose("*** dpprofits (array, rec) ***\n");
    Solution sol(ins);
    info.verbose("Not yet implemented.\n");
    return algorithm_end(sol, info);
}

