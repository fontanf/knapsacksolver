#include "knapsack/opt_balknap/balknap.hpp"

#include "knapsack/lib/part_solution_1.hpp"
#include "knapsack/lb_greedy/greedy.hpp"
#include "knapsack/lb_greedynlogn/greedynlogn.hpp"
#include "knapsack/ub_dembo/dembo.hpp"
#include "knapsack/ub_dantzig/dantzig.hpp"
#include "knapsack/lib/update_bounds.hpp"

#include <map>
#include <bitset>

using namespace knapsack;

struct StatePart
{
    Weight mu;
    Profit pi;
    bool operator()(const StatePart& s1, const StatePart& s2)
    {
        if (s1.mu != s2.mu)
            return s1.mu < s2.mu;
        if (s1.pi != s2.pi)
            return s1.pi < s2.pi;
        return false;
    }
};

struct StateValuePart
{
    ItemPos a;
    ItemPos a_prec;
    PartSol1 sol;
};

std::ostream& operator<<(std::ostream& os, const std::pair<StatePart, StateValuePart>& s)
{
    os
        << "(" << s.first.mu
        << " " << s.first.pi
        << " " << s.second.a
        << " " << s.second.a_prec
        //<< " s " << std::bitset<16>(s.second.sol)
        << ")";
    return os;
}

