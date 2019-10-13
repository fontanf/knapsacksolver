#include "knapsack/opt_minknap/minknap.hpp"

#include "knapsack/lib/part_solution_2.hpp"
#include "knapsack/lb_greedy/greedy.hpp"
#include "knapsack/lb_greedynlogn/greedynlogn.hpp"
#include "knapsack/ub_dembo/dembo.hpp"
#include "knapsack/ub_dantzig/dantzig.hpp"
#include "knapsack/ub_surrogate/surrogate.hpp"

using namespace knapsack;

void sopt_minknap_main(Instance& ins, MinknapOptionalParameters& p, MinknapOutput& output);

MinknapOutput knapsack::sopt_minknap(Instance& ins, MinknapOptionalParameters p)
{
    VER(p.info, "*** minknap");
    if (p.partial_solution_size != 64)
        VER(p.info, " k " << p.partial_solution_size);
    if (!p.greedy)
        VER(p.info, " g false");
    if (p.pairing != -1)
        VER(p.info, " p " << p.pairing);
    if (p.surrogate != -1)
        VER(p.info, " s " << p.surrogate);
    if (p.combo_core)
        VER(p.info, " cc true");
    VER(p.info, " ***" << std::endl);

    LOG_FOLD_START(p.info, "minknap"
            << " k " << p.partial_solution_size
            << " g " << p.greedy
            << " p " << p.pairing
            << " s " << p.surrogate
            << " combo_core " << p.combo_core
            << std::endl);

    bool end = false;
    if (p.end == NULL)
        p.end = &end;

    MinknapOutput output(ins, p.info);
    sopt_minknap_main(ins, p, output);

    output.algorithm_end(p.info);
    LOG_FOLD_END(p.info, "minknap");
    return output;
}

/******************************************************************************/

struct MinknapState
{
    Weight w;
    Profit p;
    PartSol2 sol;
};

std::ostream& operator<<(std::ostream& os, const MinknapState& s)
{
    os << "(" << s.w << " " << s.p << ")";
    return os;
}

struct MinknapInternalData
{
    MinknapInternalData(Instance& ins, MinknapOptionalParameters& p, MinknapOutput& output):
        ins(ins), p(p), output(output), psolf(ins, p.partial_solution_size) { }
    Instance& ins;
    MinknapOptionalParameters& p;
    MinknapOutput& output;
    PartSolFactory2 psolf;
    ItemPos s;
    ItemPos t;
    Weight w_max;
    std::vector<MinknapState> l0;
    std::vector<MinknapState> l;
    MinknapState best_state;
    std::vector<std::thread> threads;
};

void add_item(MinknapInternalData& d);
void remove_item(MinknapInternalData& d);
void sopt_minknap_update_bounds(MinknapInternalData& d);

