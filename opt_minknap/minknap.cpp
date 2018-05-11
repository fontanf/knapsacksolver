#include "opt_minknap/minknap.hpp"

#include "lb_greedy/greedy.hpp"
#include "lb_greedynlogn/greedynlogn.hpp"
#include "ub_dembo/dembo.hpp"
#include "ub_dantzig/dantzig.hpp"
#include "ub_surrogate/surrogate.hpp"

#include <bitset>

using namespace knapsack;

#define DBG(x)
//#define DBG(x) x

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
    DBG(std::cout << "ADD ITEM... S " << s << " T " << t << " LB " << lb << std::endl;)
    Weight c = ins.total_capacity();
    Weight wt = ins.item(t).w;
    Profit pt = ins.item(t).p;
    std::vector<State> l;
    std::vector<State>::iterator it = l0.begin();
    std::vector<State>::iterator it1 = l0.begin();
    while (it != l0.end() || it1 != l0.end()) {
        if (it == l0.end() || it->w > it1->w + wt) {
            State s1{it1->w+wt, it1->p+pt};
            DBG(std::cout << "STATE " << *it1 << " => " << s1 << std::flush;)
            if (l.empty() || s1.p > l.back().p) {
                if (!l.empty() && s1.w == l.back().w) {
                    l.back() = s1;
                    DBG(std::cout << " OK" << std::endl;)
                } else {
                    if (s1.w <= c && s1.p > lb) // Update lower bound
                        lb = s1.p;
                    Profit ub = (s1.w <= c)?
                        ub_dembo(ins, t+1, s1.p, c-s1.w):
                        ub_dembo_rev(ins, s, s1.p, c-s1.w);
                    DBG(std::cout << " UB " << ub << std::flush;)
                    if (ub > lb) {
                        l.push_back(s1);
                        DBG(std::cout << " OK" << std::endl;)
                    } else {
                        DBG(std::cout << " X" << std::endl;)
                    }
                }
            } else {
                DBG(std::cout << " X" << std::endl;)
            }
            it1++;
        } else {
            assert(it != l0.end());
            DBG(std::cout << "STATE " << *it << std::flush;)
            if (l.empty() || it->p > l.back().p) {
                if (!l.empty() && it->w == l.back().w) {
                    l.back() = *it;
                    DBG(std::cout << " OK" << std::endl;)
                } else {
                    Profit ub = (it->w <= c)?
                        ub_dembo(ins, t+1, it->p, c-it->w):
                        ub_dembo_rev(ins, s, it->p, c-it->w);
                    DBG(std::cout << " UB " << ub << std::flush;)
                    if (ub > lb) {
                        l.push_back(*it);
                        DBG(std::cout << " OK" << std::endl;)
                    } else {
                        DBG(std::cout << " X" << std::endl;)
                    }
                }
            } else {
                DBG(std::cout << " X" << std::endl;)
            }
            ++it;
        }
    }
    l0 = std::move(l);
    DBG(std::cout << "ADD ITEM... END" << std::endl;)
}

void remove_item(const Instance& ins, std::vector<State>& l0,
        ItemPos s, ItemPos t, Profit& lb)
{
    DBG(std::cout << "REMOVE ITEM... S " << s << " T " << t << " LB " << lb << std::endl;)
    Weight c = ins.total_capacity();
    Weight ws = ins.item(s).w;
    Profit ps = ins.item(s).p;
    std::vector<State> l;
    std::vector<State>::iterator it = l0.begin();
    std::vector<State>::iterator it1 = l0.begin();
    while (it != l0.end() || it1 != l0.end()) {
        if (it1 == l0.end() || it->w <= it1->w - ws) {
            DBG(std::cout << "STATE " << *it;)
            if (l.empty() || it->p > l.back().p) {
                if (!l.empty() && it->w == l.back().w) {
                    l.back() = *it;
                    DBG(std::cout << " OK" << std::endl;)
                } else {
                    Profit ub = (it->w <= c)?
                        ub_dembo(ins, t, it->p, c-it->w):
                        ub_dembo_rev(ins, s-1, it->p, c-it->w);
                    DBG(std::cout << " UB " << ub << " LB " << lb;)
                    if (ub > lb) {
                        l.push_back(*it);
                        DBG(std::cout << " OK" << std::endl;)
                    } else {
                        DBG(std::cout << " X" << std::endl;)
                    }
                }
            } else {
                DBG(std::cout << " X" << std::endl;)
            }
            ++it;
        } else {
            State s1{it1->w-ws, it1->p-ps};
            DBG(std::cout << "STATE " << *it1 << " => " << s1;)
            if (l.empty() || s1.p > l.back().p) {
                if (!l.empty() && s1.w == l.back().w) {
                    l.back() = s1;
                    DBG(std::cout << " OK" << std::endl;)
                } else {
                    if (s1.w <= c && s1.p > lb) // Update lower bound
                        lb = s1.p;
                    Profit ub = (s1.w <= c)?
                        ub_dembo(ins, t, s1.p, c-s1.w):
                        ub_dembo_rev(ins, s-1, s1.p, c-s1.w);
                    DBG(std::cout << " UB " << ub << " LB " << lb;)
                    if (ub > lb) {
                        l.push_back(s1);
                        DBG(std::cout << " OK" << std::endl;)
                    } else {
                        DBG(std::cout << " X" << std::endl;)
                    }
                }
            } else {
                DBG(std::cout << " X" << std::endl;)
            }
            it1++;
        }
    }
    l0 = std::move(l);
    DBG(std::cout << "REMOVE ITEM... END" << std::endl;)
}

