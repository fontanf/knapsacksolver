#include "knapsack/opt_minknap/minknap.hpp"

#include "knapsack/lib/part_solution_2.hpp"
#include "knapsack/lb_greedy/greedy.hpp"
#include "knapsack/lb_greedynlogn/greedynlogn.hpp"
#include "knapsack/ub_dembo/dembo.hpp"
#include "knapsack/ub_dantzig/dantzig.hpp"
#include "knapsack/ub_surrogate/surrogate.hpp"

using namespace knapsack;

struct StatePart
{
    Weight w;
    Profit p;
    PartSol2 sol;

    std::string to_string()
    {
        return "(" + std::to_string(w) + " " + std::to_string(p) + ")";
    }
};

struct MinknapData
{
    MinknapData(Instance& ins, Info& info, ItemIdx k):
        ins(ins), info(info), psolf(ins, k) { }
    Instance& ins;
    Info& info;
    Profit lb;
    Profit ub;
    ItemPos s;
    ItemPos t;
    std::vector<StatePart> l0;
    PartSolFactory2 psolf;
    StatePart best_state;
    StateIdx distinct_state_number;
    Cpt lb_number;
    Cpt ub_number;
};

void add_item(MinknapData& d)
{
    DBG(d.info.debug("Add item..." + STR2(d.s) + STR2(d.t) + STR2(d.lb) + "\n");)
    d.psolf.add_item(d.t);
    d.best_state.sol = d.psolf.remove(d.best_state.sol);
    Weight c = d.ins.total_capacity();
    Weight wt = d.ins.item(d.t).w;
    Profit pt = d.ins.item(d.t).p;
    ItemPos tx = (d.ins.int_right_size() > 0 && d.t == d.ins.last_sorted_item())?
            d.ins.last_item()+1: d.t+1;
    ItemPos sx = (d.ins.int_left_size() > 0 && d.s == d.ins.first_sorted_item())?
            d.ins.first_item()-1: d.s;
    DBG(d.info.debug("sx " + std::to_string(sx) + " tx " + std::to_string(tx) + "\n");)
    std::vector<StatePart> l;
    std::vector<StatePart>::iterator it = d.l0.begin();
    std::vector<StatePart>::iterator it1 = d.l0.begin();
    while (it != d.l0.end() || it1 != d.l0.end()) {
        if (it == d.l0.end() || it->w > it1->w + wt) {
            StatePart s1{it1->w+wt, it1->p+pt, d.psolf.add(it1->sol)};
            DBG(d.info.debug("State " + it1->to_string() + " => " + s1.to_string());)
            if (l.empty() || s1.p > l.back().p) {
                if (s1.w <= c && s1.p > d.lb) { // Update lower bound
                    Solution::update_lb(d.lb, s1.p, d.info, d.lb_number, d.ub);
                    d.best_state = s1;
                    assert(d.lb <= d.ub);
                    if (d.lb == d.ub)
                        return;
                }
                if (!l.empty() && s1.w == l.back().w) {
                    l.back() = s1;
                    d.distinct_state_number++;
                    DBG(d.info.debug(" ok\n");)
                } else {
                    Profit ub = (s1.w <= c)?
                        ub_dembo(d.ins, tx, s1.p, c-s1.w):
                        ub_dembo_rev(d.ins, sx, s1.p, c-s1.w);
                    DBG(d.info.debug(STR2(ub) + STR2(d.lb));)
                    if (ub > d.lb) {
                        l.push_back(s1);
                        d.distinct_state_number++;
                        DBG(d.info.debug(" ok\n");)
                    } else {
                        DBG(d.info.debug(" ×\n");)
                    }
                }
            } else {
                DBG(d.info.debug(" ×\n");)
            }
            it1++;
        } else {
            assert(it != d.l0.end());
            DBG(d.info.debug("State " + it->to_string());)
            it->sol = d.psolf.remove(it->sol);
            if (l.empty() || it->p > l.back().p) {
                if (!l.empty() && it->w == l.back().w) {
                    l.back() = *it;
                    DBG(d.info.debug(" ok\n");)
                } else {
                    Profit ub = (it->w <= c)?
                        ub_dembo(d.ins, tx, it->p, c-it->w):
                        ub_dembo_rev(d.ins, sx, it->p, c-it->w);
                    DBG(d.info.debug(STR2(ub) + STR2(d.lb));)
                    if (ub > d.lb) {
                        l.push_back(*it);
                        DBG(d.info.debug(" ok\n");)
                    } else {
                        DBG(d.info.debug(" ×\n");)
                    }
                }
            } else {
                DBG(d.info.debug(" ×\n");)
            }
            ++it;
        }
    }
    d.l0 = std::move(l);
    DBG(d.info.debug("Add item... end\n");)
}

