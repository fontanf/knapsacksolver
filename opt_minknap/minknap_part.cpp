#include "minknap.hpp"

#include "../lib/binary_solution.hpp"
#include "../lb_greedy/greedy.hpp"
#include "../lb_greedynlogn/greedynlogn.hpp"
#include "../ub_dembo/dembo.hpp"
#include "../ub_dantzig/dantzig.hpp"
#include "../ub_surrogate/surrogate.hpp"

#include <bitset>

#define DBG(x)
//#define DBG(x) x

Solution sopt_minknap_list_part(Instance& ins,
        MinknapParams params, ItemPos k, Info* info, Profit ub)
{
    assert(false); // TODO
    (void)params;
    (void)k;
    (void)info;
    (void)ub;
    Solution sol(ins);
    return ins;
}

/*

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
    BSol sol;
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
        BalknapParams& params, ItemPos k, StateIdx nodes, Info* info)
{
    if (0 <= params.ub_surrogate && params.ub_surrogate <= nodes) {
        params.ub_surrogate = -1;
        if (Info::verbose(info))
            std::cout << "SURROGATE..." << std::flush;
        so = ub_surrogate(ins, sol_best.profit());
        if (Info::verbose(info))
            std::cout << " K " << so.bound << " S " << so.multiplier << std::flush;
        if (ub > so.ub) {
            ub = so.ub;
            if (Info::verbose(info))
                std::cout << " " << ins.print_ub(ub) << std::endl;
        } else {
            if (Info::verbose(info))
                std::cout << " NO IMPROVEMENT" << std::endl;
        }
    }

    if (0 <= params.solve_sur && params.solve_sur <= nodes) {
        params.solve_sur = -1;
        if (sol_best.profit() == ub)
            return;
        if (Info::verbose(info))
            std::cout << "SOLVE SURROGATE..." << std::endl;
        assert(so.bound != -1);
        Instance ins_sur(ins);
        ins_sur.surrogate(so.multiplier, so.bound, ins_sur.first_item());
        Solution sol_sur = sopt_balknap_list_part(ins_sur, params, k, info, -1);
        if (ub > sol_sur.profit()) {
            ub = sol_sur.profit();
            if (Info::verbose(info))
                std::cout << "END SOLVE SURROGATE " << ins.print_ub(ub) << std::flush;
        }
        if (Info::verbose(info))
            std::cout << " K " << sol_sur.item_number() << "/" << so.bound << std::flush;
        if (sol_sur.item_number() == so.bound) {
            sol_best = sol_sur;
            lb = sol_best.profit();
            if (Info::verbose(info))
                std::cout << " " << ins.print_lb(sol_best.profit()) << std::flush;
        }
        if (Info::verbose(info))
            std::cout << std::endl;
    }

    if (0 <= params.lb_greedynlogn && params.lb_greedynlogn <= nodes) {
        params.lb_greedynlogn = -1;
        if (Info::verbose(info))
            std::cout << "GREEDYNLOGN..." << std::flush;
        if (sol_best.update(sol_bestgreedynlogn(ins))) {
            if (sol_best.profit() > lb)
                lb = sol_best.profit();
            if (Info::verbose(info))
                std::cout << " " << ins.print_lb(sol_best.profit()) << std::endl;
        } else {
            if (Info::verbose(info))
                std::cout << " NO IMPROVEMENT" << std::endl;
        }
    }
}

Solution sopt_balknap_list_part(
        Instance& ins, BalknapParams params, ItemPos k, Info* info, Profit o)
{
    DBG(std::cout << "BALKNAPLISTPART... OPT " << o << std::endl;)
    DBG(std::cout << ins << std::endl;)

    if (Info::verbose(info))
        std::cout << "F " << ins.first_item()
            << " L " << ins.last_item()
            << " N " << ins.item_number()
            << std::endl;

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
    if (ins.break_item() == ins.last_item()+1)
        return *ins.break_solution();

    // Compute initial lower bound
    DBG(std::cout << "LB..." << std::flush;)
    Solution sol(ins);
    if (params.lb_greedynlogn == 0) {
        params.lb_greedynlogn = -1;
        sol = sol_bestgreedynlogn(ins);
    } else if (params.lb_greedy == 0) {
        params.lb_greedy = -1;
        sol = sol_greedy(ins);
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
        ins.reduce1(lb, Info::verbose(info));
        // If the capacity is negative, then it means that sol was the optimal
        // solution. Note that this is not possible if opt-1 has been used as
        // lower bound for the reduction.
        if (ins.capacity() < 0)
            return sol;
    } else if (params.upper_bound == "t") {
        ins.reduce2(lb, Info::verbose(info));
        if (ins.capacity() < 0)
            return sol;
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
        return (ins.reduced_solution()->profit() > sol.profit())?
            *ins.reduced_solution(): sol;
    } else if (n == 1) {
        DBG(std::cout << "1 ITEM INSTANCE" << std::endl;)
        Solution sol1 = *ins.reduced_solution();
        sol1.set(f, true);
        return (sol1.profit() > sol.profit())? sol1: sol;
    } else if (ins.break_item() == ins.last_item()+1) {
        DBG(std::cout << "NO BREAK ITEM" << std::endl;)
        return (ins.break_solution()->profit() > sol.profit())?
            *ins.break_solution(): sol;
    }

    ItemPos b    = ins.break_item();
    Weight w_bar = ins.break_solution()->weight();
    Profit p_bar = ins.break_solution()->profit();
    Profit u = (o != -1)? o: ub_dantzig(ins);
    SurrogateOut so(info);
    if (sol.profit() == u) // If UB == LB, then stop
        return sol;

    DBG(std::cout
            << "N " << n << " C " << c
            << " F " << f << " L " << l
            << " B " << ins.item(b) << std::endl
            << "PBAR " << p_bar << " WBAR " << w_bar << std::endl;)

    if (Info::verbose(info))
        std::cout
            <<  "LB " << sol.profit()
            << " UB " << u
            << " GAP " << u - sol.profit() << std::endl;

    // Create memory table
    std::map<StatePart, StateValuePart, StatePart> map;

    // Initialization
    // Create first partial solution centered on the break item.
    BSolFactory bsolf(k, b, f, l);
    DBG(std::cout << "X1 " << bsolf.x1() << " X2 " << bsolf.x2() << std::endl;)
    BSol bsol_init = 0;
    for (ItemPos j=f; j<b; ++j)
        bsol_init = bsolf.add(bsol_init, j);
    // s(w_bar,p_bar) = b
    map.insert({{w_bar,p_bar},{b,f,bsol_init}});

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
        std::cout << "T " << t << " MAP " << map.size() << std::endl;
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
            DBG(std::cout << " + STATE " << *s << " ";)
            Weight mu_ = s->first.mu + wt;
            Weight pi_ = s->first.pi + pt;
            DBG(std::cout << " " << mu_ << " " << pi_ << std::flush;)

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
            DBG(std::cout << "LB " << lb << " UBTMP " << ub << " UB " << u << " ";)
            if (ub <= lb) {
                DBG(std::cout << "X" << std::endl;)
                continue;
            } else {
                DBG(std::cout << "OK" << std::flush;)
            }

            hint = map.insert(hint, {{mu_, pi_}, {s->second.a, f, bsolf.add(s->second.sol, t)}});
            if (hint->second.a < s->second.a) {
                hint->second.a    = s->second.a;
                hint->second.sol  = bsolf.add(s->second.sol, t);
            }

            // Update LB
            if (mu_ <= c && pi_ > lb) {
                lb = pi_;
                best_state = {hint->first, hint->second};
                last_item = t;
                DBG(std::cout << " BESTLB " << lb << std::flush;)
                if (lb == u)
                    goto end;
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
                DBG(std::cout << " " << mu_ << " " << pi_ << std::flush;)

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

                auto res = map.insert({{mu_,pi_},
                        {j, f, bsolf.remove(s->second.sol, j)}});
                if (!res.second) {
                    if (res.first->second.a < j) {
                        res.first->second.a = j;
                        res.first->second.sol = bsolf.remove(s->second.sol, j);
                    }
                }

                // Update LB
                if (mu_ <= c && pi_ > lb) {
                    lb = pi_;
                    best_state = {res.first->first, res.first->second};
                    last_item = t;
                    DBG(std::cout << " BESTLB " << lb << std::flush;)
                    if (lb == u)
                        goto end;
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
        return sol;

    // Reduce instance to items from best_state.second.a to last_item and remove
    // the items from the partial solution from the instance.
    // Then run the algorithm again.
    DBG(std::cout << "BEST STATE " << best_state
            << " FIRST ITEM " << best_state.second.a
            << " LAST ITEM " << last_item << std::endl;)
    DBG(std::cout << "BSOL " << std::bitset<64>(best_state.second.sol) << std::endl;)
    DBG(std::cout << "SOPT " << ins.optimal_solution()->print_bin() << std::endl;)
    DBG(std::cout << "FIX " << bsolf.x1() << " TO " << bsolf.x2() << std::endl;)
    assert(ins.check_opt(lb));
    ins.set_first_item(best_state.second.a);
    ins.set_last_item(last_item);
    DBG(std::cout << ins << std::endl;)
    ins.fix(bsolf, best_state.second.sol);
    DBG(std::cout << ins << std::endl;)

    DBG(std::cout << "BALKNAPLISTPART... END" << std::endl;)
    return sopt_balknap_list_part(ins, params, k, info, best_state.first.pi);
}

*/

#undef DBG

