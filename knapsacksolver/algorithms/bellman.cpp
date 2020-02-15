#include "knapsacksolver/algorithms/bellman.hpp"

#include "knapsacksolver/part_solution_1.hpp"
#include "knapsacksolver/algorithms/dembo.hpp"
#include "knapsacksolver/algorithms/dantzig.hpp"
#include "knapsacksolver/algorithms/greedynlogn.hpp"

#include <thread>

using namespace knapsacksolver;

#define INDEX(i,w) (i+1)*(c+1) + (w)

/******************************* bellman_array ********************************/

Output knapsacksolver::bellman_array(const Instance& ins, Info info)
{
    VER(info, "*** bellman (array) ***" << std::endl);
    Output output(ins, info);
    Weight c = ins.capacity();
    std::vector<Profit> values(c + 1, 0);
    for (ItemPos j=0; j<ins.item_number(); ++j) {
        // Check time
        if (!info.check_time()) {
            output.algorithm_end(info);
            return output;
        }

        // Update DP table
        Weight wj = ins.item(j).w;
        Profit pj = ins.item(j).p;
        for (Weight w=c; w>=wj; w--)
            if (values[w] < values[w - wj] + pj)
                values[w] = values[w - wj] + pj;

        // Update lower bound
        if (output.lower_bound < values[c]) {
            std::stringstream ss;
            ss << "it " << j;
            output.update_lb(values[c], ss, info);
        }
    }

    // Update upper bound
    output.update_ub(values[c], std::stringstream("tree search completed"), info);

    output.algorithm_end(info);
    return output;
}

/****************************** bellmanpar_array ******************************/

void bellmanpar_array_worker(const Instance& ins, ItemPos n1, ItemPos n2,
        std::vector<Profit>::iterator values, Info info)
{
    for (ItemPos j=n1; j<n2; ++j) {
        if (!info.check_time())
            return;
        Weight wj = ins.item(j).w;
        Profit pj = ins.item(j).p;
        for (Weight w=ins.capacity(); w>=wj; w--)
            if (*(values + w) < *(values + w - wj) + pj)
                *(values + w) = *(values + w - wj) + pj;
    }
}

Output knapsacksolver::bellmanpar_array(const Instance& ins, Info info)
{
    VER(info, "*** bellmanpar (array) ***" << std::endl);
    Output output(ins, info);
    ItemIdx n = ins.item_number();

    // Trivial cases
    if (n == 0) {
        output.update_ub(0, std::stringstream("no item"), info);
        output.algorithm_end(info);
        return output;
    } else if (n == 1) {
        output.update_lb(ins.item(0).p, std::stringstream("one item"), info);
        output.update_ub(ins.item(0).p, std::stringstream(""), info);
        output.algorithm_end(info);
        return output;
    }

    // Partition items and solve both knapsacks
    Weight c = ins.capacity();
    ItemIdx k = (n - 1) / 2 + 1;
    std::vector<Profit> values1(c + 1, 0);
    std::thread thread(bellmanpar_array_worker, std::ref(ins), 0, k, values1.begin(), info);
    std::vector<Profit> values2(c + 1, 0);
    bellmanpar_array_worker(std::ref(ins), k, n, values2.begin(), info);
    thread.join();
    if (!info.check_time()) {
        output.algorithm_end(info);
        return output;
    }

    // Compute optimum
    Profit opt = -1;
    for (Weight c1=0; c1<=c; ++c1) {
        Profit z = values1[c1] + values2[c - c1];
        if (opt < z)
            opt = z;
    }

    output.update_lb(opt, std::stringstream("tree search completed (lb)"), info);
    output.update_ub(opt, std::stringstream("tree search completed (ub)"), info);
    output.algorithm_end(info);
    return output;
}

/****************************** bellmanrec_rec ********************************/

Profit bellmanrec_rec(const Instance& ins,
        std::vector<Profit>& values, ItemIdx j, Weight w,
        Info& info)
{
    if (!info.check_time())
        return -1;
    Weight c = ins.capacity();
    if (values[INDEX(j, w)] == -1) {
        if (j == -1) {
            values[INDEX(j, w)] = 0;
        } else if (ins.item(j).w > w) {
            Profit p1 = bellmanrec_rec(ins, values, j - 1, w, info);
            values[INDEX(j, w)] = p1;
        } else {
            Profit p1 = bellmanrec_rec(ins, values, j - 1, w, info);
            Profit p2 = bellmanrec_rec(ins, values, j - 1, w - ins.item(j).w, info) + ins.item(j).p;
            values[INDEX(j, w)] = std::max(p1, p2);
        }
    }
    return values[INDEX(j, w)];
}

