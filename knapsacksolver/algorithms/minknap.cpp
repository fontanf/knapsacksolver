#include "knapsacksolver/algorithms/minknap.hpp"

#include "knapsacksolver/part_solution_2.hpp"
#include "knapsacksolver/algorithms/greedy.hpp"
#include "knapsacksolver/algorithms/greedynlogn.hpp"
#include "knapsacksolver/algorithms/dembo.hpp"
#include "knapsacksolver/algorithms/dantzig.hpp"
#include "knapsacksolver/algorithms/surrelax.hpp"

using namespace knapsacksolver;

void minknap_main(Instance& instance, MinknapOptionalParameters& p, MinknapOutput& output);

MinknapOutput knapsacksolver::minknap(Instance& instance, MinknapOptionalParameters p)
{
    VER(p.info, "*** minknap"
            << " -k " << p.partial_solution_size
            << ((p.greedy)? " -g": "")
            << " -p " << p.pairing
            << " -s " << p.surrelax
            << ((p.combo_core)? " -c": "")
            << " ***" << std::endl);

    LOG_FOLD_START(p.info, "*** minknap"
            << " -k " << p.partial_solution_size
            << ((p.greedy)? " -g": "")
            << " -p " << p.pairing
            << " -s " << p.surrelax
            << ((p.combo_core)? " -c": "")
            << " ***" << std::endl);

    bool end = false;
    if (p.end == NULL)
        p.end = &end;

    MinknapOutput output(instance, p.info);
    minknap_main(instance, p, output);

    LOG_FOLD_END(p.info, "minknap");
    return output.algorithm_end(p.info);
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
    MinknapInternalData(Instance& instance, MinknapOptionalParameters& p, MinknapOutput& output):
        instance(instance), p(p), output(output), psolf(instance, p.partial_solution_size) { }
    Instance& instance;
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
void minknap_update_bounds(MinknapInternalData& d);

void minknap_main(Instance& instance, MinknapOptionalParameters& p, MinknapOutput& output)
{
    output.number_of_recursive_calls++;
    LOG_FOLD_START(p.info, "minknap_main"
            << " number_of_recursive_calls " << output.number_of_recursive_calls
            << std::endl);
    LOG_FOLD(p.info, instance);

    MinknapInternalData d(instance, p, output);
    Weight  c = instance.reduced_capacity();
    ItemPos n = instance.reduced_number_of_items();

    // Trivial cases
    if (n == 0 || c == 0) {
        Solution sol_tmp = (instance.reduced_solution() == NULL)? Solution(instance): *instance.reduced_solution();
        output.update_sol(sol_tmp, std::stringstream("no item or null capacity (lb)"), p.info);
        output.update_ub(output.lower_bound, std::stringstream("no item of null capacity (ub)"), p.info);
        LOG_FOLD_END(p.info, "no item or null capacity");
        return;
    } else if (n == 1) {
        Solution sol_tmp = (instance.reduced_solution() == NULL)? Solution(instance): *instance.reduced_solution();
        sol_tmp.set(instance.first_item(), true);
        output.update_sol(sol_tmp, std::stringstream("one item (lb)"), p.info);
        output.update_ub(output.lower_bound, std::stringstream("one item (ub)"), p.info);
        LOG_FOLD_END(p.info, "one item");
        return;
    }

    // Sort partially
    instance.sort_partially(p.info);
    if (instance.break_item() == instance.last_item() + 1) {
        output.update_sol(*instance.break_solution(), std::stringstream("all items fit in the knapsack (lb)"), p.info);
        output.update_ub(output.lower_bound, std::stringstream("all items fit in the knapsack (ub)"), p.info);
        LOG_FOLD_END(p.info, "all items fit in the knapsack");
        return;
    }
    if (output.number_of_recursive_calls == 1 && p.combo_core) {
        instance.init_combo_core(p.info);
        LOG_FOLD(p.info, instance);
    }

    // Compute initial lower bound
    Solution sol_tmp(instance);
    if (p.greedy) {
        auto g_output = greedy(instance);
        sol_tmp = g_output.solution;
    } else {
        sol_tmp = *instance.break_solution();
    }
    if (output.lower_bound < sol_tmp.profit())
        output.update_sol(sol_tmp, std::stringstream("initial solution"), p.info);

    // Compute initial upper bound
    Profit ub_tmp = ub_dantzig(instance);
    output.update_ub(ub_tmp, std::stringstream("dantzig upper bound"), p.info);

    if (output.solution.profit() == output.upper_bound) {
        LOG_FOLD_END(p.info, "lower bound == upper bound");
        return;
    }

    // Recursion
    Weight w_bar = instance.break_solution()->weight();
    Profit p_bar = instance.break_solution()->profit();
    d.l0 = {{.w = w_bar, .p = p_bar, .sol = 0}};
    d.s = instance.break_item() - 1;
    d.t = instance.break_item();
    d.w_max = w_bar;
    d.best_state = d.l0.front();
    LOG_FOLD(p.info, instance);
    while (!d.l0.empty() && (d.t <= instance.last_item() || d.s >= instance.first_item())) {
        minknap_update_bounds(d); // Update bounds
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

        if (d.t <= instance.last_item()) {
            LOG(p.info, "f " << instance.first_item()
                    << " s'' " << instance.s_second()
                    << " s' " << instance.s_prime()
                    << " s " << d.s
                    << " b " << instance.break_item()
                    << " t " << d.t
                    << " t' " << instance.t_prime()
                    << " t'' " << instance.t_second()
                    << " l " << instance.last_item()
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

        if (d.s >= instance.first_item()) {
            LOG(p.info, "f " << instance.first_item()
                    << " s'' " << instance.s_second()
                    << " s' " << instance.s_prime()
                    << " s " << d.s
                    << " b " << instance.break_item()
                    << " t " << d.t
                    << " t' " << instance.t_prime()
                    << " t'' " << instance.t_second()
                    << " l " << instance.last_item()
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
    LOG_FOLD(p.info, instance);
    instance.set_first_item(d.s + 1, p.info);
    instance.set_last_item(d.t - 1);
    LOG(p.info, "best_state " << d.best_state << std::endl);
    LOG(p.info, d.psolf.print(d.best_state.sol) << std::endl);
    instance.fix(p.info, d.psolf.vector(d.best_state.sol));
    assert(instance.reduced_capacity() >= 0);

    LOG_FOLD_END(p.info, "minknap_main");
    minknap_main(instance, p, output);
}

/******************************************************************************/

void add_item(MinknapInternalData& d)
{
    Instance& instance = d.instance;
    Info& info = d.p.info;
    Profit lb = (d.output.number_of_recursive_calls == 1)?
        d.output.lower_bound:
        d.output.lower_bound - 1;
    LOG_FOLD_START(info, "add_item"
            << " s " << d.s
            << " t " << d.t
            << " item " << instance.item(d.t - 1)
            << " lb " << lb
            << std::endl);
    d.psolf.add_item(d.t - 1);
    LOG(info, "psolf " << d.psolf.print() << std::endl);
    d.best_state.sol = d.psolf.remove(d.best_state.sol);
    Weight c = instance.capacity();
    Weight wt = instance.item(d.t - 1).w;
    Profit pt = instance.item(d.t - 1).p;
    ItemPos sx = instance.bound_item_left(d.s, lb, info);
    ItemPos tx = instance.bound_item_right(d.t, lb, info);
    Weight w_max = instance.capacity() + d.w_max - instance.reduced_solution()->weight();

    d.l.clear();
    std::vector<MinknapState>::iterator it = d.l0.begin();
    std::vector<MinknapState>::iterator it1 = d.l0.begin();
    while (it != d.l0.end() || it1 != d.l0.end()) {
        if (it == d.l0.end() || it->w > it1->w + wt) {
            Weight w = it1->w + wt;
            if (w > w_max) {
                LOG(info, " ×" << std::endl);
                it1++;
                continue;
            }

            Profit p = it1->p + pt;
            if (!d.l.empty() && p <= d.l.back().p) {
                LOG(info, " ×" << std::endl);
                it1++;
                continue;
            }

            Profit ub = (w <= c)?
                ub_dembo(instance, tx, p, c - w):
                ub_dembo_rev(instance, sx, p, c - w);
            LOG(info, " ub " << ub << " lb " << lb);
            if (ub <= lb) {
                LOG(info, " ×" << std::endl);
                it1++;
                continue;
            }

            MinknapState s1{w, p, d.psolf.add(it1->sol)};
            LOG(info, "state " << *it1 << " => " << s1);

            // Update lower bound
            if (s1.w <= c && s1.p > lb) {
                if (d.output.number_of_recursive_calls == 1) {
                    std::stringstream ss;
                    ss << "it " << d.t - d.s << " (lb)";
                    d.output.update_lb(s1.p, ss, info);
                    lb = s1.p;
                }
                d.best_state = s1;
                assert(d.output.lower_bound <= d.output.upper_bound);
            }

            if (!d.l.empty() && s1.w == d.l.back().w) {
                d.l.back() = s1;
            } else {
                d.l.push_back(s1);
            }
            LOG(info, " ok" << std::endl);
            it1++;
        } else {
            assert(it != d.l0.end());
            LOG(info, "state " << *it);

            if (it->w > w_max) {
                LOG(info, " ×" << std::endl);
                it++;
                continue;
            }

            if (!d.l.empty() && it->p <= d.l.back().p) {
                LOG(info, " ×" << std::endl);
                it++;
                continue;
            }

            Profit ub = (it->w <= c)?
                ub_dembo(instance, tx, it->p, c - it->w):
                ub_dembo_rev(instance, sx, it->p, c - it->w);
            LOG(info, " ub " << ub << " lb " << lb);
            if (ub <= lb) {
                LOG(info, " ×" << std::endl);
                it++;
                continue;
            }

            it->sol = d.psolf.remove(it->sol);
            if (!d.l.empty() && it->w == d.l.back().w) {
                d.l.back() = *it;
            } else {
                d.l.push_back(*it);
            }
            LOG(info, " ok" << std::endl);
            ++it;
        }
    }
    d.l0.swap(d.l);
    LOG_FOLD_END(info, "add_item");
}

void remove_item(MinknapInternalData& d)
{
    Instance& instance = d.instance;
    Info& info = d.p.info;
    Profit lb = (d.output.number_of_recursive_calls == 1)?
        d.output.lower_bound:
        d.output.lower_bound - 1;
    LOG_FOLD_START(info, "remove_item"
            << " s " << d.s
            << " t " << d.t
            << " item " << instance.item(d.s + 1)
            << " lb " << lb << std::endl);
    d.psolf.add_item(d.s + 1);
    LOG(info, "psolf " << d.psolf.print() << std::endl);
    d.best_state.sol = d.psolf.add(d.best_state.sol);
    Weight c = instance.capacity();
    Weight ws = instance.item(d.s + 1).w;
    Profit ps = instance.item(d.s + 1).p;
    d.w_max -= ws;
    ItemPos sx = instance.bound_item_left(d.s, lb, info);
    ItemPos tx = instance.bound_item_right(d.t, lb, info);
    Weight w_max = instance.capacity() + d.w_max - instance.reduced_solution()->weight();

    d.l.clear();
    std::vector<MinknapState>::iterator it = d.l0.begin();
    std::vector<MinknapState>::iterator it1 = d.l0.begin();
    while (it != d.l0.end() || it1 != d.l0.end()) {
        if (it1 == d.l0.end() || it->w <= it1->w - ws) {
            LOG(info, "state " << *it);

            if (it->w > w_max) {
                LOG(info, " ×" << std::endl);
                it++;
                continue;
            }

            if (!d.l.empty() && it->p <= d.l.back().p) {
                LOG(info, " ×" << std::endl);
                it++;
                continue;
            }

            Profit ub = (it->w <= c)?
                ub_dembo(instance, tx, it->p, c - it->w):
                ub_dembo_rev(instance, sx, it->p, c - it->w);
            LOG(info, " ub " << ub << " lb " << lb);
            if (ub <= lb) {
                LOG(info, " ×" << std::endl);
                it++;
                continue;
            }

            it->sol = d.psolf.add(it->sol);
            if (!d.l.empty() && it->w == d.l.back().w) {
                d.l.back() = *it;
            } else {
                d.l.push_back(*it);
            }
            LOG(info, " ok" << std::endl);
            ++it;
        } else {
            Weight w = it1->w - ws;
            if (w > w_max) {
                LOG(info, " ×" << std::endl);
                it1++;
                continue;
            }

            Profit p = it1->p - ps;
            if (!d.l.empty() && p <= d.l.back().p) {
                LOG(info, " ×" << std::endl);
                it1++;
                continue;
            }

            Profit ub = (w <= c)?
                ub_dembo(instance, tx, p, c - w):
                ub_dembo_rev(instance, sx, p, c - w);
            LOG(info, " ub " << ub << " lb " << lb);
            if (ub <= lb) {
                LOG(info, " ×" << std::endl);
                it1++;
                continue;
            }

            MinknapState s1{it1->w - ws, it1->p - ps, d.psolf.remove(it1->sol)};
            LOG(info, "state " << *it1 << " => " << s1);

            // Update lower bound
            if (s1.w <= c && s1.p > lb) {
                if (d.output.number_of_recursive_calls == 1) {
                    std::stringstream ss;
                    ss << "it " << d.t - d.s << " (lb)";
                    d.output.update_lb(s1.p, ss, info);
                    lb = s1.p;
                }
                d.best_state = s1;
                assert(d.output.lower_bound <= d.output.upper_bound);
            }

            if (!d.l.empty() && s1.w == d.l.back().w) {
                d.l.back() = s1;
            } else {
                d.l.push_back(s1);
            }
            LOG(info, " ok" << std::endl);
            it1++;
        }
    }
    d.l0.swap(d.l);
    LOG_FOLD_END(info, "remove_item");
}

/******************************************************************************/

ItemPos minknap_find_state(MinknapInternalData& d, bool right)
{
    Instance& instance = d.instance;
    LOG_FOLD_START(d.p.info, "minknap_find_state" << std::endl);

    Profit lb0 = 0;
    ItemIdx j = -1;
    ItemPos first = (right)? d.t: instance.first_item();
    ItemPos last  = (right)? instance.last_item(): d.s;
    for (ItemPos t=first; t<=last; ++t) {
        if (instance.s_second() <= t && t < instance.s_prime())
            continue;
        if (instance.t_prime() < t && t <= instance.t_second())
            continue;
        LOG(d.p.info, "t " << t << std::endl);
        Weight w = (right)?
            instance.capacity() - instance.item(t).w:
            instance.capacity() + instance.item(t).w;
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
            d.l0[f].p + instance.item(t).p:
            d.l0[f].p - instance.item(t).p;
        if (lb0 < lb) {
            j = t;
            lb0 = lb;
        }
    }
    LOG_FOLD_END(d.p.info, "minknap_find_state");
    return j;
}

void minknap_update_bounds(MinknapInternalData& d)
{
    Instance& instance = d.instance;
    Info& info = d.p.info;

    if (d.p.surrelax >= 0 && d.p.surrelax <= (StateIdx)d.l0.size()) {
        d.p.surrelax = -1;
        std::function<Output (Instance&, Info, bool*)> func
            = [&d](Instance& instance, Info info, bool* end)
            {
                MinknapOptionalParameters p;
                p.info = info;
                p.partial_solution_size = d.p.partial_solution_size;
                p.pairing = d.p.pairing;
                p.greedy = d.p.greedy;
                p.surrelax = -1;
                p.combo_core = d.p.combo_core;
                p.end = end;
                p.stop_if_end = true;
                p.set_end = false;
                return minknap(instance, p);
            };
        d.threads.push_back(std::thread(solvesurrelax, SurrelaxData{
                    .instance = Instance::reset(instance),
                    .output   = d.output,
                    .func     = func,
                    .end      = d.p.end,
                    .info     = Info(info, true, "surrelax")}));
    }
    if (d.p.pairing >= 0 && d.p.pairing <= (StateIdx)d.l0.size()) {
        LOG_FOLD_START(info, "pairing" << std::endl);
        d.p.pairing *= 10;

        if (d.t <= instance.last_item()) {
            ItemPos j = minknap_find_state(d, true);
            if (j != -1) {
                instance.add_item_to_core(d.s, d.t, j, info);
                ++d.t;
                add_item(d);
                if (d.output.solution.profit() == d.output.upper_bound
                        || !info.check_time()
                        || (d.p.stop_if_end && *(d.p.end)))
                    return;
            }
        }

        if (d.s >= instance.first_item()) {
            ItemPos j = minknap_find_state(d, false);
            if (j != -1) {
                instance.add_item_to_core(d.s, d.t, j, info);
                --d.s;
                remove_item(d);
            }
        }
        LOG_FOLD_END(info, "pairing");
    }
}

