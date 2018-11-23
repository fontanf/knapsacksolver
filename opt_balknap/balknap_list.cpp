#include "knapsack/opt_balknap/balknap.hpp"

#include "knapsack/lib/part_solution_1.hpp"
#include "knapsack/lb_greedy/greedy.hpp"
#include "knapsack/lb_greedynlogn/greedynlogn.hpp"
#include "knapsack/ub_dembo/dembo.hpp"
#include "knapsack/ub_dantzig/dantzig.hpp"
#include "knapsack/ub_surrogate/surrogate.hpp"

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

    info.debug("Sort items...\n");
    if (params.upper_bound == "b") {
        ins.sort_partially();
    } else if (params.upper_bound == "t") {
        ins.sort();
    } else {
        assert(false);
    }
    if (ins.break_item() == ins.last_item()+1) { // all items are in the break solution
        info.debug("All items fit in the knapsack.\n");
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
        info.debug("All items have been reduced.\n");
        return algorithm_end(lb, info);
    }
    info.debug("Reduced solution: " + ins.reduced_solution()->print_bin() + "\n");
    info.debug("Reduced solution: " + ins.reduced_solution()->print_in() + "\n");

    Weight  c = ins.total_capacity();
    ItemPos f = ins.first_item();
    ItemPos l = ins.last_item();
    ItemPos n = ins.item_number();
    Profit p0 = ins.reduced_solution()->profit();

    // Trivial cases
    if (n == 0 || c == 0) {
        info.debug("Empty instance (after reduction).\n");
        return algorithm_end(std::max(lb, p0), info);
    } else if (n == 1) {
        info.debug("Instance only contains one item (after reduction).\n");
        return algorithm_end(std::max(lb, p0 + ins.item(f).p), info);
    } else if (ins.break_item() == ins.last_item()+1) {
        info.debug("All items fit in the knapsack (after reduction).\n");
        return algorithm_end(std::max(lb, ins.break_solution()->profit()), info);
    }

    ItemPos b    = ins.break_item();
    Weight w_bar = ins.break_solution()->weight();
    Profit p_bar = ins.break_solution()->profit();
    Info info_tmp;
    Profit u = ub_dantzig(ins, info_tmp);

    info.debug("Break solution: " + ins.break_solution()->print_bin() + "\n");
    info.debug("Break solution: " + ins.break_solution()->print_in() + "\n");
    info.debug(
            "n " + std::to_string(n) +
            " c " + std::to_string(c) +
            " f " + std::to_string(f) + " l " + std::to_string(l) +
            "\n" +
            "wbar " + std::to_string(w_bar) +
            " pbar " + std::to_string(p_bar) +
            "\n");

    if (ins.break_solution()->profit() > lb) {
        info.debug("Update best solution.");
        lb = ins.break_solution()->profit();
    }

    info.verbose(
            "lb " + std::to_string(lb) +
            " ub " + std::to_string(u) +
            " gap " + std::to_string(u - lb) +
            "\n");

    // If UB == LB, then stop
    if (lb >= u) {
        info.debug("Lower bound equals upper bound.");
        return algorithm_end(lb, info);
    }

    // Create memory table
    std::map<State, StateValue, State> map;

    // Initialization
    map.insert({{w_bar,p_bar},{b,f}}); // s(w_bar,p_bar) = b

    // Recursion
    info.verbose("Recursion...\n");
    for (ItemPos t=b; t<=l; ++t) {
        if (info.debug()) {
            info.debug("Map: " + std::to_string(map.size()) + "\n");
            for (auto s = map.begin(); s != map.end(); ++s)
                info.debug(to_string(*s) + "\n");
            info.debug(
                    "t " + std::to_string(t) +
                    " (" + std::to_string(ins.item(t).j) + ")\n"
                    );
        }

        Weight wt = ins.item(t).w;
        Profit pt = ins.item(t).p;

        // Bounding
        if (params.upper_bound == "t") {
            info.debug("Bounding...");
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
        info.debug("Add...\n");
        auto s = map.upper_bound({c+1,0});
        auto hint = s;
        hint--;
        while (s != map.begin() && (--s)->first.mu <= c) {
            info.debug(" + State " + to_string(*s));
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
            info.debug(
                    " lb " + std::to_string(lb) +
                    " ubtmp " + std::to_string(ub) +
                    " ub " + std::to_string(ub));
            if (ub <= lb) {
                info.debug(" ×\n");
                continue;
            } else {
                info.debug(" ok\n");
            }

            hint = map.insert(hint, {{mu_, pi_}, {s->second.a, f}});
            if (hint->second.a < s->second.a)
                hint->second.a    = s->second.a;
            hint--;
        }

        // Remove previously added items
        info.debug("Remove...\n");
        for (auto s = map.rbegin(); s != map.rend() && s->first.mu > c; ++s) {
            if (s->first.mu > c + wt)
                continue;
            info.debug(" - State " + to_string(*s));

            for (ItemPos j = s->second.a_prec; j < s->second.a; ++j) {
                info.debug("  j " + std::to_string(j) +
                        "(" + std::to_string(ins.item(j).j) + ")");
                Weight mu_ = s->first.mu - ins.item(j).w;
                Profit pi_ = s->first.pi - ins.item(j).p;
                info.debug(" " + std::to_string(mu_) + " " + std::to_string(pi_));

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
                info.debug(
                        " lb " + std::to_string(lb) +
                        " ubtmp " + std::to_string(ub) +
                        " ub " + std::to_string(ub));
                if (ub <= lb) {
                    info.debug(" ×");
                    continue;
                } else {
                    info.debug(" ok");
                }

                auto res = map.insert({{mu_,pi_},{j, f}});
                if (!res.second)
                    if (res.first->second.a < j)
                        res.first->second.a = j;
            }
            s->second.a_prec = s->second.a;
            info.debug("\n");
        }
    }

    if (info.debug()) {
        info.debug("Map: " + std::to_string(map.size()) + "\n");
        for (auto s = map.begin(); s != map.end(); ++s)
            info.debug(to_string(*s) + "\n");
    }

    assert(ins.check_opt(lb));
    return algorithm_end(lb, info);
}