Output knapsacksolver::bellmanrec(const Instance& ins, Info info)
{
    VER(info, "*** bellmanrec ***" << std::endl);
    Output output(ins, info);

    // Initialize memory table
    ItemPos n = ins.item_number();
    Weight  c = ins.capacity();
    StateIdx values_size = (n + 1) * (c + 1);
    std::vector<Profit> values(values_size, -1);

    // Compute recursively
    Profit opt = bellmanrec_rec(ins, values, n - 1, c, info);
    if (!info.check_time()) {
        output.algorithm_end(info);
        return output;
    }
    output.update_lb(opt, std::stringstream("tree search completed (lb)"), info);
    output.update_ub(opt, std::stringstream("tree search completed (ub)"), info);

    // Retrieve optimal solution
    Weight w = c;
    Solution sol(ins);
    for (ItemPos j=n-1; j>=0; --j) {
        if (values[INDEX(j, w)] != values[INDEX(j - 1, w)]) {
            w -= ins.item(j).w;
            sol.set(j, true);
        }
    }
    output.update_sol(sol, std::stringstream(), info);

    output.algorithm_end(info);
    return output;
}

/***************************** bellman_array_all ******************************/

Output knapsacksolver::bellman_array_all(const Instance& ins, Info info)
{
    VER(info, "*** bellman (array, all) ***" << std::endl);
    Output output(ins, info);

    // Initialize memory table
    ItemPos n = ins.item_number();
    Weight  c = ins.capacity();
    StateIdx values_size = (n + 1) * (c + 1);
    std::vector<Profit> values(values_size);

    // Compute optimal value
    std::fill(values.begin(), values.begin() + c + 1, 0);
    for (ItemPos j=0; j<ins.item_number(); ++j) {
        // Check time
        if (!info.check_time()) {
            output.algorithm_end(info);
            return output;
        }

        // Fill DP table
        Weight wj = ins.item(j).w;
        for (Weight w=0; w<wj; ++w)
            values[INDEX(j, w)] = values[INDEX(j-1, w)];
        Profit pj = ins.item(j).p;
        for (Weight w=wj; w<=c; ++w) {
            Profit v0 = values[INDEX(j - 1 ,w)];
            Profit v1 = values[INDEX(j - 1, w - wj)] + pj;
            values[INDEX(j, w)] = std::max(v1, v0);
        }

        // Update lower bound
        if (output.lower_bound < values[INDEX(j, c)]) {
            std::stringstream ss;
            ss << "it " << j;
            output.update_lb(values[INDEX(j, c)], ss, info);
        }
    }

    // Update upper bound
    output.update_ub(output.lower_bound, std::stringstream("tree search completed"), info);

    // Retrieve optimal solution
    Weight w = c;
    Solution sol(ins);
    for (ItemPos j=n-1; j>=0; --j) {
        if (values[INDEX(j, w)] != values[INDEX(j - 1, w)]) {
            w -= ins.item(j).w;
            sol.set(j, true);
        }
    }
    output.update_sol(sol, std::stringstream(), info);

    output.algorithm_end(info);
    return output;
}

/***************************** bellman_array_one ******************************/