Solution knapsack::sopt_balknap(Instance& ins, Info& info,
        BalknapParams params, ItemPos k, Profit o)
{
    if (o == -1)
        info.verbose("**** balknap (" + std::to_string(k) + ") ***\n");
    DBG(info.debug(
            "n " + std::to_string(ins.item_number()) + "/" + std::to_string(ins.total_item_number()) +
            " f " + std::to_string(ins.first_item()) +
            " l " + std::to_string(ins.last_item()) +
            " o " + std::to_string(o) +
            "\n");)

    Cpt lb_number = 0;
    Cpt ub_number = 0;
    Solution sol(ins);
    Profit lb = 0;
    Profit ub = -1;

    if (o != -1) {
        lb = o-1;
        ub = o;
    }

    // Sorting
    if (params.ub_type == 'b') {
        ins.sort_partially();
    } else if (params.ub_type == 't') {
        ins.sort();
    }
    // If all items fit in the knapsack then return break solution
    if (ins.break_item() == ins.last_item()+1) {
        if (ins.break_solution()->profit() > sol.profit())
            sol.update(*ins.break_solution(), info, lb_number, ub);
        return algorithm_end(sol, info);
    }

    // Compute initial lower bound
    info.verbose("Compute initial lower bound...\n");
    if (params.cpt_greedynlogn == 0) {
        params.cpt_greedynlogn = -1;
        Info info_tmp;
        Solution sol_tmp = sol_bestgreedynlogn(ins, info_tmp);
        if (sol_tmp.profit() > lb) {
            sol.update(sol_tmp, info, lb_number, ub);
            lb = sol.profit();
        }
    } else if (params.cpt_greedy == 0) {
        params.cpt_greedy = -1;
        Info info_tmp;
        Solution sol_tmp = sol_greedy(ins, info_tmp);
        if (sol_tmp.profit() > lb) {
            sol.update(sol_tmp, info, lb_number, ub);
            lb = sol.profit();
        }
    } else {
        Solution sol_tmp = *ins.break_solution();
        if (sol_tmp.profit() > lb) {
            sol.update(sol_tmp, info, lb_number, ub);
            lb = sol.profit();
        }
    }
    if (lb == ub)
        return algorithm_end(sol, info);

    // Variable reduction
    // If we already know the optimal value, we can use opt-1 as lower bound
    // for the reduction.
    if (params.ub_type == 'b') {
        ins.reduce1(lb, info);
        if (ins.capacity() < 0)
            return algorithm_end(sol, info);
    } else if (params.ub_type == 't') {
        ins.reduce2(lb, info);
        if (ins.capacity() < 0)
            return algorithm_end(sol, info);
    }
    Weight  c = ins.total_capacity();
    ItemPos f = ins.first_item();
    ItemPos l = ins.last_item();
    ItemPos n = ins.item_number();

    // Trivial cases
    // Return the best solution between the solution of the trivial instance and
    // the solution used for the reduction.
    if (n == 0 || c == 0) {
        if (ins.reduced_solution()->profit() > sol.profit())
            sol.update(*ins.reduced_solution(), info, lb_number, ub);
        return algorithm_end(sol, info);
    } else if (n == 1) {
        Solution sol_tmp = *ins.reduced_solution();
        sol_tmp.set(f, true);
        if (sol_tmp.profit() > sol.profit())
            sol.update(sol_tmp, info, lb_number, ub);
        return algorithm_end(sol, info);
    } else if (ins.break_item() == ins.last_item()+1) {
        if (ins.break_solution()->profit() > sol.profit())
            sol.update(*ins.break_solution(), info, lb_number, ub);
        return algorithm_end(sol, info);
    }

    ItemPos b    = ins.break_item();
    Weight w_bar = ins.break_solution()->weight();
    Profit p_bar = ins.break_solution()->profit();

    Info info_tmp;
    Profit ub_tmp = ub_dantzig(ins, info_tmp);
    if (ub == -1 || ub_tmp < ub)
        Solution::update_ub(ub, ub_tmp, info, ub_number, lb);

    DBG(info.debug("Break solution: " + ins.break_solution()->to_string_binary() + "\n");)
    DBG(info.debug("Break solution: " + ins.break_solution()->to_string_items() + "\n");)
    DBG(info.debug(
                STR1(n) + STR2(c) + STR2(f) + STR2(l) + "\n"
                + STR1(w_bar) + STR2(p_bar) + "\n");)

    if (ins.break_solution()->profit() > lb)
        sol.update(*ins.break_solution(), info, lb_number, ub);

    if (lb == ub) // If UB == LB, then stop
        return algorithm_end(sol, info);

    info.verbose("Recursion...\n");

    // Create memory table
    std::map<StatePart, StateValuePart, StatePart> map;

    // Initialization
    // Create first partial solution centered on the break item.
    PartSolFactory1 psolf(ins, k, b, f, l);
    PartSol1 psol_init = 0;
    for (ItemPos j=f; j<b; ++j)
        psol_init = psolf.add(psol_init, j);
    // s(w_bar,p_bar) = b
    map.insert({{w_bar,p_bar},{b,f,psol_init}});

    // Best state. Note that it is not a pointer
    std::pair<StatePart, StateValuePart> best_state = {map.begin()->first, map.begin()->second};
    // Also keep last added item to improve the variable reduction at the end.
    ItemPos last_item = b-1;

    auto func = [&params, &k](Instance& ins) -> Solution {
        Info info_tmp;
        return sopt_balknap(ins, info_tmp, params, k);
    };
    UpdateBoundsSolData bounds_data(ins, info, sol, lb, ub, lb_number, ub_number);
    bounds_data.cpt_greedynlogn = params.cpt_greedynlogn;
    bounds_data.cpt_ubsur       = params.cpt_surrogate;
    bounds_data.cpt_solvesur    = params.cpt_solve_sur;


    // Recursion
    for (ItemPos t=b; t<=l; ++t) {
        Weight wt = ins.item(t).w;
        Profit pt = ins.item(t).p;

        bounds_data.run(func, map.size());
        if (lb == ub)
            goto end;

        // Bounding
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
                map.erase(s++);
            } else {
                if (ub_t < ub_local)
                    ub_t = ub_local;
                s++;
            }
        }
        if (ub_t < ub) {
            Solution::update_ub(ub, ub_t, info, ub_number, lb);
            if (lb == ub)
                goto end;
        }

        // If there is no more states, the stop
        if (map.size() == 0)
            break;

        // Add item t
        auto s = map.upper_bound({c+1,0});
        auto hint = s;
        hint--;
        while (s != map.begin() && (--s)->first.mu <= c) {
            std::pair<StatePart, StateValuePart> s1 = {
                {s->first.mu + wt, s->first.pi + pt},
                {s->second.a, f, psolf.add(s->second.sol, t)}};
            Weight mu_ = s1.first.mu;
            Profit pi_ = s1.first.pi;

            // Update LB
            if (mu_ <= c && pi_ > lb) {
                Solution::update_lb(lb, pi_, info, lb_number, ub);
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
            if (ub_local <= lb)
                continue;

            hint = map.insert(hint, s1);
            if (hint->second.a < s->second.a) {
                hint->second.a    = s->second.a;
                hint->second.sol  = psolf.add(s->second.sol, t);
            }
            hint--;
        }

        // Remove previously added items
        for (auto s = map.rbegin(); s != map.rend() && s->first.mu > c; ++s) {
            if (s->first.mu > c + wt)
                continue;

            for (ItemPos j = s->second.a_prec; j < s->second.a; ++j) {
                Weight mu_ = s->first.mu - ins.item(j).w;
                Profit pi_ = s->first.pi - ins.item(j).p;
                std::pair<StatePart, StateValuePart> s1 = {
                    {mu_, pi_},
                    {j, f, psolf.remove(s->second.sol, j)}};

                // Update LB
                if (mu_ <= c && pi_ > lb) {
                    Solution::update_lb(lb, pi_, info, lb_number, ub);
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
                if (ub_local <= lb)
                    continue;

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
    if (best_state.first.pi < sol.profit())
        return algorithm_end(sol, info);

    // Reduce instance to items from best_state.second.a to last_item and remove
    // the items from the partial solution from the instance.
    // Then run the algorithm again.
    ins.set_first_item(best_state.second.a);
    ins.set_last_item(last_item);
    ins.fix(psolf.vector(best_state.second.sol));

    Info info_tmp2;
    sol = knapsack::sopt_balknap(ins, info_tmp2,  params, k, best_state.first.pi);
    return algorithm_end(sol, info);
}

