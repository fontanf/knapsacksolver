#include "knapsack/opt_bellman/bellman.hpp"

#include "knapsack/ub_dembo/dembo.hpp"

using namespace knapsack;

struct State
{
    Weight w;
    Profit p;

    std::string to_string() const
    {
        return "(" + std::to_string(w) + "," + std::to_string(p) + ")";
    }
};

std::string to_string(std::vector<State>& l)
{
    std::string str;
    for (State& s: l)
        str += s.to_string() + " ";
    return str;
}

Profit knapsack::opt_bellman_list(const Instance& ins, Info& info)
{
    info.verbose("*** bellman (list) ***\n");

    Weight  c = ins.capacity();
    ItemPos n = ins.total_item_number();
    ItemPos j_max = ins.max_efficiency_item();

    if (n == 0 || c == 0)
        return algorithm_end(0, info);

    Profit lb = 0;
    std::vector<State> l0{{0, 0}};
    for (ItemPos j=0; j<n; ++j) {
        Weight wj = ins.item(j).w;
        Profit pj = ins.item(j).p;
        std::vector<State> l{{0, 0}};
        std::vector<State>::iterator it  = l0.begin();
        std::vector<State>::iterator it1 = l0.begin();
        while (it != l0.end() || it1 != l0.end()) {
            if (it == l0.end() || it->w > it1->w + wj) {
                State s1{it1->w+wj, it1->p+pj};
                if (s1.w > c) {
                    break;
                }
                if (s1.p > l.back().p) {
                    if (s1.p > lb) // Update lower bound
                        lb = s1.p;
                    if (s1.w == l.back().w) {
                        l.back() = s1;
                    } else {
                        Profit ub = ub_0(ins, j+1, s1.p, c-s1.w, j_max);
                        if (ub > lb) {
                            l.push_back(s1);
                        } else {
                        }
                    }
                } else {
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
                } else {
                }
                ++it;
            }
        }
        l0 = std::move(l);
    }

    return algorithm_end(lb, info);
}

/******************************************************************************/

Solution knapsack::sopt_bellman_list_all(const Instance& ins, Info& info)
{
    info.verbose("*** bellman (list, all) ***\n");
    Solution sol(ins);
    info.verbose("Not yet implemented.\n");
    return algorithm_end(sol, info);
}

/******************************************************************************/

Solution knapsack::sopt_bellman_list_one(const Instance& ins, Info& info)
{
    info.verbose("*** bellman (list, one) ***\n");
    Solution sol(ins);
    info.verbose("Not yet implemented.\n");
    return algorithm_end(sol, info);
}

/******************************************************************************/

Solution knapsack::sopt_bellman_list_part(const Instance& ins, Info& info, ItemPos k)
{
    info.verbose("*** bellman (list, part " + std::to_string(k) + ") ***\n");
    Solution sol(ins);
    info.verbose("Not yet implemented.\n");
    return algorithm_end(sol, info);
}

/******************************************************************************/

struct BellmanListRecData
{
    BellmanListRecData(const Instance& ins, Info& info):
        ins(ins), info(info),
        n1(0), n2(ins.total_item_number()-1), c(ins.total_capacity()),
        sol_curr(ins),
        j_max(ins.max_efficiency_item()) {  }
    const Instance& ins;
    Info& info;
    ItemPos n1;
    ItemPos n2;
    Weight c;
    Solution sol_curr;
    ItemPos j_max;
};

std::vector<State> opts_bellman_list(BellmanListRecData& data)
{
    if (data.c == 0)
        return {{0, 0}};

    Profit lb = 0;
    std::vector<State> l0{{0, 0}};
    for (ItemPos j=data.n1; j<=data.n2; ++j) {
        Weight wj = data.ins.item(j).w;
        Profit pj = data.ins.item(j).p;
        std::vector<State> l{{0, 0}};
        std::vector<State>::iterator it = l0.begin();
        std::vector<State>::iterator it1 = l0.begin();
        while (it != l0.end() || it1 != l0.end()) {
            if (it == l0.end() || it->w > it1->w + wj) {
                State s1{it1->w+wj, it1->p+pj};
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
    return l0;
}

void sopt_bellman_list_rec_rec(BellmanListRecData& data)
{
    ItemPos n1 = data.n1;
    ItemPos n2 = data.n2;
    ItemPos k = (data.n1 + data.n2) / 2;

    DBG(data.info.debug(STR1(n1) + STR2(n2) + STR2(k) + STR4(c, data.c) + "\n");)

    Weight w1_opt = 0;
    Weight w2_opt = 0;
    Profit p1_opt = 0;
    Profit p2_opt = 0;

    {
        data.n1 = n1;
        data.n2 = k;
        std::vector<State> l1 = opts_bellman_list(data);
        data.n1 = k+1;
        data.n2 = n2;
        std::vector<State> l2 = opts_bellman_list(data);

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

        DBG(data.info.debug(STR1(z_max) + "\n" +
                STR1(w1_opt) + STR2(p1_opt) + "\n" +
                STR1(w1_opt) + STR2(p2_opt) + "\n");)
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
}

Solution knapsack::sopt_bellman_list_rec(const Instance& ins, Info& info)
{
    info.verbose("*** bellman (list, rec) ***\n");

    ItemPos n = ins.item_number();
    Solution sol(ins);

    if (n == 0)
        return algorithm_end(sol, info);
    if (n == 1) {
        sol.set(0, true);
        return algorithm_end(sol, info);
    }

    BellmanListRecData data(ins, info);
    sopt_bellman_list_rec_rec(data);
    return algorithm_end(data.sol_curr, info);
}

