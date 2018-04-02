#include "minknap.hpp"

#include "../lib/part_solution_2.hpp"
#include "../lb_greedy/greedy.hpp"
#include "../lb_greedynlogn/greedynlogn.hpp"
#include "../ub_dembo/dembo.hpp"
#include "../ub_dantzig/dantzig.hpp"
#include "../ub_surrogate/surrogate.hpp"

#include <bitset>

#define DBG(x)
//#define DBG(x) x

struct StatePart
{
    Weight w;
    Profit p;
    PartSol2 sol;
};

std::ostream& operator<<(std::ostream& os, const StatePart& s)
{
    os << "(" << s.w << " " << s.p << ")";
    return os;
}

void add_item(const Instance& ins, std::vector<StatePart>& l0,
        ItemPos s, ItemPos t, Profit& lb, StatePart& best_state, Profit ub,
        PartSolFactory2& psolf)
{
    DBG(std::cout << "ADD ITEM... S " << s << " T " << t << " LB " << lb << std::endl;)
    psolf.add_item(t);
    best_state.sol = psolf.remove(best_state.sol);
    Weight c = ins.total_capacity();
    Weight wt = ins.item(t).w;
    Profit pt = ins.item(t).p;
    std::vector<StatePart> l;
    std::vector<StatePart>::iterator it = l0.begin();
    std::vector<StatePart>::iterator it1 = l0.begin();
    while (it != l0.end() || it1 != l0.end()) {
        if (it == l0.end() || it->w > it1->w + wt) {
            StatePart s1{it1->w+wt, it1->p+pt, psolf.add(it1->sol)};
            DBG(std::cout << "STATE " << *it1 << " => " << s1 << std::flush;)
            if (l.empty() || s1.p > l.back().p) {
                if (s1.w <= c && s1.p > lb) { // Update lower bound
                    lb = s1.p;
                    best_state = s1;
                    if (lb == ub)
                        return;
                }
                if (!l.empty() && s1.w == l.back().w) {
                    l.back() = s1;
                    DBG(std::cout << " OK" << std::endl;)
                } else {
                    Profit ub = (s1.w <= c)?
                        ub_dembo(ins, t+1, s1.p, c-s1.w):
                        ub_dembo_rev(ins, s, s1.p, c-s1.w);
                    DBG(std::cout << " UB " << ub << " LB " << lb << std::flush;)
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
            it->sol = psolf.remove(it->sol);
            if (l.empty() || it->p > l.back().p) {
                if (!l.empty() && it->w == l.back().w) {
                    l.back() = *it;
                    DBG(std::cout << " OK" << std::endl;)
                } else {
                    Profit ub = (it->w <= c)?
                        ub_dembo(ins, t+1, it->p, c-it->w):
                        ub_dembo_rev(ins, s, it->p, c-it->w);
                    DBG(std::cout << " UB " << ub << " LB " << lb << std::flush;)
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

void remove_item(const Instance& ins, std::vector<StatePart>& l0,
        ItemPos s, ItemPos t, Profit& lb, StatePart& best_state, Profit ub,
        PartSolFactory2& psolf)
{
    DBG(std::cout << "REMOVE ITEM... S " << s << " T " << t << " LB " << lb << std::endl;)
    psolf.add_item(s);
    best_state.sol = psolf.add(best_state.sol);
    Weight c = ins.total_capacity();
    Weight ws = ins.item(s).w;
    Profit ps = ins.item(s).p;
    std::vector<StatePart> l;
    std::vector<StatePart>::iterator it = l0.begin();
    std::vector<StatePart>::iterator it1 = l0.begin();
    while (it != l0.end() || it1 != l0.end()) {
        if (it1 == l0.end() || it->w <= it1->w - ws) {
            DBG(std::cout << "STATE " << *it;)
            it->sol = psolf.add(it->sol);
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
            StatePart s1{it1->w-ws, it1->p-ps, psolf.remove(it1->sol)};
            DBG(std::cout << "STATE " << *it1 << " => " << s1;)
            if (l.empty() || s1.p > l.back().p) {
                if (s1.w <= c && s1.p > lb) { // Update lower bound
                    lb = s1.p;
                    best_state = s1;
                    if (lb == ub)
                        return;
                }
                if (!l.empty() && s1.w == l.back().w) {
                    l.back() = s1;
                    DBG(std::cout << " OK" << std::endl;)
                } else {
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

#undef DBG

#define DBG(x)
//#define DBG(x) x

Solution sopt_minknap_list_part(Instance& ins,
        MinknapParams params, ItemPos k, Info* info, Profit o)
{
    DBG(std::cout << "MINKNAPART... F " << ins.first_item() << " L " << ins.last_item() << std::endl;)
    if (Info::verbose(info))
        std::cout << "N " << ins.item_number() << "/" << ins.total_item_number()
            << " F " << ins.first_item()
            << " L " << ins.last_item() << std::endl;
    DBG(std::cout << ins << std::endl;)
    (void)info;

    DBG(std::cout << "SORTING..." << std::endl;)
    ins.sort_partially();
    if (ins.break_item() == ins.last_item()+1) // all items are in the break solution
        return *ins.break_solution();

    DBG(std::cout << "LB..." << std::flush;)
    Solution sol(ins);
    if (params.lb_greedynlogn == 0) {
        params.lb_greedynlogn = -1;
        sol = sol_bestgreedynlogn(ins);
    } else if (params.lb_greedy == 0) {
        params.lb_greedy = -1;
        sol = sol_greedy(ins);
    } else {
        sol = *ins.break_solution();
    }
    DBG(std::cout << " " << ins.print_lb(sol.profit()) << std::endl;)

    Weight  c = ins.total_capacity();
    ItemPos n = ins.item_number();
    Profit lb = (o != -1 && o > sol.profit())? o-1: sol.profit();

    // Trivial cases
    if (n == 0 || c == 0) {
        return (ins.reduced_solution()->profit() > sol.profit())?
            *ins.reduced_solution(): sol;
    } else if (n == 1) {
        Solution sol1 = *ins.reduced_solution();
        sol1.set(ins.first_item(), true);
        return (sol1.profit() > sol.profit())? sol1: sol;
    } else if (ins.break_item() == ins.last_item()+1) {
        return (ins.break_solution()->profit() > sol.profit())?
            *ins.break_solution(): sol;
    }

    Weight w_bar = ins.break_solution()->weight();
    Profit p_bar = ins.break_solution()->profit();
    Profit u = (o != -1)? o: ub_dantzig(ins);
    if (sol.profit() == u) // If UB == LB, then stop
        return sol;

    // Create memory table
    DBG(std::cout << "K " << k << " F " << ins.first_item() << " B " << ins.break_item() << " L " << ins.last_item() << std::endl;)
    PartSolFactory2 psolf(k);
    std::vector<StatePart> l0 = {{w_bar, p_bar, 0}};

    DBG(std::cout << "RECURSION..." << std::endl;)
    ItemPos s = ins.break_item() - 1;
    ItemPos t = ins.break_item();
    StatePart best_state = l0.front();
    while (!l0.empty()) {
        DBG(std::cout << "STATES " << l0.size() << std::endl;)
        DBG(std::cout << "F " << ins.first_item() << " S' " << ins.first_sorted_item() << " S " << s << " T " << t << " T' " << ins.last_sorted_item() << " L " << ins.last_item() << std::endl;)
        if (ins.int_right_size() > 0 && t+1 > ins.last_sorted_item())
            ins.sort_right(lb);
        if (t <= ins.last_sorted_item()) {
            add_item(ins, l0, s, t, lb, best_state, u, psolf);
            ++t;
        }
        if (lb == u)
            break;

        DBG(std::cout << "STATES " << l0.size() << std::endl;)
        DBG(std::cout << "F " << ins.first_item() << " S' " << ins.first_sorted_item() << " S " << s << " T " << t << " T' " << ins.last_sorted_item() << " L " << ins.last_item() << std::endl;)
        if (ins.int_left_size() > 0 && s-1 < ins.first_sorted_item())
            ins.sort_left(lb);
        if (s >= ins.first_sorted_item()) {
            remove_item(ins, l0, s, t, lb, best_state, u, psolf);
            --s;
        }
        if (lb == u)
            break;
    }

    if (best_state.p <= sol.profit())
        return sol;
    assert(ins.check_opt(lb));

    ins.set_first_item(s+1);
    ins.set_last_item(t-1);
    DBG(std::cout << "PSOL " << psolf.print(best_state.sol) << std::endl;)
    ins.fix(psolf, best_state.sol);
    //std::cout << "F " << ins.first_item() << " L " << ins.last_item() << std::endl;
    DBG(std::cout << "MINKNAPPART... END" << std::endl;)
    return sopt_minknap_list_part(ins, params, k, info, best_state.p);
}

#undef DBG