Output knapsacksolver::bellman_array_one(const Instance& ins, Info info)
{
    VER(info, "*** bellman (array, one) ***" << std::endl);
    Output output(ins, info);

    ItemPos n = ins.item_number();
    Weight  c = ins.capacity();

    if (n == 0) {
        output.update_ub(0, std::stringstream("no item"), info);
        output.algorithm_end(info);
        return output;
    }

    std::vector<Profit> values(c+1); // Initialize memory table
    ItemPos it = 0;
    Profit opt = -1;
    Profit opt_local = -1;
    Solution sol(ins);
    while (sol.profit() != opt) {
        it++;
        LOG(info, "it " << it << " n " << n << " opt_local " << opt_local << std::endl);

        ItemPos last_item = -1;

        // Initialization
        std::fill(values.begin(), values.end(), 0);

        // Recursion
        for (ItemPos j=0; j<n; ++j) {
            // Check time
            if (!info.check_time()) {
                output.algorithm_end(info);
                return output;
            }

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

            // Update lower bound
            if (output.lower_bound < values[c]) {
                std::stringstream ss;
                ss << "it " << j;
                output.update_lb(values[c], ss, info);
            }
        }
end:

        // If first iteration, memorize optimal value
        if (n == ins.item_number()) {
            // Update upper bound
            output.update_ub(values[c], std::stringstream("tree search completed"), info);
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
    output.update_sol(sol, std::stringstream(), info);

    output.algorithm_end(info);
    PUT(info, "Algorithm", "Iterations", it);
    VER(info, "Iterations: " << it << std::endl);
    return output;
}

/**************************** bellman_array_part ******************************/

Output knapsacksolver::bellman_array_part(const Instance& ins, ItemPos k, Info info)
{
    VER(info, "*** bellman (array, part " << k << ") ***" << std::endl);
    Output output(ins, info);

    assert(0 <= k && k <= 64);
    ItemPos n = ins.item_number();
    Weight  c = ins.capacity();
    Solution sol(ins);

    if (n == 0) {
        output.update_ub(0, std::stringstream("no item"), info);
        output.algorithm_end(info);
        return output;
    }

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
            if (!info.check_time())
                goto end;

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

            // Update lower bound
            if (output.lower_bound < values[c]) {
                std::stringstream ss;
                ss << "it " << j;
                output.update_lb(values[c], ss, info);
            }
        }
end:

        // If first iteration, memorize optimal value
        if (n == ins.item_number()) {
            // Update upper bound
            output.update_ub(values[c], std::stringstream("tree search completed"), info);
            opt = values[w_opt];
            opt_local = opt;
            LOG(info, "opt " << opt);
        }

        LOG(info, " partsol " << psolf.print(bisols[w_opt]));

        // Update solution and instance
        psolf.update_solution(bisols[w_opt], sol);
        n -= psolf.size();
        c = ins.capacity() - sol.weight();
        opt_local = opt - sol.profit();
        LOG(info, " p(S) " << sol.profit() << std::endl);
    }
    output.update_sol(sol, std::stringstream(), info);

    output.algorithm_end(info);
    PUT(info, "Algorithm", "Iterations", it);
    VER(info, "Iterations: " << it << std::endl);
    return output;
}

/***************************** bellman_array_rec ******************************/

struct RecData
{
    const Instance& ins;
    ItemPos n1;
    ItemPos n2;
    Weight  c;
    Solution& sol;
    std::vector<Profit>::iterator values;
    Info& info;
};

void opts_bellman_array(const Instance& ins, ItemPos n1, ItemPos n2, Weight c,
        std::vector<Profit>::iterator values, Info& info)
{
    std::fill(values, values + c + 1, 0);
    for (ItemPos j=n1; j<n2; ++j) {
        if (!info.check_time())
            break;
        Weight wj = ins.item(j).w;
        Profit pj = ins.item(j).p;
        for (Weight w=c; w>=wj; w--)
            if (*(values + w) < *(values + w - wj) + pj)
                *(values + w) = *(values + w - wj) + pj;
    }
}

void bellman_array_rec_rec(RecData d)
{
    ItemPos k = (d.n1 + d.n2 - 1) / 2 + 1;
    std::vector<Profit>::iterator values_2 = d.values + d.c + 1;
    LOG(d.info, "n1 " << d.n1 << " k " << k << " n2 " << d.n2 << " c " << d.c << std::endl);

    opts_bellman_array(d.ins, d.n1, k, d.c, d.values, d.info);
    opts_bellman_array(d.ins, k, d.n2, d.c, values_2, d.info);

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
        bellman_array_rec_rec({
                .ins = d.ins,
                .n1 = d.n1,
                .n2 = k,
                .c = c1_opt,
                .sol = d.sol,
                .values = d.values,
                .info = d.info});
    if (k != d.n2 - 1)
        bellman_array_rec_rec({
                .ins = d.ins,
                .n1 = k,
                .n2 = d.n2,
                .c = c2_opt,
                .sol = d.sol,
                .values = d.values + 2 * c1_opt + k - d.n1,
                .info = d.info});
}