void remove_item(MinknapData& d)
{
    DBG(d.info.debug("Remove item... " + STR2(d.s) + STR2(d.t) + STR2(d.lb) + "\n");)
    d.psolf.add_item(d.s);
    d.best_state.sol = d.psolf.add(d.best_state.sol);
    Weight c = d.ins.total_capacity();
    Weight ws = d.ins.item(d.s).w;
    Profit ps = d.ins.item(d.s).p;
    ItemPos tx = (d.ins.int_right_size() > 0 && d.t == d.ins.last_sorted_item())?
            d.ins.last_item()+1: d.t;
    ItemPos sx = (d.ins.int_left_size() > 0 && d.s == d.ins.first_sorted_item())?
            d.ins.first_item()-1: d.s-1;
    DBG(d.info.debug("sx " + std::to_string(sx) + " tx " + std::to_string(tx) + "\n");)
    std::vector<StatePart> l;
    std::vector<StatePart>::iterator it = d.l0.begin();
    std::vector<StatePart>::iterator it1 = d.l0.begin();
    while (it != d.l0.end() || it1 != d.l0.end()) {
        if (it1 == d.l0.end() || it->w <= it1->w - ws) {
            DBG(d.info.debug("State " + it->to_string());)
            it->sol = d.psolf.add(it->sol);
            if (l.empty() || it->p > l.back().p) {
                if (!l.empty() && it->w == l.back().w) {
                    l.back() = *it;
                    DBG(d.info.debug(" ok\n");)
                } else {
                    Profit ub = (it->w <= c)?
                        ub_dembo(d.ins, tx, it->p, c-it->w):
                        ub_dembo_rev(d.ins, sx, it->p, c-it->w);
                    DBG(d.info.debug(STR2(ub) + STR2(d.lb));)
                    if (ub > d.lb) {
                        l.push_back(*it);
                        DBG(d.info.debug(" ok\n");)
                    } else {
                        DBG(d.info.debug(" ×\n");)
                    }
                }
            } else {
                DBG(d.info.debug(" ×\n");)
            }
            ++it;
        } else {
            StatePart s1{it1->w-ws, it1->p-ps, d.psolf.remove(it1->sol)};
            DBG(d.info.debug("State " + it1->to_string() + " => " + s1.to_string());)
            if (l.empty() || s1.p > l.back().p) {
                if (s1.w <= c && s1.p > d.lb) { // Update lower bound
                    Solution::update_lb(d.lb, s1.p, d.info, d.lb_number, d.ub);
                    d.best_state = s1;
                    assert(d.lb <= d.ub);
                    if (d.lb == d.ub)
                        return;
                }
                if (!l.empty() && s1.w == l.back().w) {
                    l.back() = s1;
                    d.distinct_state_number++;
                    DBG(d.info.debug(" ok\n");)
                } else {
                    Profit ub = (s1.w <= c)?
                        ub_dembo(d.ins, tx, s1.p, c-s1.w):
                        ub_dembo_rev(d.ins, sx, s1.p, c-s1.w);
                    DBG(d.info.debug(STR2(ub) + STR2(d.lb));)
                    if (ub > d.lb) {
                        l.push_back(s1);
                        d.distinct_state_number++;
                        DBG(d.info.debug(" ok\n");)
                    } else {
                        DBG(d.info.debug(" ×\n");)
                    }
                }
            } else {
                DBG(d.info.debug(" ×\n");)
            }
            it1++;
        }
    }
    d.l0 = std::move(l);
    DBG(d.info.debug("Remove item... end\n");)
}

