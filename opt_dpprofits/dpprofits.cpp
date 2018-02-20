#include "dpprofits.hpp"

#define DBG(x)
//#define DBG(x) x

#define INDEX(i,q) (i+1)*(ub-p0+1) + (q)

Profit opt_dpprofits(const Instance& instance, Profit ub, Info* info)
{
    DBG(std::cout << "DPPROFITS..." << std::endl;)

    // Initialize memory table
    ItemIdx n = instance.item_number();
    Weight  c = instance.capacity();
    Profit p0 = instance.reduced_solution()->profit();
    std::vector<Weight> values(ub-p0+1,c+1);

    // Compute optimal value
    values[0] = 0;
    for (ItemPos i=0; i<n; ++i) {
        Profit pi = instance.item(i).p;
        Weight wi = instance.item(i).w;
        for (Profit q=ub; q>=p0; --q) {
            if (q - p0 < pi)
                continue;
            Weight w = (q - p0 == pi)? wi: values[q-pi-p0] + wi;
            if (w < values[q-p0])
                values[q-p0] = w;
        }
    }

    // Retrieve optimal value
    Profit opt = 0;
    for (Profit q=p0; q<=ub; ++q)
        if (values[q-p0] <= c)
            opt = q;
    assert(instance.check_opt(opt));
    DBG(std::cout << "DPPROFITS... END" << std::endl;)
    return opt;
}

Solution sopt_dpprofits_1(const Instance& instance, Profit ub, Info* info)
{
    // Initialize memory table
    ItemIdx n = instance.item_number();
    Weight  c = instance.capacity();
    Profit p0 = instance.reduced_solution()->profit();
    StateIdx values_size = (n+1)*(ub+1);
    std::vector<Weight> values(values_size);

    // Compute optimal value
    values[0] = 0;
    for (Profit q=p0+1; q<=ub; ++q)
        values[INDEX(-1,q)] = c+1;
    for (ItemPos i=0; i<n; ++i) {
        Profit pi = instance.item(i).p;
        Profit wi = instance.item(i).w;
        for (Profit q=p0; q<=ub; ++q) {
            if (q - p0 < pi) {
                values[INDEX(i,q)] = values[INDEX(i-1,q)];
                continue;
            }
            Weight v0 = values[INDEX(i-1,q)];
            Weight v1 = (q - p0 == pi)? wi: values[INDEX(i-1,q-pi)] + wi;
            values[INDEX(i,q)] = (v1 < v0)? v1: v0;
        }
    }

    // Retrieve optimal value
    Profit opt = 0;
    for (Profit q=p0; q<=ub; ++q)
        if (values[INDEX(n-1,q)] <= c)
            opt = q;
    DBG(std::cout << "OPT: " << opt << std::endl;)
    assert(instance.check_opt(opt));

    // Retrieve optimal solution
    ItemPos i = n-1;
    Profit  q = opt;
    Weight  w = values[INDEX(i,opt)];
    Solution sol = *instance.reduced_solution();
    while (w > 0) {
        DBG(std::cout << q << " " << w << " " << i << std::endl;)
        Weight wi = instance.item(i).w;
        Profit pi = instance.item(i).p;
        Weight v0 = values[INDEX(i-1,q)];
        Weight v1 = (q < pi)? instance.capacity() + 1: values[INDEX(i-1,q-pi)] + wi;
        if (v1 < v0) {
            q -= pi;
            w -= wi;
            sol.set(i, true);
        }
        i--;
    }
    assert(instance.check_sopt(sol));
    return sol;
}

#undef DBG
