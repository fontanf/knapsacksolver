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
};

std::ostream& operator<<(std::ostream& os, const StatePart& s)
{
    os << "(" << s.w << " " << s.p << ")";
    return os;
}

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
    LOG_FOLD_START(d.info, "Add item... s " << d.s << " t " << d.t << " lb " << d.lb << std::endl);
    d.psolf.add_item(d.t);
    d.best_state.sol = d.psolf.remove(d.best_state.sol);
    Weight c = d.ins.total_capacity();
    Weight wt = d.ins.item(d.t).w;
    Profit pt = d.ins.item(d.t).p;
    ItemPos tx = (d.ins.int_right_size() > 0 && d.t == d.ins.last_sorted_item())?
            d.ins.last_item()+1: d.t+1;
    ItemPos sx = (d.ins.int_left_size() > 0 && d.s == d.ins.first_sorted_item())?
            d.ins.first_item()-1: d.s;
    LOG(d.info, "sx " << sx << " tx " << tx << std::endl);
    std::vector<StatePart> l;
    std::vector<StatePart>::iterator it = d.l0.begin();
    std::vector<StatePart>::iterator it1 = d.l0.begin();
    while (it != d.l0.end() || it1 != d.l0.end()) {
        if (it == d.l0.end() || it->w > it1->w + wt) {
            StatePart s1{it1->w+wt, it1->p+pt, d.psolf.add(it1->sol)};
            LOG(d.info, "State " << *it1 << " => " << s1);
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
                    LOG(d.info, " ok" << std::endl);
                } else {
                    Profit ub = (s1.w <= c)?
                        ub_dembo(d.ins, tx, s1.p, c-s1.w):
                        ub_dembo_rev(d.ins, sx, s1.p, c-s1.w);
                    LOG(d.info, " ub " << ub << " lb " << d.lb);
                    if (ub > d.lb) {
                        l.push_back(s1);
                        d.distinct_state_number++;
                        LOG(d.info, " ok" << std::endl);
                    } else {
                        LOG(d.info, " ×" << std::endl);
                    }
                }
            } else {
                LOG(d.info, " ×" << std::endl);
            }
            it1++;
        } else {
            assert(it != d.l0.end());
            LOG(d.info, "State " << *it);
            it->sol = d.psolf.remove(it->sol);
            if (l.empty() || it->p > l.back().p) {
                if (!l.empty() && it->w == l.back().w) {
                    l.back() = *it;
                    LOG(d.info, " ok" << std::endl);
                } else {
                    Profit ub = (it->w <= c)?
                        ub_dembo(d.ins, tx, it->p, c-it->w):
                        ub_dembo_rev(d.ins, sx, it->p, c-it->w);
                    LOG(d.info, " ub " << ub << " lb " << d.lb);
                    if (ub > d.lb) {
                        l.push_back(*it);
                        LOG(d.info, " ok" << std::endl);
                    } else {
                        LOG(d.info, " ×" << std::endl);
                    }
                }
            } else {
                LOG(d.info, " ×" << std::endl);
            }
            ++it;
        }
    }
    d.l0 = std::move(l);
    LOG_FOLD_END(d.info, "");
}

void remove_item(MinknapData& d)
{
    LOG_FOLD_START(d.info, "Remove item... s " << d.s << " t " << d.t << " lb " << d.lb << std::endl);
    d.psolf.add_item(d.s);
    d.best_state.sol = d.psolf.add(d.best_state.sol);
    Weight c = d.ins.total_capacity();
    Weight ws = d.ins.item(d.s).w;
    Profit ps = d.ins.item(d.s).p;
    ItemPos tx = (d.ins.int_right_size() > 0 && d.t == d.ins.last_sorted_item())?
            d.ins.last_item()+1: d.t;
    ItemPos sx = (d.ins.int_left_size() > 0 && d.s == d.ins.first_sorted_item())?
            d.ins.first_item()-1: d.s-1;
    LOG(d.info, "sx " << sx << " tx " << tx << std::endl);
    std::vector<StatePart> l;
    std::vector<StatePart>::iterator it = d.l0.begin();
    std::vector<StatePart>::iterator it1 = d.l0.begin();
    while (it != d.l0.end() || it1 != d.l0.end()) {
        if (it1 == d.l0.end() || it->w <= it1->w - ws) {
            LOG(d.info, "State " << *it);
            it->sol = d.psolf.add(it->sol);
            if (l.empty() || it->p > l.back().p) {
                if (!l.empty() && it->w == l.back().w) {
                    l.back() = *it;
                    LOG(d.info, " ok" << std::endl);
                } else {
                    Profit ub = (it->w <= c)?
                        ub_dembo(d.ins, tx, it->p, c-it->w):
                        ub_dembo_rev(d.ins, sx, it->p, c-it->w);
                    LOG(d.info, " ub " << ub << " lb " << d.lb);
                    if (ub > d.lb) {
                        l.push_back(*it);
                        LOG(d.info, " ok" << std::endl);
                    } else {
                        LOG(d.info, " ×" << std::endl);
                    }
                }
            } else {
                LOG(d.info, " ×" << std::endl);
            }
            ++it;
        } else {
            StatePart s1{it1->w-ws, it1->p-ps, d.psolf.remove(it1->sol)};
            LOG(d.info, "State " << *it1 << " => " << s1);
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
                    LOG(d.info, " ok" << std::endl);
                } else {
                    Profit ub = (s1.w <= c)?
                        ub_dembo(d.ins, tx, s1.p, c-s1.w):
                        ub_dembo_rev(d.ins, sx, s1.p, c-s1.w);
                    LOG(d.info, " ub " << ub << " lb " << d.lb);
                    if (ub > d.lb) {
                        l.push_back(s1);
                        d.distinct_state_number++;
                        LOG(d.info, " ok" << std::endl);
                    } else {
                        LOG(d.info, " ×" << std::endl);
                    }
                }
            } else {
                LOG(d.info, " ×" << std::endl);
            }
            it1++;
        }
    }
    d.l0 = std::move(l);
    LOG_FOLD_END(d.info, "");
}