void sopt_minknap_main(Instance& ins, MinknapOptionalParameters& p, MinknapOutput& output)
{
    output.recursive_call_number++;
    LOG_FOLD_START(p.info, "minknap_main"
            << " recursive_call_number " << output.recursive_call_number
            << std::endl);
    LOG_FOLD(p.info, ins);

    MinknapInternalData d(ins, p, output);
    Weight  c = ins.reduced_capacity();
    ItemPos n = ins.reduced_item_number();

    // Trivial cases
    if (n == 0 || c == 0) {
        Solution sol_tmp = (ins.reduced_solution() == NULL)? Solution(ins): *ins.reduced_solution();
        output.update_sol(sol_tmp, std::stringstream("no item or null capacity (lb)"), p.info);
        output.update_ub(output.lower_bound, std::stringstream("no item of null capacity (ub)"), p.info);
        LOG_FOLD_END(p.info, "no item or null capacity");
        return;
    } else if (n == 1) {
        Solution sol_tmp = (ins.reduced_solution() == NULL)? Solution(ins): *ins.reduced_solution();
        sol_tmp.set(ins.first_item(), true);
        output.update_sol(sol_tmp, std::stringstream("one item (lb)"), p.info);
        output.update_ub(output.lower_bound, std::stringstream("one item (ub)"), p.info);
        LOG_FOLD_END(p.info, "one item");
        return;
    }

    // Sort partially
    ins.sort_partially(p.info);
    if (ins.break_item() == ins.last_item() + 1) {
        output.update_sol(*ins.break_solution(), std::stringstream("all items fit in the knapsack (lb)"), p.info);
        output.update_ub(output.lower_bound, std::stringstream("all items fit in the knapsack (ub)"), p.info);
        LOG_FOLD_END(p.info, "all items fit in the knapsack");
        return;
    }
    if (output.recursive_call_number == 1 && p.combo_core) {
        ins.init_combo_core(p.info);
        LOG_FOLD(p.info, ins);
    }

    // Compute initial lower bound
    Solution sol_tmp(ins);
    if (p.greedy) {
        auto g_output = sol_greedy(ins);
        sol_tmp = g_output.solution;
    } else {
        sol_tmp = *ins.break_solution();
    }
    if (output.lower_bound < sol_tmp.profit())
        output.update_sol(sol_tmp, std::stringstream("initial solution"), p.info);

    // Compute initial upper bound
    Profit ub_tmp = ub_dantzig(ins);
    output.update_ub(ub_tmp, std::stringstream("dantzig upper bound"), p.info);

    if (output.solution.profit() == output.upper_bound) {
        LOG_FOLD_END(p.info, "lower bound == upper bound");
        return;
    }

    // Recursion
    Weight w_bar = ins.break_solution()->weight();
    Profit p_bar = ins.break_solution()->profit();
    d.l0 = {{.w = w_bar, .p = p_bar, .sol = 0}};
    d.s = ins.break_item() - 1;
    d.t = ins.break_item();
    d.w_max = w_bar;
    d.best_state = d.l0.front();
    LOG_FOLD(p.info, ins);
    while (!d.l0.empty()) {
        sopt_minknap_update_bounds(d); // Update bounds
        if (!p.info.check_time()) {
            if (p.set_end)
                *(p.end) = true;
            for (std::thread& thread: d.threads)
                thread.join();
            d.threads.clear();
            return;
        }
        if (p.stop_if_end && *(p.end)) {
            LOG_FOLD_END(p.info, "end");
            return;
        }
        if (output.solution.profit() == output.upper_bound
                || d.best_state.p == output.upper_bound)
            break;

        if (d.t <= ins.last_item()) {
            LOG(p.info, "f " << ins.first_item()
                    << " s'' " << ins.s_second()
                    << " s' " << ins.s_prime()
                    << " s " << d.s
                    << " b " << ins.break_item()
                    << " t " << d.t
                    << " t' " << ins.t_prime()
                    << " t'' " << ins.t_second()
                    << " l " << ins.last_item()
                    << std::endl);
            ++d.t;
            add_item(d);
            if (!p.info.check_time()) {
                if (p.set_end)
                    *(p.end) = true;
                for (std::thread& thread: d.threads)
                    thread.join();
                d.threads.clear();
                return;
            }
            if (p.stop_if_end && *(p.end)) {
                LOG_FOLD_END(p.info, "end");
                return;
            }
            if (d.best_state.p == output.upper_bound)
                break;
        }

        if (d.s >= ins.first_item()) {
            LOG(p.info, "f " << ins.first_item()
                    << " s'' " << ins.s_second()
                    << " s' " << ins.s_prime()
                    << " s " << d.s
                    << " b " << ins.break_item()
                    << " t " << d.t
                    << " t' " << ins.t_prime()
                    << " t'' " << ins.t_second()
                    << " l " << ins.last_item()
                    << std::endl);
            --d.s;
            remove_item(d);
            if (!p.info.check_time())
                break;
            if (p.stop_if_end && *(p.end)) {
                LOG_FOLD_END(p.info, "end");
                return;
            }
            if (d.best_state.p == output.upper_bound)
                break;
        }
    }
    output.update_ub(output.lower_bound, std::stringstream("tree search completed"), p.info);

    if (p.set_end)
        *(p.end) = true;
    LOG(p.info, "end" << std::endl);
    for (std::thread& thread: d.threads)
        thread.join();
    d.threads.clear();
    LOG(p.info, "end2" << std::endl);
    //if (!d.sur_)
        //*(d.end_) = false;

    if (output.lower_bound == output.solution.profit())
        return;

    //assert(best_state_.p >= lb_);
    LOG_FOLD(p.info, ins);
    ins.set_first_item(d.s + 1, p.info);
    ins.set_last_item(d.t - 1);
    LOG(p.info, "best_state " << d.best_state << std::endl);
    LOG(p.info, d.psolf.print(d.best_state.sol) << std::endl);
    ins.fix(p.info, d.psolf.vector(d.best_state.sol));
    assert(ins.reduced_capacity() >= 0);

    LOG_FOLD_END(p.info, "minknap_main");
    sopt_minknap_main(ins, p, output);
}