/******************************************************************************/

#define DBG(x)
//#define DBG(x) x

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

void update_bounds(const Instance& ins, Solution& sol_best, Profit& lb,
        Profit& ub, SurrogateOut& so,
        BalknapParams& params, ItemPos k, StateIdx nodes, Info& info)
{
    if (0 <= params.ub_surrogate && params.ub_surrogate <= nodes) {
        params.ub_surrogate = -1;
        info.verbose("Surrogate...\n");
        Info info_tmp;
        so = ub_surrogate(ins, sol_best.profit(), info);
        info.verbose(" k " + std::to_string(so.bound)
                + " s " + std::to_string(so.multiplier));
        if (ub > so.ub) {
            ub = so.ub;
            info.verbose(" " + std::to_string(ub) + "\n");
        } else {
            info.verbose(" no improvement\n");
        }
    }

    if (0 <= params.solve_sur && params.solve_sur <= nodes) {
        params.solve_sur = -1;
        if (sol_best.profit() == ub)
            return;
        info.verbose("Solve surrogate instance...\n");
        assert(so.bound != -1);
        Instance ins_sur(ins);
        ins_sur.surrogate(so.multiplier, so.bound, ins_sur.first_item());
        info.verbose("<- End solve surrogate instance\n");
        Solution sol_sur = sopt_balknap_list_part(ins_sur, info, params, k, -1);
        if (ub > sol_sur.profit()) {
            ub = sol_sur.profit();
            info.verbose("Update upper bound " + std::to_string(ub) + "\n");
        }
        info.debug("k " + std::to_string(sol_sur.item_number())
                + "/" + std::to_string(so.bound) + "\n");
        if (sol_sur.item_number() == so.bound) {
            sol_best = sol_sur;
            lb = sol_best.profit();
            info.verbose("Update best solution " + std::to_string(sol_best.profit()));
        }
        info.verbose("\n");
    }

    if (0 <= params.lb_greedynlogn && params.lb_greedynlogn <= nodes) {
        params.lb_greedynlogn = -1;
        info.verbose("Run greedynlogn...");
        Info info_tmp;
        if (sol_best.update(sol_bestgreedynlogn(ins, info_tmp))) {
            if (sol_best.profit() > lb)
                lb = sol_best.profit();
            info.verbose(" update best solution " + std::to_string(sol_best.profit()) + "\n");
        } else {
            info.verbose(" no improvement\n");
        }
    }
}

