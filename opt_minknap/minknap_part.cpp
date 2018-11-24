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
    MinknapData(const Instance& ins, Info& info): ins(ins), info(info) { }
    const Instance& ins;
    std::vector<StatePart> l0;
    ItemPos s;
    ItemPos t;
    Profit lb;
    StatePart best_state;
    Profit ub;
    PartSolFactory2 psolf;
    Info& info;
    StateIdx distinct_state_number;
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
                    d.lb = s1.p;
                    d.best_state = s1;
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
                    d.lb = s1.p;
                    d.best_state = s1;
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
    //DBG(info.debug("n " + std::to_string(ins.item_number()) + "/" + std::to_string(ins.total_item_number()) + " f " + std::to_string(ins.first_item()) + " l " + std::to_string(ins.last_item()) + " o " + std::to_string(o) + "\n");)

    //DBG(info.debug("Sort items...\n");)
    ins.sort_partially();
    if (ins.break_item() == ins.last_item()+1) { // all items are in the break solution
        //DBG(info.debug("All items fit in the knapsack.\n");)
        Solution sol = *ins.break_solution();
        return algorithm_end(sol, info);
    }

    info.verbose("Compute lower bound...");
    Solution sol(ins);
    if (params.lb_greedynlogn == 0) {
        params.lb_greedynlogn = -1;
        Info info_tmp;
        sol = sol_bestgreedynlogn(ins, info_tmp);
    } else if (params.lb_greedy == 0) {
        params.lb_greedy = -1;
        Info info_tmp;
        sol = sol_greedy(ins, info_tmp);
    } else {
        sol = *ins.break_solution();
    }
    info.verbose(" " + std::to_string(sol.profit()) + "\n");

    Weight  c = ins.total_capacity();
    ItemPos n = ins.item_number();
    Profit lb = (o != -1 && o > sol.profit())? o-1: sol.profit();

    // Trivial cases
    if (n == 0 || c == 0) {
        //DBG(info.debug("Empty instance.\n");)
        if (ins.reduced_solution()->profit() > sol.profit())
            sol = *ins.reduced_solution();
        return algorithm_end(sol, info);
    } else if (n == 1) {
        //DBG(info.debug("Instance only contains one item.\n");)
        Solution sol1 = *ins.reduced_solution();
        sol1.set(ins.first_item(), true);
        return algorithm_end((sol1.profit() > sol.profit())? sol1: sol, info);
    } else if (ins.break_item() == ins.last_item()+1) {
        //DBG(info.debug("All items fit in the knapsack.\n");)
        if (ins.break_solution()->profit() > sol.profit())
            sol = *ins.break_solution();
        return algorithm_end(sol, info);
    }

    Weight w_bar = ins.break_solution()->weight();
    Profit p_bar = ins.break_solution()->profit();
    Info info_tmp;
    Profit u = (o != -1)? o: ub_dantzig(ins, info_tmp);
    if (sol.profit() == u) { // If UB == LB, then stop
        //DBG(info.debug("Lower bound equals upper bound.");)
        return algorithm_end(sol, info);
    }

    // Create memory table
    MinknapData d(ins, info);
    d.lb = lb;
    d.ub = u;
    d.psolf = PartSolFactory2(k);
    d.l0.push_back({w_bar, p_bar, 0});

    info.verbose("Recursion...\n");
    d.s = ins.break_item() - 1;
    d.t = ins.break_item();
    d.best_state = d.l0.front();
    d.distinct_state_number = 1;
    StateIdx state_number = 1;
    while (!d.l0.empty()) {
        //DBG(info.debug("List of states size " + std::to_string(d.l0.size()) + "\n");)
        //DBG(info.debug("f " + std::to_string(ins.first_item())
                    //+ " s " + std::to_string(ins.first_sorted_item())
                    //+ " t " + std::to_string(ins.last_sorted_item())
                    //+ " l " + std::to_string(ins.last_item()) + "\n");)
        if (ins.int_right_size() > 0 && d.t+1 > ins.last_sorted_item())
            ins.sort_right(lb);
        if (d.t <= ins.last_sorted_item()) {
            add_item(d);
            state_number += d.l0.size();
            ++d.t;
        }
        if (d.lb == d.ub)
            break;

        //DBG(info.debug("List of states size " + std::to_string(d.l0.size()) + "\n");)
        //DBG(info.debug("f " + std::to_string(ins.first_item())
                    //+ " s " + std::to_string(ins.first_sorted_item())
                    //+ " t " + std::to_string(ins.last_sorted_item())
                    //+ " l " + std::to_string(ins.last_item()) + "\n");)
        if (ins.int_left_size() > 0 && d.s-1 < ins.first_sorted_item())
            ins.sort_left(lb);
        if (d.s >= ins.first_sorted_item()) {
            remove_item(d);
            state_number += d.l0.size();
            --d.s;
        }
        if (d.lb == d.ub)
            break;
    }

    info.verbose("Total state number: " + Info::to_string(state_number) + "\n");
    info.verbose("Distinct state number: " + Info::to_string(d.distinct_state_number) + "\n");
    info.pt.put("Algorithm.TotalStateNumber", state_number);
    info.pt.put("Algorithm.DistinctStateNumber", d.distinct_state_number);

    if (d.best_state.p <= sol.profit())
        return algorithm_end(sol, info);
    assert(ins.check_opt(lb));

    ins.set_first_item(d.s+1);
    ins.set_last_item(d.t-1);
    //DBG(info.debug("psol " + d.psolf.print(d.best_state.sol) + "\n");)
    ins.fix(d.psolf, d.best_state.sol);

    Info info_tmp2;
    sol = knapsack::sopt_minknap_list_part(ins, info_tmp2, params, k, d.best_state.p);
    return algorithm_end(sol, info);
}