/******************************************************************************/

void add_item(MinknapInternalData& d)
{
    Instance& ins = d.ins;
    Info& info = d.p.info;
    Profit lb = (d.output.recursive_call_number == 1)?
        d.output.lower_bound:
        d.output.lower_bound - 1;
    LOG_FOLD_START(info, "add_item"
            << " s " << d.s
            << " t " << d.t
            << " item " << ins.item(d.t - 1)
            << " lb " << lb
            << std::endl);
    d.psolf.add_item(d.t - 1);
    LOG(info, "psolf " << d.psolf.print() << std::endl);
    d.best_state.sol = d.psolf.remove(d.best_state.sol);
    Weight c = ins.capacity();
    Weight wt = ins.item(d.t - 1).w;
    Profit pt = ins.item(d.t - 1).p;
    ItemPos sx = ins.bound_item_left(d.s, lb, info);
    ItemPos tx = ins.bound_item_right(d.t, lb, info);
    Profit ub_max = -1;
    Weight w_max = ins.capacity() + d.w_max - ins.reduced_solution()->weight();

    d.l.clear();
    std::vector<MinknapState>::iterator it = d.l0.begin();
    std::vector<MinknapState>::iterator it1 = d.l0.begin();
    while (it != d.l0.end() || it1 != d.l0.end()) {
        if (it == d.l0.end() || it->w > it1->w + wt) {
            MinknapState s1{it1->w + wt, it1->p + pt, d.psolf.add(it1->sol)};
            LOG(info, "state " << *it1 << " => " << s1);

            Profit ub = (s1.w <= c)?
                ub_dembo(ins, tx, s1.p, c - s1.w):
                ub_dembo_rev(ins, sx, s1.p, c - s1.w);
            LOG(info, " ub " << ub << " lb " << lb);

            if (s1.w <= w_max
                    && ub > lb
                    && (d.l.empty() || s1.p > d.l.back().p)) {
                // Update lower bound
                if (s1.w <= c && s1.p > lb) {
                    if (d.output.recursive_call_number == 1) {
                        std::stringstream ss;
                        ss << "it " << d.t - d.s << " (lb)";
                        d.output.update_lb(s1.p, ss, info);
                        lb = s1.p;
                    }
                    d.best_state = s1;
                    assert(d.output.lower_bound <= d.output.upper_bound);
                    if (d.best_state.p == d.output.upper_bound) {
                        LOG_FOLD_END(info, " lb == ub");
                        return;
                    }
                }

                if (ub_max < ub)
                    ub_max = ub;
                if (!d.l.empty() && s1.w == d.l.back().w) {
                    d.l.back() = s1;
                    LOG(info, " ok" << std::endl);
                } else {
                    d.l.push_back(s1);
                    LOG(info, " ok" << std::endl);
                }
            } else {
                LOG(info, " ×" << std::endl);
            }
            it1++;
        } else {
            assert(it != d.l0.end());
            LOG(info, "state " << *it);

            Profit ub = (it->w <= c)?
                ub_dembo(ins, tx, it->p, c - it->w):
                ub_dembo_rev(ins, sx, it->p, c - it->w);
            LOG(info, " ub " << ub << " lb " << lb);

            if (it->w <= w_max
                    && ub > lb
                    && (d.l.empty() || it->p > d.l.back().p)) {
                if (ub_max < ub)
                    ub_max = ub;
                it->sol = d.psolf.remove(it->sol);
                if (!d.l.empty() && it->w == d.l.back().w) {
                    d.l.back() = *it;
                    LOG(info, " ok" << std::endl);
                } else {
                    d.l.push_back(*it);
                    LOG(info, " ok" << std::endl);
                }
            } else {
                LOG(info, " ×" << std::endl);
            }
            ++it;
        }
    }
    if (d.output.recursive_call_number == 1 && ub_max != -1 && d.output.upper_bound > ub_max) {
        std::stringstream ss;
        ss << "it " << d.t - d.s << " (ub)";
        d.output.update_ub(ub_max, ss, info);
    }
    d.l0.swap(d.l);
    LOG_FOLD_END(info, "add_item");
}

