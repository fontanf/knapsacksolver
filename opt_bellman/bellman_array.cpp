#include "bellman_array.hpp"

#define INDEX(i,w) (i+1)*(c+1) + (w)

void opts_bellman_array(const Instance& ins, std::vector<Profit>& values,
        ItemPos n1, ItemPos n2, Weight c)
{
    for (Weight w=c+1; w-->0;)
        values[w] = 0;
    for (ItemPos i=n1; i<=n2; ++i) {
        Weight wi = ins.item(i).w;
        Profit pi = ins.item(i).p;
        for (Weight w=c+1; w-->0;)
            if (w >= wi && values[w-wi] + pi > values[w])
                values[w] = values[w-wi] + pi;
    }
}

Profit opt_bellman_array(const Instance& ins, Info* info)
{
    (void)info;
    Weight  c = ins.capacity();
    ItemPos n = ins.item_number();
    std::vector<Profit> values(c+1);
    opts_bellman_array(ins, values, 0, n-1, c);
    assert(ins.check_opt(values[c]));
    return values[c];
}

/******************************************************************************/

Solution sopt_bellman_array_all(const Instance& ins, Info* info)
{
    (void)info;

    // Initialize memory table
    ItemPos n = ins.item_number();
    Weight  c = ins.capacity();
    StateIdx values_size = (n+1)*(c+1);
    std::vector<Profit> values(values_size);

    // Compute optimal value
    for (Weight w=0; w<=ins.capacity(); ++w)
        values[INDEX(-1,w)] = 0;
    for (ItemPos i=0; i<ins.item_number(); ++i) {
        Weight wi = ins.item(i).w;
        for (Weight w=0; w<=ins.capacity(); ++w) {
            Profit v0 = values[INDEX(i-1,w)];
            Profit v1 = (w < wi)? 0: values[INDEX(i-1,w-wi)] + ins.item(i).p;
            values[INDEX(i,w)] = (v1 > v0)? v1: v0;
        }
    }
    Profit opt = values[values_size-1];

    // Retrieve optimal solution
    Solution solution(ins);
    ItemPos i = n-1;
    Weight  w = c;
    Profit  v = 0;
    while (v < opt) {
        Weight wi = ins.item(i).w;
        Profit pi = ins.item(i).p;
        Profit v0 = values[INDEX(i-1,w)];
        Profit v1 = (w < wi)? 0: values[INDEX(i-1,w-wi)] + pi;
        if (v1 > v0) {
            v += pi;
            w -= wi;
            solution.set(i, true);
        }
        i--;
    }
    assert(ins.check_sol(solution));
    return solution;
}

/******************************************************************************/

#define DBG(x)
//#define DBG(x) x

Solution sopt_bellman_array_one(const Instance& ins, Info* info)
{
    ItemPos n = ins.item_number();
    Weight  c = ins.capacity();
    Solution solution(ins);

    if (n == 0)
        return solution;

    std::vector<Profit> values(c+1); // Initialize memory table
    ItemPos iter = 0;
    Profit opt = -1;
    Profit opt_local = -1;
    while (solution.profit() != opt) {
        DBG(std::cout << "N " << n << " OPT " << opt_local << std::flush;)
        iter++;
        std::pair<ItemPos, Weight> idx_opt = {n, c};
        ItemPos last_item = -1;

        // Initialization
        for (Weight w=c; w>=0; w--)
            values[w] = 0;

        // Recursion
        for (ItemPos i=0; i<n; ++i) {
            Weight wi = ins.item(i).w;
            Profit pi = ins.item(i).p;

            // For w == c
            if (c >= wi && values[c-wi] + pi > values[c]) {
                values[c] = values[c-wi] + pi;
                last_item = i;
                if (values[c] == opt_local) {
                    DBG(std::cout << " OPT REACHED " << i << std::flush;)
                    idx_opt = {i,c};
                    goto end;
                }
            }

            // For other values of w
            for (Weight w=c; w>=0; w--) {
                if (w >= wi && values[w-wi] + pi > values[w]) {
                    values[w] = values[w-wi] + pi;
                    if (values[w] == opt_local) {
                        DBG(std::cout << " OPT REACHED " << i << std::flush;)
                        idx_opt = {i,w};
                        goto end;
                    }
                }
            }

        }
end:

        // If first iteration, memorize optimal value
        if (n == ins.item_number()) {
            opt = values[c];
            DBG(std::cout << " OPT " << opt << std::flush;)
            opt_local = values[c];
        }

        DBG(std::cout << " LAST ITEM " << last_item << std::flush;)

        // Update solution and instance
        DBG(std::cout << " ADD" << last_item << std::flush;)
        solution.set(last_item, true);
        c -= ins.item(last_item).w;
        opt_local -= ins.item(last_item).p;
        n = last_item;
        DBG(std::cout << " P " << solution.profit() << std::endl;)
    }

    if (info != NULL) {
        info->pt.put("Solution.Iterations", iter);
    }
    assert(ins.check_sopt(solution));
    return solution;
}

