#include "knapsacksolver/algorithms/balknap.hpp"

#include "knapsacksolver/part_solution_1.hpp"
#include "knapsacksolver/algorithms/greedy.hpp"
#include "knapsacksolver/algorithms/greedynlogn.hpp"
#include "knapsacksolver/algorithms/dembo.hpp"
#include "knapsacksolver/algorithms/dantzig.hpp"
#include "knapsacksolver/algorithms/surrelax.hpp"

#include <bitset>

using namespace knapsacksolver;

void balknap_main(Instance& ins, BalknapOptionalParameters& p, BalknapOutput& output);

BalknapOutput knapsacksolver::balknap(Instance& ins, BalknapOptionalParameters p)
{
    VER(p.info, "*** balknap");
    if (p.partial_solution_size != 64)
        VER(p.info, " k " << p.partial_solution_size);
    if (!p.greedy)
        VER(p.info, " g false");
    if (p.greedynlogn != -1)
        VER(p.info, " gn " << p.greedynlogn);
    if (p.surrelax != -1)
        VER(p.info, " s " << p.surrelax);
    VER(p.info, " ***" << std::endl);

    LOG_FOLD_START(p.info, "balknap"
            << " k " << p.partial_solution_size
            << " g " << p.greedy
            << " gn " << p.greedynlogn
            << " s " << p.surrelax
            << std::endl);

    bool end = false;
    if (p.end == NULL)
        p.end = &end;

    BalknapOutput output(ins, p.info);
    balknap_main(ins, p, output);

    output.algorithm_end(p.info);
    LOG_FOLD_END(p.info, "balknap");
    return output;
}

/******************************************************************************/

struct BalknapState
{
    Weight mu;
    Profit pi;
    bool operator()(const BalknapState& s1, const BalknapState& s2)
    {
        if (s1.mu != s2.mu)
            return s1.mu < s2.mu;
        if (s1.pi != s2.pi)
            return s1.pi < s2.pi;
        return false;
    }
};

struct BalknapValue
{
    ItemPos a;
    ItemPos a_prec;
    PartSol1 sol;
};

std::ostream& operator<<(std::ostream& os, const std::pair<BalknapState, BalknapValue>& s)
{
    os
        << "(mu " << s.first.mu
        << " pi " << s.first.pi
        << " a " << s.second.a
        << " ap " << s.second.a_prec
        //<< " s " << std::bitset<16>(s.second.sol)
        << ")";
    return os;
}

struct BalknapInternalData
{
    BalknapInternalData(Instance& ins, BalknapOptionalParameters& p, BalknapOutput& output):
        ins(ins), p(p), output(output) { }
    Instance& ins;
    BalknapOptionalParameters& p;
    BalknapOutput& output;
    std::map<BalknapState, BalknapValue, BalknapState> map;
    std::vector<std::thread> threads;
};

void balknap_update_bounds(BalknapInternalData& d);