void remove_item(MinknapInternalData& d)
{
    Instance& ins = d.ins;
    Info& info = d.p.info;
    Profit lb = (d.output.recursive_call_number == 1)?
        d.output.lower_bound:
        d.output.lower_bound - 1;
    LOG_FOLD_START(info, "remove_item"
            << " s " << d.s
            << " t " << d.t
            << " item " << ins.item(d.s + 1)
            << " lb " << lb << std::endl);
    d.psolf.add_item(d.s + 1);
    LOG(info, "psolf " << d.psolf.print() << std::endl);
    d.best_state.sol = d.psolf.add(d.best_state.sol);
    Weight c = ins.capacity();
    Weight ws = ins.item(d.s + 1).w;
    Profit ps = ins.item(d.s + 1).p;
    d.w_max -= ws;
    ItemPos sx = ins.bound_item_left(d.s, lb, info);
    ItemPos tx = ins.bound_item_right(d.t, lb, info);
    Profit ub_max = -1;
    Weight w_max = ins.capacity() + d.w_max - ins.reduced_solution()->weight();

    d.l.clear();
    std::vector<MinknapState>::iterator it = d.l0.begin();
    std::vector<MinknapState>::iterator it1 = d.l0.begin();
    while (it != d.l0.end() || it1 != d.l0.end()) {
        if (it1 == d.l0.end() || it->w <= it1->w - ws) {
            LOG(info, "state " << *it);

            Profit ub = (it->w <= c)?
                ub_dembo(ins, tx, it->p, c - it->w):
                ub_dembo_rev(ins, sx, it->p, c - it->w);
            LOG(info, " ub " << ub << " lb " << lb);

            if (it->w <= w_max
                    && ub > lb
                    && (d.l.empty() || it->p > d.l.back().p)) {
                if (ub_max < ub)
                    ub_max = ub;
                it->sol = d.psolf.add(it->sol);
                if (!d.l.empty() && it->w == d.l.back().w) {
                    d.l.back() = *it;
                    LOG(info, " ok" << std::endl);
                } else {
                    d.l.push_back(*it);
                    LOG(info, " ok" << std::endl);
                }
            } else {
                LOG(info, " ×" << std::endl);
            }
            ++it;
        } else {
            MinknapState s1{it1->w - ws, it1->p - ps, d.psolf.remove(it1->sol)};
            LOG(info, "state " << *it1 << " => " << s1);

            Profit ub = (s1.w <= c)?
                ub_dembo(ins, tx, s1.p, c - s1.w):
                ub_dembo_rev(ins, sx, s1.p, c - s1.w);
            LOG(info, " ub " << ub << " lb " << lb);

            if (s1.w <= w_max
                    && ub > lb
                    && (d.l.empty() || s1.p > d.l.back().p)) {
                // Update lower bound
                if (s1.w <= c && s1.p > lb) {
                    if (d.output.recursive_call_number == 1) {
                        std::stringstream ss;
                        ss << "it " << d.t - d.s << " (lb)";
                        d.output.update_lb(s1.p, ss, info);
                        lb = s1.p;
                    }
                    d.best_state = s1;
                    assert(d.output.lower_bound <= d.output.upper_bound);
                    if (d.best_state.p == d.output.upper_bound) {
                        LOG_FOLD_END(info, " lb == ub");
                        return;
                    }
                }

                if (ub_max < ub)
                    ub_max = ub;
                if (!d.l.empty() && s1.w == d.l.back().w) {
                    d.l.back() = s1;
                    LOG(info, " ok" << std::endl);
                } else {
                    d.l.push_back(s1);
                    LOG(info, " ok" << std::endl);
                }
            } else {
                LOG(info, " ×" << std::endl);
            }
            it1++;
        }
    }
    if (d.output.recursive_call_number == 1 && ub_max != -1 && d.output.upper_bound > ub_max) {
        std::stringstream ss;
        ss << "it " << d.t - d.s << " (ub)";
        d.output.update_ub(ub_max, ss, info);
    }
    d.l0.swap(d.l);
    LOG_FOLD_END(info, "remove_item");
}