#undef DBG

/******************************************************************************/

Solution sopt_bellman_array_part(const Instance& ins, Info* info)
{
    (void)info;
    assert(false); // TODO
    return Solution(ins);
}

/******************************************************************************/

#define DBG(x)
//#define DBG(x) x

struct RecData
{
    RecData(const Instance& ins):
        ins(ins), n1(0), n2(ins.item_number()-1),
        c(ins.capacity()), sol_curr(ins)
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
};

void sopt_bellman_array_rec_rec(RecData& d)
{
    DBG(std::cout << "Rec n1 " << d.n1 << " n2 " << d.n2 << " c " << d.c << std::endl;)
    DBG(std::cout << d.sol_curr << std::endl;)
    ItemPos k = (d.n1 + d.n2) / 2;
    DBG(std::cout << "k " << k << std::endl;)
    ItemPos n2 = d.n2;
    opts_bellman_array(d.ins, d.values1, d.n1, k, d.c);
    opts_bellman_array(d.ins, d.values2, k+1, d.n2, d.c);

    DBG(std::cout << "Find" << std::endl;)
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
    DBG(std::cout << "c1 " << c1_opt << " c2 " << c2_opt << std::endl;)

    DBG(std::cout << "Conquer" << std::endl;)
    if (k == d.n1) {
        DBG(std::cout << "Leaf" << std::endl;)
        if (d.values1[c1_opt] == d.ins.item(d.n1).p) {
            DBG(std::cout << "Set " << d.n1 << std::endl;)
            d.sol_curr.set(d.n1, true);
        }
    } else {
        DBG(std::cout << "..." << std::endl;)
        d.n2 = k;
        d.c  = c1_opt;
        sopt_bellman_array_rec_rec(d);
    }

    if (k+1 == n2) {
        DBG(std::cout << "Leaf" << std::endl;)
        if (z2_opt == d.ins.item(n2).p) {
            DBG(std::cout << "Set " << n2 << std::endl;)
            d.sol_curr.set(n2, true);
        }
    } else {
        DBG(std::cout << "..." << std::endl;)
        d.n1 = k+1;
        d.n2 = n2;
        d.c  = c2_opt;
        sopt_bellman_array_rec_rec(d);
    }
}

Solution sopt_bellman_array_rec(const Instance& ins, Info* info)
{
    (void)info;

    if (ins.item_number() == 0)
        return Solution(ins);

    if (ins.item_number() == 1) {
        Solution solution(ins);
        solution.set(0, true);
        return solution;
    }

    RecData data(ins);
    sopt_bellman_array_rec_rec(data);
    DBG(std::cout << data.sol_curr << std::endl;)
    assert(ins.check_sopt(data.sol_curr));
    return data.sol_curr;
}

#undef DBG