void balknap_main(Instance& ins, BalknapOptionalParameters& p, BalknapOutput& output)
{
    Info& info = p.info;
    output.recursive_call_number++;
    LOG_FOLD_START(info, "balknap_main"
            << " recursive_call_number " << output.recursive_call_number
            << std::endl);

    // Trivial cases
    if (ins.reduced_item_number() == 0 || ins.reduced_capacity() == 0) {
        Solution sol_tmp = (ins.reduced_solution() == NULL)? Solution(ins): *ins.reduced_solution();
        output.update_sol(sol_tmp, std::stringstream("no item of null capacity (lb)"), p.info);
        output.update_ub(output.lower_bound, std::stringstream("no item of null capacity (ub)"), p.info);
        LOG_FOLD_END(info, "no item or null capacity");
        return;
    } else if (ins.reduced_item_number() == 1) {
        Solution sol_tmp = (ins.reduced_solution() == NULL)? Solution(ins): *ins.reduced_solution();
        sol_tmp.set(ins.first_item(), true);
        output.update_sol(sol_tmp, std::stringstream("one item (lb)"), p.info);
        output.update_ub(output.lower_bound, std::stringstream("one item (ub)"), p.info);
        LOG_FOLD_END(p.info, "one item");
        return;
    }

    // Sorting
    if (p.ub == 'b') {
        ins.sort_partially(info);
    } else if (p.ub == 't') {
        ins.sort(info);
    }
    if (ins.break_item() == ins.last_item() + 1) {
        output.update_sol(*ins.break_solution(), std::stringstream("all items fit in the knapsack (lb)"), p.info);
        output.update_ub(output.lower_bound, std::stringstream("all items fit in the knapsack (ub)"), p.info);
        LOG_FOLD_END(p.info, "all items fit in the knapsack");
        return;
    }

    // Compute initial lower bound
    Solution sol_tmp(ins);
    if (p.greedy) {
        auto g_output = greedy(ins);
        sol_tmp = g_output.solution;
    } else {
        sol_tmp = *ins.break_solution();
    }
    if (output.lower_bound < sol_tmp.profit())
        output.update_sol(sol_tmp, std::stringstream("initial solution"), p.info);

    // Variable reduction
    // If we already know the optimal value, we can use opt-1 as lower bound
    // for the reduction.
    Profit lb_red = (output.recursive_call_number == 1)?
        output.lower_bound:
        output.lower_bound - 1;
    if (p.ub == 'b') {
        ins.reduce1(lb_red, info);
    } else if (p.ub == 't') {
        ins.reduce2(lb_red, info);
    }
    if (ins.reduced_capacity() < 0) {
        output.update_ub(output.lower_bound, std::stringstream("negative capacity after reduction"), info);
        LOG_FOLD_END(info, "c < 0");
        return;
    }

    if (output.solution.profit() < ins.break_solution()->profit())
        output.update_sol(*ins.break_solution(), std::stringstream("break solution after reduction"), p.info);

    Weight  c = ins.capacity();
    ItemPos f = ins.first_item();
    ItemPos l = ins.last_item();
    ItemPos n = ins.reduced_item_number();

    // Trivial cases
    if (n == 0 || ins.reduced_capacity() == 0) {
        Solution sol_tmp = (ins.reduced_solution() == NULL)? Solution(ins): *ins.reduced_solution();
        output.update_sol(sol_tmp, std::stringstream("no item or null capacity after reduction (lb)"), p.info);
        output.update_ub(output.lower_bound, std::stringstream("no item of null capacity after reduction (ub)"), p.info);
        LOG_FOLD_END(p.info, "no item or null capacity after reduction");
        return;
    } else if (n == 1) {
        Solution sol_tmp = (ins.reduced_solution() == NULL)? Solution(ins): *ins.reduced_solution();
        sol_tmp.set(ins.first_item(), true);
        output.update_sol(sol_tmp, std::stringstream("one item after reduction (lb)"), p.info);
        output.update_ub(output.lower_bound, std::stringstream("one item after reduction (ub)"), p.info);
        LOG_FOLD_END(p.info, "one item after reduction");
        return;
    } else if (ins.break_item() == ins.last_item()+1) {
        output.update_sol(*ins.break_solution(), std::stringstream("all items fit in the knapsack after reduction (lb)"), p.info);
        output.update_ub(output.lower_bound, std::stringstream("all items fit in the knapsack after reduction (ub)"), p.info);
        LOG_FOLD_END(p.info, "all items fit in the knapsack after reduction");
        return;
    }

    ItemPos b    = ins.break_item();
    Weight w_bar = ins.break_solution()->weight();
    Profit p_bar = ins.break_solution()->profit();

    // Compute initial upper bound
    Profit ub_tmp = std::max(ub_dantzig(ins), output.lower_bound);
    output.update_ub(ub_tmp, std::stringstream("dantzig upper bound"), p.info);

    if (output.solution.profit() == output.upper_bound) {
        LOG_FOLD_END(p.info, "lower bound == upper bound");
        return;
    }

    // Initialization
    // Create first partial solution centered on the break item.
    BalknapInternalData d(ins, p, output);
    PartSolFactory1 psolf(ins, p.partial_solution_size, b, f, l);
    PartSol1 psol_init = 0;
    for (ItemPos j=f; j<b; ++j)
        psol_init = psolf.add(psol_init, j);
    // s(w_bar,p_bar) = b
    d.map.insert({{w_bar, p_bar},{b, f, psol_init}});

    // Best state. Note that it is not a pointer
    std::pair<BalknapState, BalknapValue> best_state = {d.map.begin()->first, d.map.begin()->second};
    // Also keep last added item to improve the variable reduction at the end.
    ItemPos last_item = b-1;

    Profit lb = (d.output.recursive_call_number == 1)?
        d.output.lower_bound:
        d.output.lower_bound - 1;

    // Recursion
    for (ItemPos t=b; t<=l; ++t) {
        balknap_update_bounds(d);
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
                || best_state.first.pi == output.upper_bound)
            break;

        LOG(info, "t " << t << " (" << ins.item(t) << ")" << std::endl);
        Weight wt = ins.item(t).w;
        Profit pt = ins.item(t).p;

        // Bounding
        LOG(info, "bound" << std::endl);
        Profit ub_t = -1;
        for (auto s = d.map.begin(); s != d.map.end() && s->first.mu <= c;) {
            Profit pi = s->first.pi;
            Weight mu = s->first.mu;
            Profit ub_local = 0;
            if (d.p.ub == 'b') {
                ub_local = (mu <= c)?
                    ub_dembo(ins, b, pi, c-mu):
                    ub_dembo_rev(ins, b, pi, c-mu);
            } else if (d.p.ub == 't') {
                ub_local = (mu <= c)?
                    ub_dembo(ins, t, pi, c-mu):
                    ub_dembo_rev(ins, s->second.a, pi, c-mu);
            }
            if (ub_local < lb) {
                LOG(info, "remove " << *s << std::endl);
                d.map.erase(s++);
            } else {
                if (ub_t < ub_local)
                    ub_t = ub_local;
                s++;
            }
        }
        if (ub_t != -1 && output.upper_bound > ub_t) {
            std::stringstream ss;
            ss << "it " << t - b << " (ub)";
            output.update_ub(ub_t, ss, info);
            if (output.solution.profit() == output.upper_bound
                    || best_state.first.pi == output.upper_bound)
                goto end;
        }

        // If there is no more states, the stop
        if (d.map.size() == 0)
            break;
        if (best_state.first.pi == output.upper_bound)
            goto end;
        if (p.stop_if_end && *(p.end)) {
            LOG_FOLD_END(p.info, "end");
            return;
        }

        // Add item t
        LOG(info, "add" << std::endl);
        auto s = d.map.upper_bound({c+1,0});
        auto hint = s;
        hint--;
        while (s != d.map.begin() && (--s)->first.mu <= c) {
            std::pair<BalknapState, BalknapValue> s1 = {
                {s->first.mu + wt, s->first.pi + pt},
                {s->second.a, f, psolf.add(s->second.sol, t)}};
            LOG(info, s1);
            Weight mu_ = s1.first.mu;
            Profit pi_ = s1.first.pi;

            // Update LB
            if (mu_ <= c && pi_ > lb) {
                if (d.output.recursive_call_number == 1) {
                    std::stringstream ss;
                    ss << "it " << t - b << " (lb)";
                    output.update_lb(pi_, ss, info);
                    lb = pi_;
                }
                best_state = s1;
                last_item = t;
                if (best_state.first.pi == output.upper_bound)
                    goto end;
            }

            // Bounding
            Profit ub_local = 0;
            if (d.p.ub == 'b') {
                ub_local = (mu_ <= c)?
                    ub_dembo(ins, b, pi_, c-mu_):
                    ub_dembo_rev(ins, b, pi_, c-mu_);
            } else if (d.p.ub == 't') {
                ub_local = (mu_ <= c)?
                    ub_dembo(ins, t + 1, pi_, c-mu_):
                    ub_dembo_rev(ins, s->second.a - 1, pi_, c-mu_);
            }
            if (ub_local <= lb) {
                LOG(info, " ×" << std::endl);
                continue;
            }

            LOG(info, " ok" << std::endl);
            hint = d.map.insert(hint, s1);
            if (hint->second.a < s->second.a) {
                hint->second.a = s->second.a;
                hint->second.sol = psolf.add(s->second.sol, t);
            }
            hint--;
        }

        // Remove previously added items
        LOG(info, "remove" << std::endl);
        for (auto s = d.map.rbegin(); s != d.map.rend() && s->first.mu > c; ++s) {
            if (s->first.mu > c + wt)
                continue;
            LOG(info, *s << std::endl);

            balknap_update_bounds(d);
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
                    || best_state.first.pi == output.upper_bound)
                break;

            for (ItemPos j = s->second.a_prec; j < s->second.a; ++j) {
                LOG(info, "j " << j);
                Weight mu_ = s->first.mu - ins.item(j).w;
                Profit pi_ = s->first.pi - ins.item(j).p;
                std::pair<BalknapState, BalknapValue> s1 = {
                    {mu_, pi_},
                    {j, f, psolf.remove(s->second.sol, j)}};

                // Update LB
                if (mu_ <= c && pi_ > lb) {
                    if (d.output.recursive_call_number == 1) {
                        std::stringstream ss;
                        ss << "it " << t - b << " (lb)";
                        output.update_lb(pi_, ss, info);
                        lb = pi_;
                    }
                    best_state = s1;
                    last_item = t;
                    if (best_state.first.pi == output.upper_bound)
                        goto end;
                }

                // Bounding
                Profit ub_local = 0;
                if (d.p.ub == 'b') {
                    ub_local = (mu_ <= c)?
                        ub_dembo(ins, b, pi_, c-mu_):
                        ub_dembo_rev(ins, b, pi_, c-mu_);
                } else if (d.p.ub == 't') {
                    ub_local = (mu_ <= c)?
                        ub_dembo(ins, t + 1, pi_, c-mu_):
                        ub_dembo_rev(ins, j - 1, pi_, c-mu_);
                }
                if (ub_local <= lb) {
                    LOG(info, " ×" << std::endl);
                    continue;
                }

                LOG(info, " ok" << std::endl);
                auto res = d.map.insert(s1);
                if (!res.second) {
                    if (res.first->second.a < j) {
                        res.first->second.a = j;
                        res.first->second.sol = psolf.remove(s->second.sol, j);
                    }
                }
            }
            s->second.a_prec = s->second.a;
        }

    }
