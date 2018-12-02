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

struct State
{
    Weight mu;
    Profit pi;
    bool operator()(const State& s1, const State& s2)
    {
        if (s1.mu != s2.mu)
            return s1.mu < s2.mu;
        if (s1.pi != s2.pi)
            return s1.pi < s2.pi;
        return false;
    }
};

struct StateValue
{
    ItemPos a;
    ItemPos a_prec;
};

std::string to_string(const std::pair<State, StateValue>& s)
{
    return
        "(mu " + std::to_string(s.first.mu) +
        " pi " + std::to_string(s.first.pi) +
        " a " +  std::to_string(s.second.a) +
        " ap " + std::to_string(s.second.a_prec) +
        ")";
}

Profit knapsack::opt_balknap_list(Instance& ins, Info& info,
        BalknapParams params)
{
    info.verbose("*** balknap (list) ***\n");

    DBG(info.debug("Sort items...\n");)
    if (params.upper_bound == "b") {
        ins.sort_partially();
    } else if (params.upper_bound == "t") {
        ins.sort();
    } else {
        assert(false);
    }
    if (ins.break_item() == ins.last_item()+1) { // all items are in the break solution
        DBG(info.debug("All items fit in the knapsack.\n");)
        return algorithm_end(ins.break_profit(), info);
    }

    // Compute lower bound
    info.verbose("Compute lower bound...");
    Profit lb = 0;
    if (params.lb_greedynlogn == 0) {
        Info info_tmp;
        lb = sol_bestgreedynlogn(ins, info_tmp).profit();
    } else if (params.lb_greedy == 0) {
        Info info_tmp;
        lb = sol_greedy(ins, info_tmp).profit();
    } else {
        lb = ins.break_profit();
    }
    info.verbose(" " + std::to_string(lb) + "\n");

    // Variable reduction
    if (params.upper_bound == "b") {
        ins.reduce1(lb, info);
    } else if (params.upper_bound == "t") {
        ins.reduce2(lb, info);
    } else {
        assert(false);
    }
    if (ins.capacity() < 0) {
        DBG(info.debug("All items have been reduced.\n");)
        return algorithm_end(lb, info);
    }
    DBG(info.debug("Reduced solution: " + ins.reduced_solution()->to_string_binary() + "\n");)
    DBG(info.debug("Reduced solution: " + ins.reduced_solution()->to_string_items() + "\n");)

    Weight  c = ins.total_capacity();
    ItemPos f = ins.first_item();
    ItemPos l = ins.last_item();
    ItemPos n = ins.item_number();
    Profit p0 = ins.reduced_solution()->profit();

    // Trivial cases
    if (n == 0 || c == 0) {
        DBG(info.debug("Empty instance (after reduction).\n");)
        return algorithm_end(std::max(lb, p0), info);
    } else if (n == 1) {
        DBG(info.debug("Instance only contains one item (after reduction).\n");)
        return algorithm_end(std::max(lb, p0 + ins.item(f).p), info);
    } else if (ins.break_item() == ins.last_item()+1) {
        DBG(info.debug("All items fit in the knapsack (after reduction).\n");)
        return algorithm_end(std::max(lb, ins.break_solution()->profit()), info);
    }

    ItemPos b    = ins.break_item();
    Weight w_bar = ins.break_solution()->weight();
    Profit p_bar = ins.break_solution()->profit();
    Info info_tmp;
    Profit u = ub_dantzig(ins, info_tmp);

    DBG(info.debug("Break solution: " + ins.break_solution()->to_string_binary() + "\n");)
    DBG(info.debug("Break solution: " + ins.break_solution()->to_string_items() + "\n");)
    DBG(info.debug(STR1(n) + STR2(c) + STR2(f) + STR2(l) + "\n"
                + STR1(w_bar) + STR2(p_bar) + "\n");)

    if (ins.break_solution()->profit() > lb) {
        DBG(info.debug("Update best solution.");)
        lb = ins.break_solution()->profit();
    }

    info.verbose(STR1(lb) + STR2(u) + STR4(gap, u-lb) + "\n");

    // If UB == LB, then stop
    if (lb >= u) {
        DBG(info.debug("Lower bound equals upper bound.");)
        return algorithm_end(lb, info);
    }

    // Create memory table
    std::map<State, StateValue, State> map;

    // Initialization
    map.insert({{w_bar,p_bar},{b,f}}); // s(w_bar,p_bar) = b

    // Recursion
    info.verbose("Recursion...\n");
    for (ItemPos t=b; t<=l; ++t) {
        DBG(
            info.debug("Map: " + std::to_string(map.size()) + "\n");
            for (auto s = map.begin(); s != map.end(); ++s)
                info.debug(to_string(*s) + "\n");
            info.debug(
                    "t " + std::to_string(t) +
                    " (" + std::to_string(ins.item(t).j) + ")\n"
                    );
        )

        Weight wt = ins.item(t).w;
        Profit pt = ins.item(t).p;

        // Bounding
        if (params.upper_bound == "t") {
            DBG(info.debug("Bounding...");)
            for (auto s = map.begin(); s != map.end();) {
                Profit pi = s->first.pi;
                Weight mu = s->first.mu;
                Profit ub = (mu <= c)?
                    ub_dembo(ins, t, pi, c-mu):
                    ub_dembo_rev(ins, s->second.a, pi, c-mu);
                if (ub < lb) {
                    map.erase(s++);
                } else {
                    s++;
                }
            }
        }
        if (map.size() == 0)
            break;

        // Add item t
        DBG(info.debug("Add...\n");)
        auto s = map.upper_bound({c+1,0});
        auto hint = s;
        hint--;
        while (s != map.begin() && (--s)->first.mu <= c) {
            DBG(info.debug(" + State " + to_string(*s));)
            Weight mu_ = s->first.mu + wt;
            Weight pi_ = s->first.pi + pt;

            // Update LB
            if (mu_ <= c && pi_ > lb) {
                lb = pi_;
            }

            // Bounding
            Profit ub = 0;
            if (params.upper_bound == "b") {
                ub = (mu_ <= c)?
                    ub_dembo(ins, b, pi_, c-mu_):
                    ub_dembo_rev(ins, b, pi_, c-mu_);
            } else if (params.upper_bound == "t") {
                ub = (mu_ <= c)?
                    ub_dembo(ins, t+1, pi_, c-mu_):
                    ub_dembo_rev(ins, s->second.a-1, pi_, c-mu_);
            } else {
                    assert(false);
            }
            DBG(info.debug(
                    " lb " + std::to_string(lb) +
                    " ubtmp " + std::to_string(ub) +
                    " ub " + std::to_string(ub));)
            if (ub <= lb) {
                DBG(info.debug(" ×\n");)
                continue;
            } else {
                DBG(info.debug(" ok\n");)
            }

            hint = map.insert(hint, {{mu_, pi_}, {s->second.a, f}});
            if (hint->second.a < s->second.a)
                hint->second.a    = s->second.a;
            hint--;
        }

        // Remove previously added items
        DBG(info.debug("Remove...\n");)
        for (auto s = map.rbegin(); s != map.rend() && s->first.mu > c; ++s) {
            if (s->first.mu > c + wt)
                continue;
            DBG(info.debug(" - State " + to_string(*s));)

            for (ItemPos j = s->second.a_prec; j < s->second.a; ++j) {
                DBG(info.debug("  j " + std::to_string(j) +
                        "(" + std::to_string(ins.item(j).j) + ")");)
                Weight mu_ = s->first.mu - ins.item(j).w;
                Profit pi_ = s->first.pi - ins.item(j).p;
                DBG(info.debug(" " + std::to_string(mu_) + " " + std::to_string(pi_));)

                // Update LB
                if (mu_ <= c && pi_ > lb) {
                    lb = pi_;
                }

                // Bounding
                Profit ub = 0;
                if (params.upper_bound == "b") {
                    ub = (mu_ <= c)?
                        ub_dembo(ins, b, pi_, c-mu_):
                        ub_dembo_rev(ins, b, pi_, c-mu_);
                } else if (params.upper_bound == "t") {
                    ub = (mu_ <= c)?
                        ub_dembo(ins, t+1, pi_, c-mu_):
                        ub_dembo_rev(ins, j-1, pi_, c-mu_);
                } else {
                    assert(false);
                }
                DBG(info.debug(
                        " lb " + std::to_string(lb) +
                        " ubtmp " + std::to_string(ub) +
                        " ub " + std::to_string(ub));)
                if (ub <= lb) {
                    DBG(info.debug(" ×");)
                    continue;
                } else {
                    DBG(info.debug(" ok");)
                }

                auto res = map.insert({{mu_,pi_},{j, f}});
                if (!res.second)
                    if (res.first->second.a < j)
                        res.first->second.a = j;
            }
            s->second.a_prec = s->second.a;
            DBG(info.debug("\n");)
        }
    }

    DBG(
        info.debug("Map: " + std::to_string(map.size()) + "\n");
        for (auto s = map.begin(); s != map.end(); ++s)
            info.debug(to_string(*s) + "\n");
    )

    return algorithm_end(lb, info);
}

