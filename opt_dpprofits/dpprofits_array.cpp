#include "dpprofits_array.hpp"

#define DBG(x)
//#define DBG(x) x

#define INDEX(i,q) (i+1)*(ub+1) + (q)

Profit opt_dpprofits_array(const Instance& ins, Info* info)
{
    DBG(std::cout << "DPPROFITS..." << std::endl;)
    (void)info;

    ItemIdx n = ins.item_number();
    Weight  c = ins.capacity();
    if (n == 0)
        return 0;

    // Initialize memory table
    Profit ub = (ins.capacity() * ins.most_efficient_item().p) / ins.most_efficient_item().w;
    std::vector<Weight> values(ub+1,c+1);

    // Compute optimal value
    values[0] = 0;
    for (ItemPos i=0; i<n; ++i) {
        Profit pi = ins.item(i).p;
        Weight wi = ins.item(i).w;
        for (Profit q=ub; q>=0; --q) {
            if (q < pi)
                continue;
            Weight w = (q == pi)? wi: values[q-pi] + wi;
            if (w < values[q])
                values[q] = w;
        }
    }

    // Retrieve optimal value
    Profit opt = 0;
    for (Profit q=0; q<=ub; ++q)
        if (values[q] <= c)
            opt = q;
    assert(ins.check_opt(opt));
    DBG(std::cout << "DPPROFITS... END" << std::endl;)
    return opt;
}

/******************************************************************************/

Solution sopt_dpprofits_array_all(const Instance& ins, Info* info)
{
    (void)info;

    ItemIdx n = ins.item_number();
    Weight  c = ins.capacity();
    if (n == 0)
        return Solution(ins);

    // Initialize memory table
    Profit ub = (ins.capacity() * ins.most_efficient_item().p) / ins.most_efficient_item().w;
    StateIdx values_size = (n+1)*(ub+1);
    std::vector<Weight> values(values_size);

    // Compute optimal value
    values[0] = 0;
    for (Profit q=1; q<=ub; ++q)
        values[INDEX(-1,q)] = c+1;
    for (ItemPos i=0; i<n; ++i) {
        Profit pi = ins.item(i).p;
        Profit wi = ins.item(i).w;
        for (Profit q=0; q<=ub; ++q) {
            if (q < pi) {
                values[INDEX(i,q)] = values[INDEX(i-1,q)];
                continue;
            }
            Weight v0 = values[INDEX(i-1,q)];
            Weight v1 = (q == pi)? wi: values[INDEX(i-1,q-pi)] + wi;
            values[INDEX(i,q)] = (v1 < v0)? v1: v0;
        }
    }

    // Retrieve optimal value
    Profit opt = 0;
    for (Profit q=0; q<=ub; ++q)
        if (values[INDEX(n-1,q)] <= c)
            opt = q;
    DBG(std::cout << "OPT: " << opt << std::endl;)
    assert(ins.check_opt(opt));

    // Retrieve optimal solution
    ItemPos i = n-1;
    Profit  q = opt;
    Weight  w = values[INDEX(i,opt)];
    Solution sol = *ins.reduced_solution();
    while (w > 0) {
        DBG(std::cout << q << " " << w << " " << i << std::endl;)
        Weight wi = ins.item(i).w;
        Profit pi = ins.item(i).p;
        Weight v0 = values[INDEX(i-1,q)];
        Weight v1 = (q < pi)? ins.capacity() + 1: values[INDEX(i-1,q-pi)] + wi;
        if (v1 < v0) {
            q -= pi;
            w -= wi;
            sol.set(i, true);
        }
        i--;
    }
    assert(ins.check_sopt(sol));
    return sol;
}

/******************************************************************************/

Solution sopt_dpprofits_array_one(const Instance& ins, Info* info)
{
    (void)info;
    assert(false); // TODO
    return Solution(ins);
}

/******************************************************************************/

Solution sopt_dpprofits_array_part(const Instance& ins, ItemPos k, Info* info)
{
    (void)info;
    (void)k;
    assert(false); // TODO
    return Solution(ins);
}

/******************************************************************************/

Solution sopt_dpprofits_array_rec(const Instance& ins, Info* info)
{
    (void)info;
    assert(false); // TODO
    return Solution(ins);
}

#undef DBG