end:
    output.update_ub(output.lower_bound, std::stringstream("tree search completed"), p.info);

    if (p.set_end)
        *(p.end) = true;
    LOG(p.info, "end" << std::endl);
    for (std::thread& thread: d.threads)
        thread.join();
    d.threads.clear();
    LOG(p.info, "end2" << std::endl);

    if (output.lower_bound == output.solution.profit())
        return;

    LOG(info, "best_state " << best_state << std::endl);
    LOG(info, "partial sol " << std::bitset<64>(best_state.second.sol) << std::endl);

    // Reduce instance to items from best_state.second.a to last_item and remove
    // the items from the partial solution from the instance.
    // Then run the algorithm again.
    ins.set_first_item(best_state.second.a, info);
    ins.set_last_item(last_item);
    ins.fix(info, psolf.vector(best_state.second.sol));

    LOG_FOLD_END(info, "balknap_main");
    balknap_main(ins, p, output);
}

/******************************************************************************/

void balknap_update_bounds(BalknapInternalData& d)
{
    Instance& ins = d.ins;
    Info& info = d.p.info;

    if (d.p.surrelax >= 0 && d.p.surrelax <= (StateIdx)d.map.size()) {
        d.p.surrelax = -1;
        std::function<Output (Instance&, Info, bool*)> func
            = [&d](Instance& ins, Info info, bool* end)
            {
                BalknapOptionalParameters p;
                p.info = info;
                p.partial_solution_size = d.p.partial_solution_size;
                p.greedy = d.p.greedy;
                p.greedynlogn = d.p.greedynlogn;
                p.surrelax = -1;
                p.end = end;
                p.stop_if_end = true;
                p.set_end = false;
                return balknap(ins, p);
            };
        d.threads.push_back(std::thread(solvesurrelax, SurrelaxData{
                    .ins      = Instance::reset(ins),
                    .output   = d.output,
                    .func     = func,
                    .end      = d.p.end,
                    .info     = Info(info, true, "surrelax")}));
    }
    if (d.p.greedynlogn >= 0 && d.p.greedynlogn <= (StateIdx)d.map.size()) {
        d.p.greedynlogn = -1;
        auto gn_output = greedynlogn(d.ins);
        d.output.update_sol(gn_output.solution, std::stringstream("greedynlogn"), d.p.info);
    }
}