Profit knapsack::opt_minknap_list(Instance& ins, MinknapParams params, Info* info)
{
    DBG(std::cout << "MINKNAPOPT..." << std::endl;)
    DBG(std::cout << ins << std::endl;)
    (void)info;

    DBG(std::cout << "SORTING..." << std::endl;)
    ins.sort_partially();
    if (ins.break_item() == ins.last_item()+1) // all items are in the break solution
        return ins.break_profit();

    DBG(std::cout << "LB..." << std::flush;)
    Profit lb = 0;
    if (params.lb_greedynlogn == 0) {
        lb = sol_bestgreedynlogn(ins).profit();
    } else if (params.lb_greedy == 0) {
        lb = sol_greedy(ins).profit();
    } else {
        lb = ins.break_profit();
    }
    DBG(std::cout << " " << ins.print_lb(lb) << std::endl;)

    Weight  c = ins.total_capacity();
    ItemPos n = ins.item_number();
    Profit p0 = ins.reduced_solution()->profit();

    // Trivial cases
    if (n == 0 || c == 0) {
        return std::max(lb, p0);
    } else if (n == 1) {
        return std::max(lb, p0 + ins.item(ins.first_item()).p);
    } else if (ins.break_item() == ins.last_item()+1) {
        return std::max(lb, ins.break_solution()->profit());
    }

    Weight w_bar = ins.break_solution()->weight();
    Profit p_bar = ins.break_solution()->profit();
    Profit u     = ub_dantzig(ins);
    if (lb == u) // If UB == LB, then stop
        return lb;

    // Create memory table
    std::vector<State> l0 = {{w_bar, p_bar}};

    DBG(std::cout << "RECURSION..." << std::endl;)
    ItemPos s = ins.break_item() - 1;
    ItemPos t = ins.break_item();
    while (!l0.empty()) {
        DBG(std::cout << "STATES " << l0.size() << std::endl;)
        DBG(std::cout << "F " << ins.first_item() << " S' " << ins.first_sorted_item() << " S " << s << " T " << t << " T' " << ins.last_sorted_item() << " L " << ins.last_item() << std::endl;)
        if (ins.int_right_size() > 0 && t+1 > ins.last_sorted_item())
            ins.sort_right(lb);
        if (t <= ins.last_sorted_item()) {
            add_item(ins, l0, s, t, lb);
            ++t;
        }

        DBG(std::cout << "STATES " << l0.size() << std::endl;)
        DBG(std::cout << "F " << ins.first_item() << " S' " << ins.first_sorted_item() << " S " << s << " T " << t << " T' " << ins.last_sorted_item() << " L " << ins.last_item() << std::endl;)
        if (ins.int_left_size() > 0 && s-1 < ins.first_sorted_item())
            ins.sort_left(lb);
        if (s >= ins.first_sorted_item()) {
            remove_item(ins, l0, s, t, lb);
            --s;
        }
    }

    assert(ins.check_opt(lb));
    DBG(std::cout << "MINKNAPOPT... END" << std::endl;)
    return lb;
}

#undef DBG