Solution knapsack::sopt_minknap(Instance& ins, Info& info,
        MinknapParams params, ItemPos k, Profit o)
{
    if (o == -1)
        VER(info, "*** minknap (" << k << ") ***" << std::endl);

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

    ins.sort_partially(info);
    if (ins.break_item() == ins.last_item()+1) { // all items are in the break solution
        if (ins.break_solution()->profit() > sol.profit())
            sol.update(*ins.break_solution(), info, data.lb_number, data.ub);
        return algorithm_end(sol, info);
    }

    VER(info, "Compute lower bound..." << std::endl);
    if (params.lb_greedynlogn == 0) {
        params.lb_greedynlogn = -1;
        Info info_tmp(info.logger);
        Solution sol_tmp = sol_greedynlogn(ins, info_tmp);
        if (sol_tmp.profit() > data.lb) {
            sol.update(sol_tmp, info, data.lb_number, data.ub);
            data.lb = sol.profit();
        }
    } else if (params.lb_greedy >= 0) {
        Info info_tmp(info.logger);
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

    Info info_tmp(info.logger);
    Profit ub_tmp = ub_dantzig(ins, info_tmp);
    if (data.ub == -1 || ub_tmp < data.ub)
        Solution::update_ub(data.ub, ub_tmp, info, data.ub_number, data.lb);

    if (data.lb == data.ub) // If UB == LB, then stop
        return algorithm_end(sol, info);

    // Create memory table
    VER(info, "Recursion..." << std::endl);
    data.l0.push_back({w_bar, p_bar, 0});

    data.s = ins.break_item() - 1;
    data.t = ins.break_item();
    data.best_state = data.l0.front();
    data.distinct_state_number = 1;
    StateIdx state_number = 1;
    while (!data.l0.empty()) {
        LOG(info, "f " << ins.first_item() << " s' " << ins.first_sorted_item()
                << " s " << data.s << " t " << data.t
                << " t' " << ins.last_sorted_item() << " l " << ins.last_item()
                << std::endl);

        if (ins.int_right_size() > 0 && data.t+1 > ins.last_sorted_item())
            ins.sort_right(data.lb);
        if (data.t <= ins.last_sorted_item() && data.t <= ins.last_item()) {
            add_item(data);
            state_number += data.l0.size();
            ++data.t;
        }
        if (data.lb == data.ub)
            break;

        LOG(info, "f " << ins.first_item() << " s' " << ins.first_sorted_item()
                << " s " << data.s << " t " << data.t
                << " t' " << ins.last_sorted_item() << " l " << ins.last_item()
                << std::endl);

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

    VER(info, "Total state number: " << state_number << std::endl);
    VER(info, "Distinct state number: " << data.distinct_state_number << std::endl);
    PUT(info, "Algorithm.TotalStateNumber", state_number);
    PUT(info, "Algorithm.DistinctStateNumber", data.distinct_state_number);

    if (data.best_state.p <= sol.profit())
        return algorithm_end(sol, info);
    assert(data.best_state.p >= data.lb);

    ins.set_first_item(data.s+1);
    ins.set_last_item(data.t-1);
    bool b = ins.update_sorted();
    (void)b;
    assert(b);
    ins.fix(info, data.psolf.vector(data.best_state.sol));

    Info info_tmp2(info.logger);
    sol = knapsack::sopt_minknap(ins, info_tmp2, params, k, data.best_state.p);
    return algorithm_end(sol, info);
}

