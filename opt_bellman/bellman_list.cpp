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
    BellmanListRecData(const Instance& ins, Info& info):
        ins(ins), info(info),
        n1(0), n2(ins.total_item_number()-1), c(ins.total_capacity()),
        sol_curr(ins),
        j_max(ins.max_efficiency_item(info)) {  }
    const Instance& ins;
    Info& info;
    ItemPos n1;
    ItemPos n2;
    Weight c;
    Solution sol_curr;
    ItemPos j_max;
};

std::vector<BellmanState> opts_bellman_list(BellmanListRecData& data)
{
    LOG_FOLD_START(data.info, "solve n1 " << data.n1 << " n2 " << data.n2 << " c " << data.c << std::endl);
    if (data.c == 0) {
        LOG_FOLD_END(data.info, "");
        return {{0, 0}};
    }

    Profit lb = 0;
    std::vector<BellmanState> l0{{0, 0}};
    for (ItemPos j=data.n1; j<=data.n2; ++j) {
        Weight wj = data.ins.item(j).w;
        Profit pj = data.ins.item(j).p;
        std::vector<BellmanState> l{{0, 0}};
        std::vector<BellmanState>::iterator it = l0.begin();
        std::vector<BellmanState>::iterator it1 = l0.begin();
        while (it != l0.end() || it1 != l0.end()) {
            if (it == l0.end() || it->w > it1->w + wj) {
                BellmanState s1{it1->w+wj, it1->p+pj};
                if (s1.w > data.c) {
                    break;
                }
                if (s1.p > l.back().p) {
                    if (s1.p > lb) // Update lower bound
                        lb = s1.p;
                    if (s1.w == l.back().w) {
                        l.back() = s1;
                    } else {
                        Profit ub = ub_0(data.ins, j, s1.p, data.c-s1.w, data.j_max);
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
    LOG_FOLD_END(data.info, "");
    return l0;
}

void sopt_bellman_list_rec_rec(BellmanListRecData& data)
{
    ItemPos n1 = data.n1;
    ItemPos n2 = data.n2;
    ItemPos k = (data.n1 + data.n2) / 2;

    LOG_FOLD_START(data.info, "rec n1 " << n1 << " n2 " << n2 << " k " << k << " c " << data.c << std::endl);

    Weight w1_opt = 0;
    Weight w2_opt = 0;
    Profit p1_opt = 0;
    Profit p2_opt = 0;

    {
        data.n1 = n1;
        data.n2 = k;
        std::vector<BellmanState> l1 = opts_bellman_list(data);
        data.n1 = k+1;
        data.n2 = n2;
        std::vector<BellmanState> l2 = opts_bellman_list(data);

        Profit z_max  = 0;
        Weight i1_opt = 0;
        Weight i2_opt = 0;
        if (l1.size() > 0) {
            StateIdx i2 = l2.size()-1;
            for (Weight i1=0; i1<(Weight)l1.size(); ++i1) {
                while (l1[i1].w + l2[i2].w > data.c)
                    i2--;
                assert(i2 >= 0);
                Profit z = l1[i1].p + l2[i2].p;
                if (z > z_max) {
                    z_max = z;
                    i1_opt = i1;
                    i2_opt = i2;
                }
            }
        }
        if (l2.size() > 0) {
            StateIdx i1 = l1.size()-1;
            for (Weight i2=0; i2<(Weight)l2.size(); ++i2) {
                while (l2[i2].w + l1[i1].w > data.c)
                    i1--;
                Profit z = l1[i1].p + l2[i2].p;
                if (z > z_max) {
                    z_max = z;
                    i1_opt = i1;
                    i2_opt = i2;
                }
            }
        }

        w1_opt = l1[i1_opt].w;
        w2_opt = l2[i2_opt].w;
        p1_opt = l1[i1_opt].p;
        p2_opt = l2[i2_opt].p;

        LOG(data.info, "z_max " << z_max << std::endl );
        LOG(data.info, "w1_opt " << w1_opt << " p1_opt " << p1_opt << std::endl );
        LOG(data.info, "w2_opt " << w2_opt << " p2_opt " << p2_opt << std::endl );
    }

    if (k == n1) {
        if (p1_opt == data.ins.item(n1).p)
            data.sol_curr.set(n1, true);
    } else {
        data.n1 = n1;
        data.n2 = k;
        data.c  = w1_opt;
        sopt_bellman_list_rec_rec(data);
    }

    if (k+1 == n2) {
        if (p2_opt == data.ins.item(n2).p)
            data.sol_curr.set(n2, true);
    } else {
        data.n1 = k+1;
        data.n2 = n2;
        data.c  = w2_opt;
        sopt_bellman_list_rec_rec(data);
    }
    LOG_FOLD_END(data.info, "");
}

Solution knapsack::sopt_bellman_list_rec(const Instance& ins, Info info)
{
    LOG_FOLD_START(info, "*** bellman (list, rec) ***" << std::endl);
    VER(info, "*** bellman (list, rec) ***" << std::endl);

    ItemPos n = ins.item_number();
    Solution sol(ins);

    if (n == 0) {
        LOG_FOLD_END(info, "");
        return algorithm_end(sol, info);
    } else if (n == 1) {
        sol.set(0, true);
        LOG_FOLD_END(info, "");
        return algorithm_end(sol, info);
    }

    BellmanListRecData data(ins, info);
    sopt_bellman_list_rec_rec(data);
    LOG_FOLD_END(info, "");
    return algorithm_end(data.sol_curr, info);
}