Output knapsacksolver::bellman_array_rec(const Instance& ins, Info info)
{
    VER(info, "*** bellman (array, rec) ***" << std::endl);
    Output output(ins, info);

    if (ins.item_number() == 0) {
        output.update_ub(0, std::stringstream("no item"), info);
        output.algorithm_end(info);
        return output;
    } else if (ins.item_number() == 1) {
        Solution sol(ins);
        sol.set(0, true);
        output.update_sol(sol, std::stringstream("one item (lb)"), info);
        output.update_ub(sol.profit(), std::stringstream("one item (ub)"), info);
        output.algorithm_end(info);
        return output;
    }

    std::vector<Profit> values(2 * ins.capacity() + ins.item_number());
    Solution sol(ins);
    bellman_array_rec_rec({
        .ins = ins,
        .n1 = 0,
        .n2 = ins.item_number(),
        .c = ins.capacity(),
        .sol = sol,
        .values = values.begin(),
        .info = info
    });
    if (!info.check_time()) {
        output.algorithm_end(info);
        return output;
    }
    output.update_sol(sol, std::stringstream("tree search completed (lb)"), info);
    output.update_ub(sol.profit(), std::stringstream("tree search completed (ub)"), info);
    output.algorithm_end(info);
    return output;
}

/******************************** bellman_list ********************************/

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

