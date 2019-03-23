#include "knapsack/opt_balknap/balknap.hpp"

#include "knapsack/lib/part_solution_1.hpp"
#include "knapsack/lb_greedy/greedy.hpp"
#include "knapsack/lb_greedynlogn/greedynlogn.hpp"
#include "knapsack/ub_dembo/dembo.hpp"
#include "knapsack/ub_dantzig/dantzig.hpp"

#include <map>
#include <bitset>

using namespace knapsack;

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

Solution knapsack::sopt_balknap(Instance& ins,
        BalknapParams params, Profit o, Info info)
{
    LOG_FOLD_START(info, "balknap k " << params.k << std::endl);
    if (o == -1)
        VER(info, "**** balknap (" << params.k << ") ***" << std::endl;);

    Solution sol(ins);
    if (ins.item_number() == 0 || ins.capacity() == 0) {
        LOG_FOLD_END(info, "no item or null capacity");
        if (ins.reduced_solution() != NULL && ins.reduced_solution()->profit() > sol.profit()) {
            return algorithm_end(*ins.reduced_solution(), info);
        } else {
            return algorithm_end(sol, info);
        }
    }

    Profit lb = 0;
    Profit ub = -1;
    if (o != -1) {
        lb = o-1;
        ub = o;
    }

    // Sorting
    if (params.ub_type == 'b') {
        ins.sort_partially(info);
    } else if (params.ub_type == 't') {
        ins.sort(info);
    }
    // If all items fit in the knapsack then return break solution
    if (ins.break_item() == ins.last_item()+1) {
        if (ins.break_solution()->profit() > sol.profit())
            sol = *ins.break_solution();
        LOG_FOLD_END(info, "all items fit in the knapsack");
        return algorithm_end(sol, info);
    }

    // Compute initial lower bound
    VER(info, "Compute initial lower bound..." << std::endl);
    Solution sol_tmp = *ins.break_solution();
    if (params.cpt_greedynlogn == 0) {
        params.cpt_greedynlogn = -1;
        Solution sol_tmp = sol_greedynlogn(ins);
    } else if (params.cpt_greedy == 0) {
        params.cpt_greedy = -1;
        Solution sol_tmp = sol_greedy(ins);
    }
    if (sol_tmp.profit() > lb) {
        sol = sol_tmp;
        lb = sol.profit();
    }
    LOG(info, "lb " << lb << std::endl);
    if (lb == ub) {
        LOG_FOLD_END(info, "lb is optimal");
        return algorithm_end(sol, info);
    }

    // Variable reduction
    // If we already know the optimal value, we can use opt-1 as lower bound
    // for the reduction.
    if (params.ub_type == 'b') {
        ins.reduce1(lb, info);
        if (ins.capacity() < 0) {
            LOG_FOLD_END(info, "negative capacity");
            return algorithm_end(sol, info);
        }
    } else if (params.ub_type == 't') {
        ins.reduce2(lb, info);
        if (ins.capacity() < 0) {
            LOG_FOLD_END(info, "negative capacity");
            return algorithm_end(sol, info);
        }
    }
    Weight  c = ins.total_capacity();
    ItemPos f = ins.first_item();
    ItemPos l = ins.last_item();
    ItemPos n = ins.item_number();

    // Trivial cases
    if (n == 0 || ins.capacity() == 0) {
        LOG_FOLD_END(info, "no item or null capacity");
        if (ins.reduced_solution()->profit() > sol.profit()) {
            return algorithm_end(*ins.reduced_solution(), info);
        } else {
            return algorithm_end(sol, info);
        }
    } else if (n == 1) {
        Solution sol_tmp = *ins.reduced_solution();
        sol_tmp.set(f, true);
        LOG_FOLD_END(info, "1 item");
        if (sol_tmp.profit() > sol.profit()) {
            return algorithm_end(sol_tmp, info);
        } else {
            return algorithm_end(sol, info);
        }
    } else if (ins.break_item() == ins.last_item()+1) {
        LOG_FOLD_END(info, "all items fit in the knapsack");
        if (ins.break_solution()->profit() > sol.profit()) {
            return algorithm_end(*ins.break_solution(), info);
        } else {
            return algorithm_end(sol, info);
        }
    }

    ItemPos b    = ins.break_item();
    Weight w_bar = ins.break_solution()->weight();
    Profit p_bar = ins.break_solution()->profit();

    Profit ub_tmp = ub_dantzig(ins);
    if (ub == -1 || ub_tmp < ub)
        ub = ub_tmp;
    LOG(info, "ub " << ub << std::endl);

    LOG(info, ins);
    if (ins.break_solution()->profit() > lb) {
        sol = *ins.break_solution();
        lb = sol.profit();
        LOG(info, "lb " << lb << std::endl);
    }

    if (lb == ub) { // If UB == LB, then stop
        LOG_FOLD_END(info, "lb is optimal");
        return algorithm_end(sol, info);
    }

    VER(info, "Recursion..." << std::endl);

    // Create memory table
    std::map<BalknapState, BalknapValue, BalknapState> map;

    // Initialization
    // Create first partial solution centered on the break item.
    PartSolFactory1 psolf(ins, params.k, b, f, l);
    PartSol1 psol_init = 0;
    for (ItemPos j=f; j<b; ++j)
        psol_init = psolf.add(psol_init, j);
    // s(w_bar,p_bar) = b
    map.insert({{w_bar,p_bar},{b,f,psol_init}});

    // Best state. Note that it is not a pointer
    std::pair<BalknapState, BalknapValue> best_state = {map.begin()->first, map.begin()->second};
    // Also keep last added item to improve the variable reduction at the end.
    ItemPos last_item = b-1;

    // Recursion
    for (ItemPos t=b; t<=l; ++t) {
        LOG(info, "t " << t << " (" << ins.item(t) << ")" << std::endl);
        Weight wt = ins.item(t).w;
        Profit pt = ins.item(t).p;

        //bounds_data.run(func, map.size());
        if (lb == ub)
            goto end;

        // Bounding
        LOG(info, "bound" << std::endl);
        Profit ub_t = 0;
        for (auto s = map.begin(); s != map.end() && s->first.mu <= c;) {
            Profit pi = s->first.pi;
            Weight mu = s->first.mu;
            Profit ub_local = 0;
            if (params.ub_type == 'b') {
                ub_local = (mu <= c)?
                    ub_dembo(ins, b, pi, c-mu):
                    ub_dembo_rev(ins, b, pi, c-mu);
            } else if (params.ub_type == 't') {
                ub_local = (mu <= c)?
                    ub_dembo(ins, t, pi, c-mu):
                    ub_dembo_rev(ins, s->second.a, pi, c-mu);
            }
            if (ub_local < lb) {
                LOG(info, "remove " << *s << std::endl);
                map.erase(s++);
            } else {
                if (ub_t < ub_local)
                    ub_t = ub_local;
                s++;
            }
        }
        if (ub_t < ub) {
            ub = ub_t;
            if (lb == ub)
                goto end;
        }

        // If there is no more states, the stop
        if (map.size() == 0)
            break;

        // Add item t
        LOG(info, "add" << std::endl);
        auto s = map.upper_bound({c+1,0});
        auto hint = s;
        hint--;
        while (s != map.begin() && (--s)->first.mu <= c) {
            std::pair<BalknapState, BalknapValue> s1 = {
                {s->first.mu + wt, s->first.pi + pt},
                {s->second.a, f, psolf.add(s->second.sol, t)}};
            LOG(info, s1);
            Weight mu_ = s1.first.mu;
            Profit pi_ = s1.first.pi;

            // Update LB
            if (mu_ <= c && pi_ > lb) {
                lb = pi_;
                best_state = s1;
                last_item = t;
                if (lb == ub)
                    goto end;
            }

            // Bounding
            Profit ub_local = 0;
            if (params.ub_type == 'b') {
                ub_local = (mu_ <= c)?
                    ub_dembo(ins, b, pi_, c-mu_):
                    ub_dembo_rev(ins, b, pi_, c-mu_);
            } else if (params.ub_type == 't') {
                ub_local = (mu_ <= c)?
                    ub_dembo(ins, t+1, pi_, c-mu_):
                    ub_dembo_rev(ins, s->second.a-1, pi_, c-mu_);
            }
            if (ub_local <= lb) {
                LOG(info, " ×" << std::endl);
                continue;
            }

            LOG(info, " ok" << std::endl);
            hint = map.insert(hint, s1);
            if (hint->second.a < s->second.a) {
                hint->second.a    = s->second.a;
                hint->second.sol  = psolf.add(s->second.sol, t);
            }
            hint--;
        }

        // Remove previously added items
        LOG(info, "remove" << std::endl);
        for (auto s = map.rbegin(); s != map.rend() && s->first.mu > c; ++s) {
            if (s->first.mu > c + wt)
                continue;
            LOG(info, *s << std::endl);

            for (ItemPos j = s->second.a_prec; j < s->second.a; ++j) {
                LOG(info, "j " << j);
                Weight mu_ = s->first.mu - ins.item(j).w;
                Profit pi_ = s->first.pi - ins.item(j).p;
                std::pair<BalknapState, BalknapValue> s1 = {
                    {mu_, pi_},
                    {j, f, psolf.remove(s->second.sol, j)}};

                // Update LB
                if (mu_ <= c && pi_ > lb) {
                    lb = pi_;
                    best_state = s1;
                    last_item = t;
                    if (lb == ub)
                        goto end;
                }

                // Bounding
                Profit ub_local = 0;
                if (params.ub_type == 'b') {
                    ub_local = (mu_ <= c)?
                        ub_dembo(ins, b, pi_, c-mu_):
                        ub_dembo_rev(ins, b, pi_, c-mu_);
                } else if (params.ub_type == 't') {
                    ub_local = (mu_ <= c)?
                        ub_dembo(ins, t+1, pi_, c-mu_):
                        ub_dembo_rev(ins, j-1, pi_, c-mu_);
                }
                if (ub_local <= lb) {
                    LOG(info, " ×" << std::endl);
                    continue;
                }

                LOG(info, " ok" << std::endl);
                auto res = map.insert(s1);
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

    // If we didn't improve sol, then it means that sol was optimal
    if (best_state.first.pi < sol.profit()) {
        LOG_FOLD_END(info, "sol is optimal");
        return algorithm_end(sol, info);
    }
    LOG(info, "best_state " << best_state << std::endl);
    LOG(info, "partial sol " << std::bitset<64>(best_state.second.sol) << std::endl);

    // Reduce instance to items from best_state.second.a to last_item and remove
    // the items from the partial solution from the instance.
    // Then run the algorithm again.
    ins.set_first_item(best_state.second.a, info);
    ins.set_last_item(last_item);
    ins.fix(info, psolf.vector(best_state.second.sol));

    sol = knapsack::sopt_balknap(ins, params, best_state.first.pi);
    LOG_FOLD_END(info, "balknap");
    return algorithm_end(sol, info);
}