Solution knapsack::sopt_balknap_list_part(Instance& ins, Info& info,
        BalknapParams params, ItemPos k, Profit o)
{
    if (o == -1)
        info.verbose("**** balknap (list, part " + std::to_string(k) + ")***\n");
    info.debug(
            "n " + std::to_string(ins.item_number()) + "/" + std::to_string(ins.total_item_number()) +
            " f " + std::to_string(ins.first_item()) +
            " l " + std::to_string(ins.last_item()) +
            " o " + std::to_string(o) +
            "\n");

    // Sorting
    DBG(std::cout << "SORTING..." << std::endl;)
    if (params.upper_bound == "b") {
        ins.sort_partially();
    } else if (params.upper_bound == "t") {
        ins.sort();
    } else {
        assert(false);
    }
    // If all items fit in the knapsack then return break solution
    if (ins.break_item() == ins.last_item()+1) {
        Solution sol = *ins.break_solution();
        return algorithm_end(sol, info);
    }

    // Compute initial lower bound
    DBG(std::cout << "LB..." << std::flush;)
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
    DBG(std::cout << " " << ins.print_lb(sol.profit()) << std::endl;)

    // Variable reduction
    DBG(std::cout << "REDUCTION..." << std::endl;)
    // If we already know the optimal value, we can use opt-1 as lower bound
    // for the reduction.
    Profit lb = (o != -1 && o > sol.profit())? o-1: sol.profit();
    if (params.upper_bound == "b") {
        ins.reduce1(lb, info);
        // If the capacity is negative, then it means that sol was the optimal
        // solution. Note that this is not possible if opt-1 has been used as
        // lower bound for the reduction.
        if (ins.capacity() < 0)
            return algorithm_end(sol, info);
    } else if (params.upper_bound == "t") {
        ins.reduce2(lb, info);
        if (ins.capacity() < 0)
            return algorithm_end(sol, info);
    } else {
        assert(false);
    }
    Weight  c = ins.total_capacity();
    ItemPos f = ins.first_item();
    ItemPos l = ins.last_item();
    ItemPos n = ins.item_number();

    // Trivial cases
    // Return the best solution between the solution of the trivial instance and
    // the solution used for the reduction.
    if (n == 0 || c == 0) {
        DBG(std::cout << "EMPTY INSTANCE" << std::endl;)
        if (ins.reduced_solution()->profit() > sol.profit())
            sol = *ins.reduced_solution();
        return algorithm_end(sol, info);
    } else if (n == 1) {
        DBG(std::cout << "1 ITEM INSTANCE" << std::endl;)
        Solution sol1 = *ins.reduced_solution();
        sol1.set(f, true);
        if (sol1.profit() > sol.profit())
            sol1 = sol;
        return algorithm_end(sol, info);
    } else if (ins.break_item() == ins.last_item()+1) {
        DBG(std::cout << "NO BREAK ITEM" << std::endl;)
        if (ins.break_solution()->profit() > sol.profit())
            sol = *ins.break_solution();
        return algorithm_end(sol, info);
    }

    ItemPos b    = ins.break_item();
    Weight w_bar = ins.break_solution()->weight();
    Profit p_bar = ins.break_solution()->profit();
    Info info_tmp;
    Profit u = (o != -1)? o: ub_dantzig(ins, info_tmp);
    SurrogateOut so;
    if (sol.profit() == u) // If UB == LB, then stop
        return algorithm_end(sol, info);

    info.debug("Break solution: " + ins.break_solution()->print_bin() + "\n");
    info.debug("Break solution: " + ins.break_solution()->print_in() + "\n");
    info.debug(
            "n " + std::to_string(n) +
            " c " + std::to_string(c) +
            " f " + std::to_string(f) + " l " + std::to_string(l) +
            "\n" +
            "wbar " + std::to_string(w_bar) +
            " pbar " + std::to_string(p_bar) +
            "\n");

    if (ins.break_solution()->profit() > sol.profit()) {
        info.debug("Update best solution.");
        sol = *ins.break_solution();
    }

    info.verbose(
            "lb " + std::to_string(sol.profit()) +
            " ub " + std::to_string(u) +
            " gap " + std::to_string(u - sol.profit()) +
            "\n");

    // Create memory table
    std::map<StatePart, StateValuePart, StatePart> map;

    // Initialization
    // Create first partial solution centered on the break item.
    PartSolFactory1 psolf(k, b, f, l);
    DBG(std::cout << "X1 " << bsolf.x1() << " X2 " << bsolf.x2() << std::endl;)
    PartSol1 psol_init = 0;
    for (ItemPos j=f; j<b; ++j)
        psol_init = psolf.add(psol_init, j);
    // s(w_bar,p_bar) = b
    map.insert({{w_bar,p_bar},{b,f,psol_init}});

    DBG(for (auto s = map.begin(); s != map.end(); ++s)
        std::cout << *s << " ";
    std::cout << std::endl;)

    // Best state. Note that it is not a pointer
    std::pair<StatePart, StateValuePart> best_state = {map.begin()->first, map.begin()->second};
    // Also keep last added item to improve the variable reduction at the end.
    ItemPos last_item = b-1;
    // Update LB
    if (best_state.first.pi > lb) {
        lb = best_state.first.pi;
        last_item = b;
        DBG(std::cout << " BESTLB " << lb << std::flush;)
            if (lb == u)
                goto end;
    }

    // Recursion
    DBG(std::cout << "RECURSION..." << std::endl;)
    for (ItemPos t=b; t<=l; ++t) {
        //std::cout << "T " << t << " MAP " << map.size() << std::endl;
        DBG(std::cout << "MAP " << map.size() << std::flush;)
        DBG(std::cout << " T " << t << " " << ins.item(t) << std::endl;)
        Weight wt = ins.item(t).w;
        Profit pt = ins.item(t).p;

        update_bounds(ins, sol, lb, u, so, params, k, map.size(), info);
        if (lb == u)
            goto end;

        // Bounding
        // If the upper bound with the break item is used, then the upper bound
        // is not smeller after each iteration. Thus we don't need to compute
        // it.
        // On the other hand, if the other upper bound is used, then it will
        // be lower for feasible states.
        if (params.upper_bound == "t") {
            DBG(std::cout << "BOUNDING..." << std::endl;)
            for (auto s = map.begin(); s != map.end() && s->first.mu <= c;) {
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
        // If there is no more states, the stop
        if (map.size() == 0)
            break;

        // Add item t
        DBG(std::cout << "ADD..." << std::endl;)
        auto s = map.upper_bound({c+1,0});
        auto hint = s;
        hint--;
        while (s != map.begin() && (--s)->first.mu <= c) {
            DBG(std::cout << " + STATE " << *s;)
            std::pair<StatePart, StateValuePart> s1 = {
                {s->first.mu + wt, s->first.pi + pt},
                {s->second.a, f, psolf.add(s->second.sol, t)}};
            Weight mu_ = s1.first.mu;
            Profit pi_ = s1.first.pi;
            DBG(std::cout << " " << s1 << std::flush;)

            // Update LB
            if (mu_ <= c && pi_ > lb) {
                lb = pi_;
                best_state = s1;
                last_item = t;
                DBG(std::cout << " BESTLB " << lb << std::flush;)
                if (lb == u)
                    goto end;
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
            DBG(std::cout << " LB " << lb << " UBTMP " << ub << " UB " << u;)
            if (ub <= lb) {
                DBG(std::cout << " X" << std::endl;)
                continue;
            } else {
                DBG(std::cout << " OK" << std::flush;)
            }

            hint = map.insert(hint, s1);
            if (hint->second.a < s->second.a) {
                hint->second.a    = s->second.a;
                hint->second.sol  = psolf.add(s->second.sol, t);
            }
            hint--;
            DBG(std::cout << std::endl;)
        }

        // Remove previously added items
        DBG(std::cout << "REMOVE..." << std::endl;)
        for (auto s = map.rbegin(); s != map.rend() && s->first.mu > c; ++s) {
            if (s->first.mu > c + wt)
                continue;
            DBG(std::cout << " - STATE " << *s << std::endl;)

            for (ItemPos j = s->second.a_prec; j < s->second.a; ++j) {
                DBG(std::cout << "  J " << j << " " << ins.item(j);)
                Weight mu_ = s->first.mu - ins.item(j).w;
                Profit pi_ = s->first.pi - ins.item(j).p;
                std::pair<StatePart, StateValuePart> s1 = {
                    {mu_, pi_},
                    {j, f, psolf.remove(s->second.sol, j)}};
                DBG(std::cout << " " << mu_ << " " << pi_ << std::flush;)

                // Update LB
                if (mu_ <= c && pi_ > lb) {
                    lb = pi_;
                    best_state = s1;
                    last_item = t;
                    DBG(std::cout << " BESTLB " << lb << std::flush;)
                    if (lb == u)
                        goto end;
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
                DBG(std::cout << " LB " << lb << " UBTMP " << ub << " UB " << u;)
                if (ub <= lb) {
                    DBG(std::cout << " X" << std::endl;)
                    continue;
                } else {
                    DBG(std::cout << " OK" << std::flush;)
                }

                auto res = map.insert(s1);
                if (!res.second) {
                    if (res.first->second.a < j) {
                        res.first->second.a = j;
                        res.first->second.sol = psolf.remove(s->second.sol, j);
                    }
                }
                DBG(std::cout << std::endl;)
            }
            s->second.a_prec = s->second.a;
        }
        DBG(std::cout << std::endl;)

        DBG(for (auto s = map.begin(); s != map.end(); ++s)
            std::cout << *s << " ";
        std::cout << std::endl;)
    }
end:

    // If we didn't improve sol, then it means that sol was optimal
    if (best_state.first.pi < sol.profit())
        return algorithm_end(sol, info);

    // Reduce instance to items from best_state.second.a to last_item and remove
    // the items from the partial solution from the instance.
    // Then run the algorithm again.
    DBG(std::cout << "BEST STATE " << best_state
            << " FIRST ITEM " << best_state.second.a
            << " LAST ITEM " << last_item << std::endl;)
    DBG(std::cout << "PSOL " << psolf.print(best_state.second.sol) << std::endl;)
    DBG(std::cout << "SOPT " << ins.optimal_solution()->print_bin() << std::endl;)
    assert(ins.check_opt(lb));
    ins.set_first_item(best_state.second.a);
    ins.set_last_item(last_item);
    DBG(std::cout << ins << std::endl;)
    ins.fix(psolf, best_state.second.sol);

    Info info_tmp2;
    sol = knapsack::sopt_balknap_list_part(ins, info_tmp2,  params, k, best_state.first.pi);
    return algorithm_end(sol, info);
}

#undef DBG

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

    info.debug("Sort items...\n");
    if (params.upper_bound == "b") {
        ins.sort_partially();
    } else if (params.upper_bound == "t") {
        ins.sort();
    } else {
        assert(false);
    }
    if (ins.break_item() == ins.last_item()+1) { // all items are in the break solution
        info.debug("All items fit in the knapsack.\n");
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
        info.debug("All items have been reduced.\n");
        return algorithm_end(sol, info);
    }
    info.debug("Reduced solution: " + ins.reduced_solution()->print_bin() + "\n");
    info.debug("Reduced solution: " + ins.reduced_solution()->print_in() + "\n");

    Weight  c = ins.total_capacity();
    ItemPos f = ins.first_item();
    ItemPos l = ins.last_item();
    ItemPos n = ins.item_number();

    // Trivial cases
    if (n == 0 || c == 0) {
        info.debug("Empty instance (after reduction).\n");
        if (ins.reduced_solution()->profit() > sol.profit())
            sol = *ins.reduced_solution();
        return algorithm_end(sol, info);
    } else if (n == 1) {
        info.debug("Instance only contains one item (after reduction).\n");
        Solution sol1 = *ins.reduced_solution();
        sol1.set(f, true);
        if (sol1.profit() > sol.profit())
            sol = sol1;
        return algorithm_end(sol, info);
    } else if (ins.break_item() == ins.last_item()+1) {
        info.debug("All items fit in the knapsack (after reduction).\n");
        if (ins.break_solution()->profit() > sol.profit())
            sol = *ins.break_solution();
        return algorithm_end(sol, info);
    }

    ItemPos b    = ins.break_item();
    Weight w_bar = ins.break_solution()->weight();
    Profit p_bar = ins.break_solution()->profit();
    Info info_tmp;
    Profit u = ub_dantzig(ins, info_tmp);

    info.debug("Break solution: " + ins.break_solution()->print_bin() + "\n");
    info.debug("Break solution: " + ins.break_solution()->print_in() + "\n");
    info.debug(
            "n " + std::to_string(n) +
            " c " + std::to_string(c) +
            " f " + std::to_string(f) + " l " + std::to_string(l) +
            "\n" +
            "wbar " + std::to_string(w_bar) +
            " pbar " + std::to_string(p_bar) +
            "\n");

    if (ins.break_solution()->profit() > sol.profit()) {
        info.debug("Update best solution.");
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
        info.debug("Lower bound equals upper bound.");
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
        if (info.debug()) {
            info.debug("Map: " + std::to_string(maps[k-1].size()) + "\n");
            for (auto s = maps[k-1].begin(); s != maps[k-1].end(); ++s)
                info.debug(to_string(*s) + "\n");
            info.debug(
                    "t " + std::to_string(t) +
                    " (" + std::to_string(ins.item(t).j) + ")\n"
                    );
        }

        Weight wt = ins.item(t).w;
        Profit pt = ins.item(t).p;

        maps[k] = std::map<StateAll, StateValueAll, StateAll>();
        for (auto s = maps[k-1].begin(); s != maps[k-1].end(); ++s)
            maps[k].insert({
                    {s->first.mu, s->first.pi, t},
                    {s->second.a, s->second.a, s}});

        // Bounding
        if (params.upper_bound == "t") {
            info.debug("Bounding...");
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
        info.debug("Add...\n");
        auto s = maps[k].upper_bound({c+1,0,0});
        auto hint = s;
        if (s != maps[k].begin())
            hint--;
        while (s != maps[k].begin() && (--s)->first.mu <= c) {
            info.debug(" + State " + to_string(*s));
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
            info.debug(
                    " lb " + std::to_string(lb) +
                    " ubtmp " + std::to_string(ub) +
                    " ub " + std::to_string(ub));
            if (ub <= lb) {
                info.debug(" ×\n");
                continue;
            } else {
                info.debug(" ok\n");
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
        info.debug("Remove...\n");
        for (auto s = --(maps[k].end()); s->first.mu > c; --s) {
            if (s->first.mu > c + wt)
                continue;
            info.debug(" - State " + to_string(*s));

            for (ItemPos j = s->second.a_prec; j < s->second.a; ++j) {
                info.debug("  j " + std::to_string(j) +
                        "(" + std::to_string(ins.item(j).j) + ")");
                Weight mu_ = s->first.mu - ins.item(j).w;
                Profit pi_ = s->first.pi - ins.item(j).p;
                info.debug(" " + std::to_string(mu_) + " " + std::to_string(pi_));

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
                info.debug(
                        " lb " + std::to_string(lb) +
                        " ubtmp " + std::to_string(ub) +
                        " ub " + std::to_string(ub));
                if (ub <= lb) {
                    info.debug(" ×");
                    continue;
                } else {
                    info.debug(" ok");
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
            info.debug("\n");
        }
    }

    if (info.debug()) {
        info.debug("Map: " + std::to_string(maps[l-b].size()) + "\n");
        for (auto s = maps[l-b].begin(); s != maps[l-b].end(); ++s)
            info.debug(to_string(*s) + "\n");
    }

    StateIdx map_size = 0;
    for (ItemPos k=0; k<=n-b; ++k)
        map_size += maps[k].size();

    assert(ins.check_opt(lb));
    if (lb == sol.profit()) {
        info.debug("Optimal value equals lower bound.\n");
        return algorithm_end(sol, info);
    }

    // Retrieve optimal solution
    info.debug("Retrieve optimal solution...");
    sol = *ins.break_solution();
    auto s = best_state;

    ItemPos t = best_state->first.b;
    ItemPos a = s->second.a;
    while (!(sol.profit() == best_state->first.pi && sol.remaining_capacity() >= 0)) {
        auto s_next = s->second.pred;
        info.debug("s " + to_string(*s) + " s_next " + to_string(*s_next) + "\n");

        if (s_next->first.pi < s->first.pi) {
            while (s->first.mu != s_next->first.mu + ins.item(t).w
                    || s->first.pi != s_next->first.pi + ins.item(t).p) {
                t--;
                info.debug("t " + std::to_string(t));
                assert(t >= b - 1);
            }
            sol.set(t, true);
            info.debug("Add " + std::to_string(t) + " p(S) " + std::to_string(sol.profit()) + "\n");
            t--;
            assert(t >= b - 1);
        } else if (s_next->first.pi > s->first.pi) {
            while (s->first.mu + ins.item(a).w != s_next->first.mu
                    || s->first.pi + ins.item(a).p != s_next->first.pi) {
                a++;
                info.debug("a " + std::to_string(a));
                assert(a <= b);
            }
            sol.set(a, false);
            info.debug("Remove " + std::to_string(a) + " p(S) " + std::to_string(sol.profit()) + "\n");
            a++;
            assert(a <= b);
        }

        s = s_next;
    }

    assert(ins.check_sopt(sol));
    return algorithm_end(sol, info);
}