/******************************************************************************/

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

Solution knapsack::sopt_balknap_list_part(Instance& ins, Info& info,
        BalknapParams params, ItemPos k, Profit o)
{
    if (o == -1)
        info.verbose("**** balknap (list, part " + std::to_string(k) + ") ***\n");
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
    if (params.upper_bound == "b") {
        ins.sort_partially();
    } else if (params.upper_bound == "t") {
        ins.sort();
    } else {
        assert(false);
    }
    // If all items fit in the knapsack then return break solution
    if (ins.break_item() == ins.last_item()+1) {
        if (ins.break_solution()->profit() > sol.profit())
            sol.update(*ins.break_solution(), info, lb_number, ub);
        return algorithm_end(sol, info);
    }

    // Compute initial lower bound
    info.verbose("Compute initial lower bound...\n");
    if (params.lb_greedynlogn == 0) {
        params.lb_greedynlogn = -1;
        Info info_tmp;
        Solution sol_tmp = sol_bestgreedynlogn(ins, info_tmp);
        if (sol_tmp.profit() > lb) {
            sol.update(sol_tmp, info, lb_number, ub);
            lb = sol.profit();
        }
    } else if (params.lb_greedy == 0) {
        params.lb_greedy = -1;
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
    if (params.upper_bound == "b") {
        ins.reduce1(lb, info);
        if (ins.capacity() < 0)
            return algorithm_end(sol, info);
    } else if (params.upper_bound == "t") {
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
        return sopt_balknap_list_part(ins, info_tmp, params, k);
    };
    UpdateBoundsSolData bounds_data(ins, info, sol, lb, ub, lb_number, ub_number);
    bounds_data.cpt_greedynlogn = params.lb_greedynlogn;
    bounds_data.cpt_ubsur       = params.ub_surrogate;
    bounds_data.cpt_solvesur    = params.solve_sur;


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
            if (params.upper_bound == "b") {
                ub_local = (mu <= c)?
                    ub_dembo(ins, b, pi, c-mu):
                    ub_dembo_rev(ins, b, pi, c-mu);
            } else if (params.upper_bound == "t") {
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
            if (params.upper_bound == "b") {
                ub_local = (mu_ <= c)?
                    ub_dembo(ins, b, pi_, c-mu_):
                    ub_dembo_rev(ins, b, pi_, c-mu_);
            } else if (params.upper_bound == "t") {
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
                if (params.upper_bound == "b") {
                    ub_local = (mu_ <= c)?
                        ub_dembo(ins, b, pi_, c-mu_):
                        ub_dembo_rev(ins, b, pi_, c-mu_);
                } else if (params.upper_bound == "t") {
                    ub_local = (mu_ <= c)?
                        ub_dembo(ins, t+1, pi_, c-mu_):
                        ub_dembo_rev(ins, j-1, pi_, c-mu_);
                } else {
                    assert(false);
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
    sol = knapsack::sopt_balknap_list_part(ins, info_tmp2,  params, k, best_state.first.pi);
    return algorithm_end(sol, info);
}

/******************************************************************************/

struct StateAll
{
    Weight mu;
    Profit pi;
    ItemPos b;
    bool operator()(const StateAll& s1, const StateAll& s2)
    {
        if (s1.mu != s2.mu)
            return s1.mu < s2.mu;
        if (s1.pi != s2.pi)
            return s1.pi < s2.pi;
        return false;
    }
};

struct StateValueAll
{
    ItemPos a;
    ItemPos a_prec; // 'a' from state '(mu,pi,a,b-1)'
    std::map<StateAll, StateValueAll, StateAll>::iterator pred;
};

std::string to_string(const std::pair<StateAll, StateValueAll>& s)
{
    auto pred = s.second.pred;
    return
        "(mu " + std::to_string(s.first.mu) +
        " pi " + std::to_string(s.first.pi) +
        " a " +  std::to_string(s.second.a) +
        " ap " + std::to_string(s.second.a_prec) +
        " [mu" + std::to_string(pred->first.mu) +
        " pi " + std::to_string(pred->first.pi) +
        " a " + std::to_string(pred->second.a) +
        " b " + std::to_string(pred->first.b) +
        "]" + ")";
}

Solution knapsack::sopt_balknap_list_all(Instance& ins, Info& info,
        BalknapParams params)
{
    info.verbose("*** balknap (list, all) ***\n");

    DBG(info.debug("Sort items...\n");)
    if (params.upper_bound == "b") {
        ins.sort_partially();
    } else if (params.upper_bound == "t") {
        ins.sort();
    } else {
        assert(false);
    }
    if (ins.break_item() == ins.last_item()+1) { // all items are in the break solution
        DBG(info.debug("All items fit in the knapsack.\n");)
        Solution sol = *ins.break_solution();
        return algorithm_end(sol, info);
    }

    info.verbose("Compute lower bound...");
    Solution sol(ins);
    if (params.lb_greedynlogn == 0) {
        Info info_tmp;
        sol = sol_bestgreedynlogn(ins, info_tmp);
    } else if (params.lb_greedy == 0) {
        Info info_tmp;
        sol = sol_greedy(ins, info_tmp);
    } else {
        sol = *ins.break_solution();
    }
    info.verbose(" " + std::to_string(sol.profit()) + "\n");

    // Variable reduction
    if (params.upper_bound == "b") {
        ins.reduce1(sol.profit(), info);
    } else if (params.upper_bound == "t") {
        ins.reduce2(sol.profit(), info);
    } else {
        assert(false);
    }
    if (ins.capacity() < 0) {
        // If the capacity is negative, then it means that sol was the optimal
        // solution. Note that this is not possible if opt-1 has been used as
        // lower bound for the reduction.
        DBG(info.debug("All items have been reduced.\n");)
        return algorithm_end(sol, info);
    }
    DBG(info.debug("Reduced solution: " + ins.reduced_solution()->to_string_binary() + "\n");)
    DBG(info.debug("Reduced solution: " + ins.reduced_solution()->to_string_items() + "\n");)

    Weight  c = ins.total_capacity();
    ItemPos f = ins.first_item();
    ItemPos l = ins.last_item();
    ItemPos n = ins.item_number();

    // Trivial cases
    if (n == 0 || c == 0) {
        DBG(info.debug("Empty instance (after reduction).\n");)
        if (ins.reduced_solution()->profit() > sol.profit())
            sol = *ins.reduced_solution();
        return algorithm_end(sol, info);
    } else if (n == 1) {
        DBG(info.debug("Instance only contains one item (after reduction).\n");)
        Solution sol1 = *ins.reduced_solution();
        sol1.set(f, true);
        if (sol1.profit() > sol.profit())
            sol = sol1;
        return algorithm_end(sol, info);
    } else if (ins.break_item() == ins.last_item()+1) {
        DBG(info.debug("All items fit in the knapsack (after reduction).\n");)
        if (ins.break_solution()->profit() > sol.profit())
            sol = *ins.break_solution();
        return algorithm_end(sol, info);
    }

    ItemPos b    = ins.break_item();
    Weight w_bar = ins.break_solution()->weight();
    Profit p_bar = ins.break_solution()->profit();
    Info info_tmp;
    Profit u = ub_dantzig(ins, info_tmp);

    DBG(info.debug("Break solution: " + ins.break_solution()->to_string_binary() + "\n");)
    DBG(info.debug("Break solution: " + ins.break_solution()->to_string_items() + "\n");)
    DBG(info.debug(STR1(n) + STR2(c) + STR2(f) + STR2(l) + "\n" +
                STR1(w_bar) + STR1(p_bar) + "\n");)

    if (ins.break_solution()->profit() > sol.profit()) {
        DBG(info.debug("Update best solution.");)
        sol = *ins.break_solution();
    }

    Profit lb = sol.profit();
    info.verbose(
            "lb " + std::to_string(sol.profit()) +
            " ub " + std::to_string(u) +
            " gap " + std::to_string(u - sol.profit()) +
            "\n");

    // If UB == LB, then stop
    if (sol.profit() == u) {
        DBG(info.debug("Lower bound equals upper bound.");)
        return algorithm_end(sol, info);
    }

    // Create memory table
    std::vector<std::map<StateAll, StateValueAll, StateAll>> maps(l-b+2);
    maps[0] = std::map<StateAll, StateValueAll, StateAll>();

    // Initialization
    maps[0].insert({{w_bar,p_bar,b-1},{b,f,maps[0].end()}}); // s(w_bar,p_bar) = b
    maps[0].begin()->second.pred = maps[0].begin();
    auto best_state = maps[0].begin();

    // Recursion
    info.verbose("Recursion...\n");
    for (ItemPos t=b; t<=l; ++t) {
        ItemPos k = t - b + 1;
        DBG(
            info.debug("Map: " + std::to_string(maps[k-1].size()) + "\n");
            for (auto s = maps[k-1].begin(); s != maps[k-1].end(); ++s)
                info.debug(to_string(*s) + "\n");
            info.debug(
                    "t " + std::to_string(t) +
                    " (" + std::to_string(ins.item(t).j) + ")\n"
                    );
        )

        Weight wt = ins.item(t).w;
        Profit pt = ins.item(t).p;

        maps[k] = std::map<StateAll, StateValueAll, StateAll>();
        for (auto s = maps[k-1].begin(); s != maps[k-1].end(); ++s)
            maps[k].insert({
                    {s->first.mu, s->first.pi, t},
                    {s->second.a, s->second.a, s}});

        // Bounding
        if (params.upper_bound == "t") {
            DBG(info.debug("Bounding...");)
            for (auto s = maps[k].begin(); s != maps[k].end();) {
                Profit pi = s->first.pi;
                Weight mu = s->first.mu;
                Profit ub = (mu <= c)?
                    ub_dembo(ins, t, pi, c-mu):
                    ub_dembo_rev(ins, s->second.a, pi, c-mu);
                if (ub < lb) {
                    maps[k].erase(s++);
                } else {
                    s++;
                }
            }
        }
        if (maps[k].size() == 0)
            break;

        // Add item t
        DBG(info.debug("Add...\n");)
        auto s = maps[k].upper_bound({c+1,0,0});
        auto hint = s;
        if (s != maps[k].begin())
            hint--;
        while (s != maps[k].begin() && (--s)->first.mu <= c) {
            DBG(info.debug(" + State " + to_string(*s));)
            Weight mu_ = s->first.mu + wt;
            Weight pi_ = s->first.pi + pt;

            // Bounding
            Profit ub = 0;
            if (params.upper_bound == "b") {
                ub = (mu_ <= c)?
                    ub_dembo(ins, b, pi_, c-mu_):
                    ub_dembo_rev(ins, b, pi_, c-mu_);
            } else if (params.upper_bound == "t") {
                ub = (mu_ <= c)?
                    ub_dembo(ins, t+1, pi_, c-mu_):
                    ub_dembo_rev(ins, s->second.a-1, pi_, c-mu_);
            } else {
                    assert(false);
            }
            DBG(info.debug(
                    " lb " + std::to_string(lb) +
                    " ubtmp " + std::to_string(ub) +
                    " ub " + std::to_string(ub));)
            if (ub <= lb) {
                DBG(info.debug(" ×\n");)
                continue;
            } else {
                DBG(info.debug(" ok\n");)
            }

            hint = maps[k].insert(hint, {{mu_, pi_, t}, {s->second.a, f, s->second.pred}});
            // If (mu_,pi_,t) existed but should be updated
            if (hint->second.a < s->second.a) {
                hint->second.a    = s->second.a;
                hint->second.pred = s->second.pred;
            }

            // Update LB
            if (mu_ <= c && pi_ > lb) {
                lb = pi_;
                best_state = hint;
            }
            hint--;
        }

        // Remove previously added items
        DBG(info.debug("Remove...\n");)
        for (auto s = --(maps[k].end()); s->first.mu > c; --s) {
            if (s->first.mu > c + wt)
                continue;
            DBG(info.debug(" - State " + to_string(*s));)

            for (ItemPos j = s->second.a_prec; j < s->second.a; ++j) {
                DBG(info.debug("  j " + std::to_string(j) +
                        "(" + std::to_string(ins.item(j).j) + ")");)
                Weight mu_ = s->first.mu - ins.item(j).w;
                Profit pi_ = s->first.pi - ins.item(j).p;
                DBG(info.debug(" " + std::to_string(mu_) + " " + std::to_string(pi_));)

                // Bounding
                Profit ub = 0;
                if (params.upper_bound == "b") {
                    ub = (mu_ <= c)?
                        ub_dembo(ins, b, pi_, c-mu_):
                        ub_dembo_rev(ins, b, pi_, c-mu_);
                } else if (params.upper_bound == "t") {
                    ub = (mu_ <= c)?
                        ub_dembo(ins, t+1, pi_, c-mu_):
                        ub_dembo_rev(ins, j-1, pi_, c-mu_);
                } else {
                    assert(false);
                }
                DBG(info.debug(
                        " lb " + std::to_string(lb) +
                        " ubtmp " + std::to_string(ub) +
                        " ub " + std::to_string(ub));)
                if (ub <= lb) {
                    DBG(info.debug(" ×");)
                    continue;
                } else {
                    DBG(info.debug(" ok");)
                }


                auto res = maps[k].insert({{mu_, pi_, t}, {j, f, s}});
                // If (mu_,pi_,t) existed and should be updated
                if (!res.second && res.first->second.a < j) {
                    res.first->second.a    = j;
                    res.first->second.pred = s;
                }

                // Update LB
                if (mu_ <= c && pi_ > lb) {
                    lb = pi_;
                    best_state = res.first;
                }
            }
            s->second.a_prec = s->second.a;
            DBG(info.debug("\n");)
        }
    }

    DBG(
        info.debug("Map: " + std::to_string(maps[l-b].size()) + "\n");
        for (auto s = maps[l-b].begin(); s != maps[l-b].end(); ++s)
            info.debug(to_string(*s) + "\n");
    )

    StateIdx map_size = 0;
    for (ItemPos k=0; k<=n-b; ++k)
        map_size += maps[k].size();

    if (lb == sol.profit()) {
        DBG(info.debug("Optimal value equals lower bound.\n");)
        return algorithm_end(sol, info);
    }

    // Retrieve optimal solution
    DBG(info.debug("Retrieve optimal solution...");)
    sol = *ins.break_solution();
    auto s = best_state;

    ItemPos t = best_state->first.b;
    ItemPos a = s->second.a;
    while (!(sol.profit() == best_state->first.pi && sol.remaining_capacity() >= 0)) {
        auto s_next = s->second.pred;
        DBG(info.debug("s " + to_string(*s) + " s_next " + to_string(*s_next) + "\n");)

        if (s_next->first.pi < s->first.pi) {
            while (s->first.mu != s_next->first.mu + ins.item(t).w
                    || s->first.pi != s_next->first.pi + ins.item(t).p) {
                t--;
                DBG(info.debug("t " + std::to_string(t));)
                assert(t >= b - 1);
            }
            sol.set(t, true);
            DBG(info.debug("Add " + std::to_string(t) + " p(S) " + std::to_string(sol.profit()) + "\n");)
            t--;
            assert(t >= b - 1);
        } else if (s_next->first.pi > s->first.pi) {
            while (s->first.mu + ins.item(a).w != s_next->first.mu
                    || s->first.pi + ins.item(a).p != s_next->first.pi) {
                a++;
                DBG(info.debug("a " + std::to_string(a));)
                assert(a <= b);
            }
            sol.set(a, false);
            DBG(info.debug("Remove " + std::to_string(a) + " p(S) " + std::to_string(sol.profit()) + "\n");)
            a++;
            assert(a <= b);
        }

        s = s_next;
    }

    return algorithm_end(sol, info);
}

