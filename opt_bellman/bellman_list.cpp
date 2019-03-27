#include "knapsack/opt_bellman/bellman.hpp"

#include "knapsack/ub_dembo/dembo.hpp"

using namespace knapsack;

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

Profit knapsack::opt_bellman_list(const Instance& ins, Info info)
{
    VER(info, "*** bellman (list) ***" << std::endl);

    Weight  c = ins.total_capacity();
    ItemPos n = ins.total_item_number();
    ItemPos j_max = ins.max_efficiency_item(info);

    if (n == 0 || c == 0)
        return algorithm_end(0, info);

    Profit lb = 0;
    std::vector<BellmanState> l0{{.w = 0, .p = 0}};
    for (ItemPos j=0; j<n; ++j) {
        if (!info.check_time())
            break;
        Weight wj = ins.item(j).w;
        Profit pj = ins.item(j).p;
        std::vector<BellmanState> l{{.w = 0, .p = 0}};
        std::vector<BellmanState>::iterator it  = l0.begin();
        std::vector<BellmanState>::iterator it1 = l0.begin();
        while (it != l0.end() || it1 != l0.end()) {
            if (it == l0.end() || it->w > it1->w + wj) {
                BellmanState s1{.w = it1->w + wj, .p = it1->p + pj};
                if (s1.w > c)
                    break;
                if (s1.p > l.back().p) {
                    if (s1.p > lb) // Update lower bound
                        lb = s1.p;
                    if (s1.w == l.back().w) {
                        l.back() = s1;
                    } else {
                        Profit ub = ub_0(ins, j+1, s1.p, c-s1.w, j_max);
                        if (ub > lb)
                            l.push_back(s1);
                    }
                }
                it1++;
            } else {
                assert(it != l0.end());
                if (it->p > l.back().p) {
                    if (it->w == l.back().w) {
                        l.back() = *it;
                    } else {
                        l.push_back(*it);
                    }
                }
                ++it;
            }
        }
        l0 = std::move(l);
    }

    return algorithm_end(lb, info);
}

/******************************************************************************/

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
        std::vector<BellmanState> l{{0, 0}};
        std::vector<BellmanState>::iterator it = l0.begin();
        std::vector<BellmanState>::iterator it1 = l0.begin();
        while (it != l0.end() || it1 != l0.end()) {
            if (it == l0.end() || it->w > it1->w + wj) {
                BellmanState s1{it1->w+wj, it1->p+pj};
                if (s1.w > c) {
                    break;
                }
                if (s1.p > l.back().p) {
                    if (s1.p > lb) // Update lower bound
                        lb = s1.p;
                    if (s1.w == l.back().w) {
                        l.back() = s1;
                    } else {
                        Profit ub = ub_0(ins, j, s1.p, c - s1.w, j_max);
                        if (ub >= lb) {
                            l.push_back(s1);
                        } else {
                        }
                    }
                } else {
                }
                it1++;
            } else {
                if (it->p > l.back().p) {
                    if (it->w == l.back().w) {
                        l.back() = *it;
                    } else {
                        l.push_back(*it);
                    }
                } else {
                }
                ++it;
            }
        }
        l0 = std::move(l);
    }
    LOG_FOLD_END(info, "");
    return l0;
}

void sopt_bellman_list_rec_rec(BellmanListRecData d)
{
    ItemPos k = (d.n1 + d.n2 - 1) / 2 + 1;
    LOG_FOLD_START(d.info, "rec n1 " << d.n1 << " n2 " << d.n2 << " k " << k << " c " << d.c << std::endl);

    std::vector<BellmanState> l1 = opts_bellman_list(d.ins, d.n1, k, d.c, d.j_max, d.info);
    std::vector<BellmanState> l2 = opts_bellman_list(d.ins, k, d.n2, d.c, d.j_max, d.info);

    Profit z_max  = -1;
    Weight i1_opt = 0;
    Weight i2_opt = 0;
    StateIdx i2 = l2.size() - 1;
    for (StateIdx i1=0; i1<(StateIdx)l1.size(); ++i1) {
        while (l1[i1].w + l2[i2].w > d.c)
            i2--;
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
        sopt_bellman_list_rec_rec({
                .ins = d.ins,
                .n1 = d.n1,
                .n2 = k,
                .c = l1[i1_opt].w,
                .sol = d.sol,
                .j_max = d.j_max,
                .info = d.info});
    if (k != d.n2 - 1)
        sopt_bellman_list_rec_rec({
                .ins = d.ins,
                .n1 = k,
                .n2 = d.n2,
                .c = l2[i2_opt].w,
                .sol = d.sol,
                .j_max = d.j_max,
                .info = d.info});

    LOG_FOLD_END(d.info, "");
}

Solution knapsack::sopt_bellman_list_rec(const Instance& ins, Info info)
{
    LOG_FOLD_START(info, "*** bellman (list, rec) ***" << std::endl);
    VER(info, "*** bellman (list, rec) ***" << std::endl);
    ItemPos n = ins.total_item_number();
    Solution sol(ins);

    if (n == 0) {
        LOG_FOLD_END(info, "");
        return algorithm_end(sol, info);
    } else if (n == 1) {
        sol.set(0, true);
        LOG_FOLD_END(info, "");
        return algorithm_end(sol, info);
    }

    sopt_bellman_list_rec_rec({
        .ins = ins,
        .n1 = 0,
        .n2 = ins.total_item_number(),
        .c = ins.total_capacity(),
        .sol = sol,
        .j_max = ins.max_efficiency_item(info),
        .info = info});

    LOG_FOLD_END(info, "");
    return algorithm_end(sol, info);
}