Solution knapsack::sopt_minknap_list_part(Instance& ins, Info& info,
        MinknapParams params, ItemPos k, Profit o)
{
    if (o == -1)
        info.verbose("*** minknap (list, part " + std::to_string(k) + ") ***\n");

    MinknapData data(ins, info, k);
    data.lb_number = 0;
    data.ub_number = 0;
    Solution sol(ins);
    data.lb = 0;
    data.ub = -1;

    if (o != -1) {
        data.lb = o-1;
        data.ub = o;
    }

    ins.sort_partially();
    if (ins.break_item() == ins.last_item()+1) { // all items are in the break solution
        if (ins.break_solution()->profit() > sol.profit())
            sol.update(*ins.break_solution(), info, data.lb_number, data.ub);
        return algorithm_end(sol, info);
    }

    info.verbose("Compute lower bound...\n");
    if (params.lb_greedynlogn == 0) {
        params.lb_greedynlogn = -1;
        Info info_tmp;
        Solution sol_tmp = sol_bestgreedynlogn(ins, info_tmp);
        if (sol_tmp.profit() > data.lb) {
            sol.update(sol_tmp, info, data.lb_number, data.ub);
            data.lb = sol.profit();
        }
    } else if (params.lb_greedy >= 0) {
        Info info_tmp;
        Solution sol_tmp = sol_greedy(ins, info_tmp);
        if (sol_tmp.profit() > data.lb) {
            sol.update(sol_tmp, info, data.lb_number, data.ub);
            data.lb = sol.profit();
        }
    } else {
        Solution sol_tmp = *ins.break_solution();
        if (sol_tmp.profit() > data.lb) {
            sol.update(sol_tmp, info, data.lb_number, data.ub);
            data.lb = sol.profit();
        }
    }
    if (data.lb == data.ub)
        return algorithm_end(sol, info);

    Weight  c = ins.total_capacity();
    ItemPos n = ins.item_number();

    // Trivial cases
    if (n == 0 || c == 0) {
        if (ins.reduced_solution()->profit() > sol.profit())
            sol.update(*ins.reduced_solution(), info, data.lb_number, data.ub);
        return algorithm_end(sol, info);
    } else if (n == 1) {
        Solution sol_tmp = *ins.reduced_solution();
        sol_tmp.set(ins.first_item(), true);
        if (sol_tmp.profit() > sol.profit())
            sol.update(sol_tmp, info, data.lb_number, data.ub);
        return algorithm_end(sol, info);
    } else if (ins.break_item() == ins.last_item()+1) {
        if (ins.break_solution()->profit() > sol.profit())
            sol.update(*ins.break_solution(), info, data.lb_number, data.ub);
        return algorithm_end(sol, info);
    }

    Weight w_bar = ins.break_solution()->weight();
    Profit p_bar = ins.break_solution()->profit();

    Info info_tmp;
    Profit ub_tmp = ub_dantzig(ins, info_tmp);
    if (data.ub == -1 || ub_tmp < data.ub)
        Solution::update_ub(data.ub, ub_tmp, info, data.ub_number, data.lb);

    if (data.lb == data.ub) // If UB == LB, then stop
        return algorithm_end(sol, info);

    // Create memory table
    info.verbose("Recursion...\n");
    data.l0.push_back({w_bar, p_bar, 0});

    data.s = ins.break_item() - 1;
    data.t = ins.break_item();
    data.best_state = data.l0.front();
    data.distinct_state_number = 1;
    StateIdx state_number = 1;
    while (!data.l0.empty()) {
        DBG(info.debug(
                STR3(f, ins.first_item())
                + STR4(f_, ins.first_sorted_item())
                + STR4(s, data.s)
                + STR4(t, data.t)
                + STR4(t_, ins.last_sorted_item())
                + STR4(l, ins.last_item()) + "\n");
        )

        if (ins.int_right_size() > 0 && data.t+1 > ins.last_sorted_item())
            ins.sort_right(data.lb);
        if (data.t <= ins.last_sorted_item() && data.t <= ins.last_item()) {
            add_item(data);
            state_number += data.l0.size();
            ++data.t;
        }
        if (data.lb == data.ub)
            break;

        DBG(info.debug(
                STR3(f, ins.first_item())
                + STR4(f_, ins.first_sorted_item())
                + STR4(s, data.s)
                + STR4(t, data.t)
                + STR4(t_, ins.last_sorted_item())
                + STR4(l, ins.last_item()) + "\n");
        )

        if (ins.int_left_size() > 0 && data.s-1 < ins.first_sorted_item())
            ins.sort_left(data.lb);
        if (data.s >= ins.first_sorted_item() && data.s >= ins.first_item()) {
            remove_item(data);
            state_number += data.l0.size();
            --data.s;
        }
        if (data.lb == data.ub)
            break;
    }

    info.verbose("Total state number: " + Info::to_string(state_number) + "\n");
    info.verbose("Distinct state number: " + Info::to_string(data.distinct_state_number) + "\n");
    info.pt.put("Algorithm.TotalStateNumber", state_number);
    info.pt.put("Algorithm.DistinctStateNumber", data.distinct_state_number);

    if (data.best_state.p <= sol.profit())
        return algorithm_end(sol, info);
    assert(data.best_state.p >= data.lb);

    ins.set_first_item(data.s+1);
    ins.set_last_item(data.t-1);
    bool b = ins.update_sorted();
    (void)b;
    assert(b);
    ins.fix(data.psolf.vector(data.best_state.sol));

    Info info_tmp2;
    sol = knapsack::sopt_minknap_list_part(ins, info, params, k, data.best_state.p);
    return algorithm_end(sol, info);
}