/******************************************************************************/

ItemPos sopt_minknap_find_state(MinknapInternalData& d, bool right)
{
    Instance& ins = d.ins;
    LOG_FOLD_START(d.p.info, "sopt_minknap_find_state" << std::endl);

    Profit lb0 = 0;
    ItemIdx j = -1;
    ItemPos first = (right)? d.t: ins.first_item();
    ItemPos last  = (right)? ins.last_item(): d.s;
    for (ItemPos t=first; t<=last; ++t) {
        if (ins.s_second() <= t && t < ins.s_prime())
            continue;
        if (ins.t_prime() < t && t <= ins.t_second())
            continue;
        LOG(d.p.info, "t " << t << std::endl);
        Weight w = (right)?
            ins.capacity() - ins.item(t).w:
            ins.capacity() + ins.item(t).w;
        if (d.l0.front().w > w)
            continue;
        ItemPos f = 0;
        ItemPos l = d.l0.size() - 1; // l0_[l] > w
        while (f + 1 < l) {
            LOG(d.p.info, "f " << f << " l " << l << std::endl);
            ItemPos m = (f + l) / 2;
            if (d.l0[m].w >= w) {
                assert(l != m);
                l = m;
            } else {
                assert(f != m);
                f = m;
            }
        }
        LOG(d.p.info, "f " << f << " l " << l << std::endl);
        if (f != (StateIdx)d.l0.size() - 1 && d.l0[f + 1].w <= w)
            f++;
        LOG(d.p.info, "f " << f << " l " << l << std::endl);
        assert(f < (StateIdx)d.l0.size());
        assert(d.l0[f].w <= w);
        assert(f == (StateIdx)d.l0.size() - 1 || d.l0[f + 1].w > w);
        Profit lb = (right)?
            d.l0[f].p + ins.item(t).p:
            d.l0[f].p - ins.item(t).p;
        if (lb0 < lb) {
            j = t;
            lb0 = lb;
        }
    }
    LOG_FOLD_END(d.p.info, "sopt_minknap_find_state");
    return j;
}

void sopt_minknap_update_bounds(MinknapInternalData& d)
{
    Instance& ins = d.ins;
    Info& info = d.p.info;

    if (d.p.surrogate >= 0 && d.p.surrogate <= (StateIdx)d.l0.size()) {
        d.p.surrogate = -1;
        std::function<Output (Instance&, Info, bool*)> func
            = [&d](Instance& ins, Info info, bool* end)
            {
                MinknapOptionalParameters p;
                p.info = info;
                p.partial_solution_size = d.p.partial_solution_size;
                p.pairing = d.p.pairing;
                p.greedy = d.p.greedy;
                p.surrogate = -1;
                p.combo_core = d.p.combo_core;
                p.end = end;
                p.stop_if_end = true;
                p.set_end = false;
                return sopt_minknap(ins, p);
            };
        d.threads.push_back(std::thread(ub_solvesurrelax, SurrelaxData{
                    .ins      = Instance::reset(ins),
                    .output   = d.output,
                    .func     = func,
                    .end      = d.p.end,
                    .info     = Info(info, true, "surrelax")}));
    }
    if (d.p.pairing >= 0 && d.p.pairing <= (StateIdx)d.l0.size()) {
        LOG_FOLD_START(info, "pairing" << std::endl);
        d.p.pairing *= 10;

        if (d.t <= ins.last_item()) {
            ItemPos j = sopt_minknap_find_state(d, true);
            if (j != -1) {
                ins.add_item_to_core(d.s, d.t, j, info);
                ++d.t;
                add_item(d);
                if (d.output.solution.profit() == d.output.upper_bound
                        || !info.check_time()
                        || (d.p.stop_if_end && *(d.p.end)))
                    return;
            }
        }

        if (d.s >= ins.first_item()) {
            ItemPos j = sopt_minknap_find_state(d, false);
            if (j != -1) {
                ins.add_item_to_core(d.s, d.t, j, info);
                --d.s;
                remove_item(d);
            }
        }
        LOG_FOLD_END(info, "pairing");
    }
}

