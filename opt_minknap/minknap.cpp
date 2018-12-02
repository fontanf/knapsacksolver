#include "knapsack/opt_minknap/minknap.hpp"

#include "knapsack/lb_greedy/greedy.hpp"
#include "knapsack/lb_greedynlogn/greedynlogn.hpp"
#include "knapsack/ub_dembo/dembo.hpp"
#include "knapsack/ub_dantzig/dantzig.hpp"
#include "knapsack/ub_surrogate/surrogate.hpp"

#include <bitset>

using namespace knapsack;

struct State
{
    Weight w;
    Profit p;
};

std::ostream& operator<<(std::ostream& os, const State& s)
{
    os << "(" << s.w << " " << s.p << ")";
    return os;
}

void add_item(const Instance& ins, std::vector<State>& l0,
        ItemPos s, ItemPos t, Profit& lb)
{
    Weight c = ins.total_capacity();
    Weight wt = ins.item(t).w;
    Profit pt = ins.item(t).p;
    ItemPos tx = (ins.int_right_size() > 0 && t == ins.last_sorted_item())?
            ins.last_item()+1: t+1;
    ItemPos sx = (ins.int_left_size() > 0 && s == ins.first_sorted_item())?
            ins.first_item()-1: s;
    std::vector<State> l;
    std::vector<State>::iterator it = l0.begin();
    std::vector<State>::iterator it1 = l0.begin();
    while (it != l0.end() || it1 != l0.end()) {
        if (it == l0.end() || it->w > it1->w + wt) {
            State s1{it1->w+wt, it1->p+pt};
            if (l.empty() || s1.p > l.back().p) {
                if (s1.w <= c && s1.p > lb) { // Update lower bound
                    lb = s1.p;
                }
                if (!l.empty() && s1.w == l.back().w) {
                    l.back() = s1;
                } else {
                    Profit ub = (s1.w <= c)?
                        ub_dembo(ins, tx, s1.p, c-s1.w):
                        ub_dembo_rev(ins, sx, s1.p, c-s1.w);
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
            if (l.empty() || it->p > l.back().p) {
                if (!l.empty() && it->w == l.back().w) {
                    l.back() = *it;
                } else {
                    Profit ub = (it->w <= c)?
                        ub_dembo(ins, tx, it->p, c-it->w):
                        ub_dembo_rev(ins, sx, it->p, c-it->w);
                    if (ub > lb) {
                        l.push_back(*it);
                    } else {
                    }
                }
            } else {
            }
            ++it;
        }
    }
    l0 = std::move(l);
}

void remove_item(const Instance& ins, std::vector<State>& l0,
        ItemPos s, ItemPos t, Profit& lb)
{
    Weight c = ins.total_capacity();
    Weight ws = ins.item(s).w;
    Profit ps = ins.item(s).p;
    ItemPos tx = (ins.int_right_size() > 0 && t == ins.last_sorted_item())?
            ins.last_item()+1: t;
    ItemPos sx = (ins.int_left_size() > 0 && s == ins.first_sorted_item())?
            ins.first_item()-1: s-1;
    std::vector<State> l;
    std::vector<State>::iterator it = l0.begin();
    std::vector<State>::iterator it1 = l0.begin();
    while (it != l0.end() || it1 != l0.end()) {
        if (it1 == l0.end() || it->w <= it1->w - ws) {
            if (l.empty() || it->p > l.back().p) {
                if (!l.empty() && it->w == l.back().w) {
                    l.back() = *it;
                } else {
                    Profit ub = (it->w <= c)?
                        ub_dembo(ins, tx, it->p, c-it->w):
                        ub_dembo_rev(ins, sx, it->p, c-it->w);
                    if (ub > lb) {
                        l.push_back(*it);
                    } else {
                    }
                }
            } else {
            }
            ++it;
        } else {
            State s1{it1->w-ws, it1->p-ps};
            if (l.empty() || s1.p > l.back().p) {
                if (s1.w <= c && s1.p > lb) { // Update lower bound
                    lb = s1.p;
                }
                if (!l.empty() && s1.w == l.back().w) {
                    l.back() = s1;
                } else {
                    Profit ub = (s1.w <= c)?
                        ub_dembo(ins, tx, s1.p, c-s1.w):
                        ub_dembo_rev(ins, sx, s1.p, c-s1.w);
                    if (ub > lb) {
                        l.push_back(s1);
                    } else {
                    }
                }
            } else {
            }
            it1++;
        }
    }
    l0 = std::move(l);
}

Profit knapsack::opt_minknap_list(Instance& ins, Info& info, MinknapParams params)
{
    (void)info;

    ins.sort_partially();
    if (ins.break_item() == ins.last_item()+1) // all items are in the break solution
        return algorithm_end(ins.break_profit(), info);

    Profit lb = 0;
    if (params.lb_greedynlogn == 0) {
        Info info_tmp;
        lb = sol_bestgreedynlogn(ins, info_tmp).profit();
    } else if (params.lb_greedy == 0) {
        Info info_tmp;
        lb = sol_greedy(ins, info_tmp).profit();
    } else {
        lb = ins.break_profit();
    }

    Weight  c = ins.total_capacity();
    ItemPos n = ins.item_number();
    Profit p0 = ins.reduced_solution()->profit();

    // Trivial cases
    if (n == 0 || c == 0) {
        return algorithm_end(std::max(lb, p0), info);
    } else if (n == 1) {
        return algorithm_end(std::max(lb, p0 + ins.item(ins.first_item()).p), info);
    } else if (ins.break_item() == ins.last_item()+1) {
        return algorithm_end(std::max(lb, ins.break_solution()->profit()), info);
    }

    Weight w_bar = ins.break_solution()->weight();
    Profit p_bar = ins.break_solution()->profit();
    Info info_tmp;
    Profit u = ub_dantzig(ins, info_tmp);
    if (lb == u) // If UB == LB, then stop
        return algorithm_end(lb, info);

    // Create memory table
    std::vector<State> l0 = {{w_bar, p_bar}};

    ItemPos s = ins.break_item() - 1;
    ItemPos t = ins.break_item();
    while (!l0.empty()) {
        if (ins.int_right_size() > 0 && t+1 > ins.last_sorted_item())
            ins.sort_right(lb);
        if (t <= ins.last_sorted_item()) {
            add_item(ins, l0, s, t, lb);
            ++t;
        }

        if (ins.int_left_size() > 0 && s-1 < ins.first_sorted_item())
            ins.sort_left(lb);
        if (s >= ins.first_sorted_item()) {
            remove_item(ins, l0, s, t, lb);
            --s;
        }
    }

    return algorithm_end(lb, info);
}