Output knapsacksolver::bellman_list(Instance& ins, bool sort, Info info)
{
    VER(info, "*** bellman (list" << ((sort)? ", sort": "") << ") ***" << std::endl);
    LOG_FOLD_START(info, "bellman sort " << sort << std::endl);
    Output output(ins, info);

    Weight  c = ins.capacity();
    ItemPos n = ins.item_number();
    ItemPos j_max = ins.max_efficiency_item(info);

    if (n == 0 || c == 0) {
        output.update_ub(0, std::stringstream("no item or null capacity"), info);
        output.algorithm_end(info);
        LOG_FOLD_END(info, "no item or null capacity");
        return output;
    }

    if (!sort && INT_FAST64_MAX / ins.item(j_max).p < ins.capacity()) {
        output.algorithm_end(info);
        LOG_FOLD_END(info, "");
        return output;
    }

    if (sort) {
        ins.sort(info);
        if (ins.break_item() == ins.last_item() + 1) {
            output.update_lb(ins.break_solution()->profit(), std::stringstream("all items fit in the knapsack (lb)"), info);
            output.update_ub(output.lower_bound, std::stringstream("all item fit in the knapsack (ub)"), info);
            output.algorithm_end(info);
            LOG_FOLD_END(info, "all items fit in the knapsack");
            return output;
        }
        auto g_output = greedynlogn(ins);
        output.update_sol(g_output.solution, std::stringstream("greedynlogn"), info);

        ins.reduce2(output.lower_bound, info);
        if (ins.reduced_capacity() < 0) {
            output.update_ub(output.lower_bound, std::stringstream("negative capacity after reduction"), info);
            output.algorithm_end(info);
            LOG_FOLD_END(info, "c < 0");
            return output;
        } else if (n == 0 || ins.reduced_capacity() == 0) {
            output.update_sol(*ins.reduced_solution(), std::stringstream("no item or null capacity after reduction (lb)"), info);
            output.update_ub(output.lower_bound, std::stringstream("no item or null capacity after reduction (ub)"), info);
            output.algorithm_end(info);
            LOG_FOLD_END(info, "no item or null capacity after reduction");
            return output;
        } else if (ins.break_item() == ins.last_item() + 1) {
            output.update_sol(*ins.break_solution(), std::stringstream("all items fit in the knapsack after reduction (lb)"), info);
            output.update_ub(output.lower_bound, std::stringstream("all items fit in the knapsack after reduction (ub)"), info);
            output.algorithm_end(info);
            LOG_FOLD_END(info, "all items fit in the knapsack after reduction");
            return output;
        }
    }

    Profit ub = (!sort)? ub_0(ins, 0, 0, ins.capacity(), j_max):
        std::max(ub_dantzig(ins), output.lower_bound);
    output.update_ub(ub, std::stringstream("initial upper bound"), info);
    std::vector<BellmanState> l0{{
        .w = (ins.reduced_solution() == NULL)? 0: ins.reduced_solution()->weight(),
        .p = (ins.reduced_solution() == NULL)? 0: ins.reduced_solution()->profit()}};
    for (ItemPos j=ins.first_item(); j<=ins.last_item() && !l0.empty(); ++j) {
        // Check time
        if (!info.check_time()) {
            output.algorithm_end(info);
            LOG_FOLD_END(info, "no time left");
            return output;
        }

        Profit ub_max = -1;
        Weight wj = ins.item(j).w;
        Profit pj = ins.item(j).p;
        std::vector<BellmanState> l;
        std::vector<BellmanState>::iterator it  = l0.begin();
        std::vector<BellmanState>::iterator it1 = l0.begin();
        while (it != l0.end() || it1 != l0.end()) {
            if (it1 != l0.end() && (it == l0.end() || it->w > it1->w + wj)) {
                BellmanState s1{.w = it1->w + wj, .p = it1->p + pj};
                if (s1.w > c)
                    break;

                Profit ub_curr = (!sort)?
                    ub_0(ins, j+1, s1.p, c - s1.w, j_max):
                    ub_dembo(ins, j+1, s1.p, c - s1.w);

                if (ub_curr > output.lower_bound && (l.empty() || s1.p > l.back().p)) {
                    // Update lower bound
                    if (output.lower_bound < s1.p) {
                        std::stringstream ss;
                        ss << "it " << j - ins.first_item();
                        output.update_lb(s1.p, ss, info);
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
                    ub_0(ins, j+1, it->p, c - it->w, j_max):
                    ub_dembo(ins, j+1, it->p, c - it->w);

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
            ss << "it " << j - ins.first_item();
            output.update_ub(ub_max, ss, info);
        }
    }
    output.update_ub(output.lower_bound, std::stringstream("tree search completed"), info);

    output.algorithm_end(info);
    LOG_FOLD_END(info, "");
    return output;
}

/***************************** bellman_list_rec *******************************/

struct BellmanListRecData
{
    const Instance& ins;
    ItemPos n1;
    ItemPos n2;
    Weight c;
    Solution& sol;
    ItemPos j_max;
    Info& info;
};

std::vector<BellmanState> opts_bellman_list(const Instance& ins,
        ItemPos n1, ItemPos n2, Weight c, ItemPos j_max, Info& info)
{
    LOG_FOLD_START(info, "solve n1 " << n1 << " n2 " << n2 << " c " << c << std::endl);
    if (c == 0) {
        LOG_FOLD_END(info, "c == 0");
        return {{0, 0}};
    }

    Profit lb = 0;
    std::vector<BellmanState> l0{{0, 0}};
    for (ItemPos j=n1; j<n2; ++j) {
        if (!info.check_time())
            break;
        Weight wj = ins.item(j).w;
        Profit pj = ins.item(j).p;
        LOG(info, "j " << j << " wj " << wj << " pj " << pj << std::endl);
        std::vector<BellmanState> l;
        std::vector<BellmanState>::iterator it = l0.begin();
        std::vector<BellmanState>::iterator it1 = l0.begin();
        while (it != l0.end() || it1 != l0.end()) {
            if (it1 != l0.end() && (it == l0.end() || it->w > it1->w + wj)) {
                BellmanState s1{it1->w+wj, it1->p+pj};
                LOG(info, "s1 " << s1);
                if (s1.w > c) {
                    LOG(info, " too large" << std::endl);
                    break;
                }
                if (s1.p > l.back().p) {
                    if (s1.p > lb) // Update lower bound
                        lb = s1.p;
                    if (s1.w == l.back().w) {
                        LOG(info, " replace" << std::endl);
                        l.back() = s1;
                    } else {
                        Profit ub = ub_0(ins, j, s1.p, c - s1.w, j_max);
                        LOG(info, " ub " << ub << " lb " << lb);
                        if (ub >= lb) {
                            l.push_back(s1);
                            LOG(info, " added" << std::endl);
                        } else {
                            LOG(info, " ×" << std::endl);
                        }
                    }
                } else {
                    LOG(info, " ×" << std::endl);
                }
                it1++;
            } else {
                LOG(info, "s0 " << *it);
                if (l.empty()) {
                    LOG(info, " added" << std::endl);
                    l.push_back(*it);
                } else {
                    if (it->p > l.back().p) {
                        if (it->w == l.back().w) {
                            LOG(info, " replace" << std::endl);
                            l.back() = *it;
                        } else {
                            LOG(info, " added" << std::endl);
                            l.push_back(*it);
                        }
                    } else {
                        LOG(info, " ×" << std::endl);
                    }
                }
                ++it;
            }
        }
        l0 = std::move(l);
    }
    LOG_FOLD_END(info, "");
    return l0;
}

void bellman_list_rec_rec(BellmanListRecData d)
{
    ItemPos k = (d.n1 + d.n2 - 1) / 2 + 1;
    LOG_FOLD_START(d.info, "rec n1 " << d.n1 << " n2 " << d.n2 << " k " << k << " c " << d.c << std::endl);

    std::vector<BellmanState> l1 = opts_bellman_list(d.ins, d.n1, k, d.c, d.j_max, d.info);
    std::vector<BellmanState> l2 = opts_bellman_list(d.ins, k, d.n2, d.c, d.j_max, d.info);
    LOG(d.info, "l1.size() " << l1.size() << " l2.size() " << l2.size() << std::endl);

    Profit z_max  = -1;
    Weight i1_opt = 0;
    Weight i2_opt = 0;
    StateIdx i2 = l2.size() - 1;
    for (StateIdx i1=0; i1<(StateIdx)l1.size(); ++i1) {
        while (l1[i1].w + l2[i2].w > d.c)
            i2--;
        LOG(d.info, "i1 " << i1 << " l1[i1].w " << l1[i1].w << " i2 " << i2 << " l2[i2].w " << l2[i2].w << std::endl);
        assert(i2 >= 0);
        Profit z = l1[i1].p + l2[i2].p;
        if (z_max < z) {
            z_max = z;
            i1_opt = i1;
            i2_opt = i2;
        }
    }
    StateIdx i1 = l1.size() - 1;
    for (StateIdx i2=0; i2<(StateIdx)l2.size(); ++i2) {
        while (l2[i2].w + l1[i1].w > d.c)
            i1--;
        Profit z = l1[i1].p + l2[i2].p;
        if (z_max < z) {
            z_max = z;
            i1_opt = i1;
            i2_opt = i2;
        }
    }
    LOG(d.info, "z_max " << z_max << std::endl );

    if (d.n1 == k - 1)
        if (l1[i1_opt].p == d.ins.item(d.n1).p)
            d.sol.set(d.n1, true);
    if (k == d.n2 - 1)
        if (l2[i2_opt].p == d.ins.item(k).p)
            d.sol.set(k, true);

    if (d.n1 != k - 1)
        bellman_list_rec_rec({
                .ins = d.ins,
                .n1 = d.n1,
                .n2 = k,
                .c = l1[i1_opt].w,
                .sol = d.sol,
                .j_max = d.j_max,
                .info = d.info});
    if (k != d.n2 - 1)
        bellman_list_rec_rec({
                .ins = d.ins,
                .n1 = k,
                .n2 = d.n2,
                .c = l2[i2_opt].w,
                .sol = d.sol,
                .j_max = d.j_max,
                .info = d.info});

    LOG_FOLD_END(d.info, "");
}

Output knapsacksolver::bellman_list_rec(const Instance& ins, Info info)
{
    LOG_FOLD_START(info, "*** bellman (list, rec) ***" << std::endl);
    VER(info, "*** bellman (list, rec) ***" << std::endl);
    Output output(ins, info);
    ItemPos n = ins.item_number();

    if (n == 0) {
        output.update_ub(0, std::stringstream("no item (ub)"), info);
        output.algorithm_end(info);
        return output;
    } else if (n == 1) {
        Solution sol(ins);
        sol.set(0, true);
        output.update_sol(sol, std::stringstream("one item (lb)"), info);
        output.update_ub(sol.profit(), std::stringstream("one item (ub)"), info);
        output.algorithm_end(info);
        LOG_FOLD_END(info, "");
        return output;
    }

    ItemPos j_max = ins.max_efficiency_item(info);
    if (INT_FAST64_MAX / ins.item(j_max).p < ins.capacity()) {
        output.algorithm_end(info);
        LOG_FOLD_END(info, "");
        return output;
    }

    Solution sol(ins);
    bellman_list_rec_rec({
        .ins = ins,
        .n1 = 0,
        .n2 = ins.item_number(),
        .c = ins.capacity(),
        .sol = sol,
        .j_max = j_max,
        .info = info});
    if (!info.check_time()) {
        output.algorithm_end(info);
        LOG_FOLD_END(info, "");
        return output;
    }

    output.update_sol(sol, std::stringstream("tree search completed (lb)"), info);
    output.update_ub(sol.profit(), std::stringstream("tree search completed (ub)"), info);
    output.algorithm_end(info);
    LOG_FOLD_END(info, "");
    return output;
}

