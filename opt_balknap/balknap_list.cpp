#include "balknap.hpp"

#include "../lb_greedy/greedy.hpp"
#include "../lb_greedynlogn/greedynlogn.hpp"
#include "../ub_dembo/dembo.hpp"
#include "../ub_dantzig/dantzig.hpp"
#include "../ub_surrogate/surrogate.hpp"

#include <map>

#define DBG(x)
//#define DBG(x) x

void opt_balknap_list_update_bounds(Instance ins,
        Profit& lb, Profit& ub, SurrogateOut& so,
        BalknapParams& p, StateIdx it, ItemPos k, Info* info)
{
    if (p.greedy == it) {
        lb = std::max(lb, sol_bestgreedy(ins).profit());
        if (Info::verbose(info))
            std::cout << "RUN GREEDY... " << ins.print_lb(lb) << std::endl;
    }
    if (p.greedynlogn == it) {
        lb = std::max(lb, sol_bestgreedynlogn(ins).profit());
        if (Info::verbose(info))
            std::cout << "RUN GREEDYNLOGN... " << ins.print_lb(lb) << std::endl;
    }
    if (p.surrogate == it) {
        so = ub_surrogate(ins, {{ins.first_item(), ins.last_item()}}, lb, info);
        ub = (so.ub > ub)? so.ub: ub;
        if (Info::verbose(info))
            std::cout << "RUN SURROGATE RELAXATION... " << ins.print_ub(ub) << std::endl;
    }
    if (p.solve_sur == it) {
        p.solve_sur = -1;
        Instance ins_sur(ins);
        ins.surrogate(so.multiplier, so.bound);
        Solution sol_sur = sopt_balknap_list_part(ins_sur, p, k, NULL);
        if (sol_sur.item_number() == so.bound)
            lb = sol_sur.profit();
    }
}

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

std::ostream& operator<<(std::ostream& os, const std::pair<State, StateValue>& s)
{
    os << "(mu " << s.first.mu << " pi " << s.first.pi << " a " << s.second.a << " ap " << s.second.a_prec << ")";
    return os;
}

Profit opt_balknap_list(
        Instance& ins, BalknapParams params, Info* info)
{
    DBG(std::cout << "BALKNAPLISTOPT..." << std::endl;)
    DBG(std::cout << ins << std::endl;)

    DBG(std::cout << "SORTING..." << std::endl;)
    if (params.upper_bound == "b") {
        ins.sort_partially();
    } else if (params.upper_bound == "t") {
        ins.sort();
    } else {
        assert(false);
    }
    ItemPos b = ins.break_item();
    if (b == ins.last_item()+1) // all items are in the break solution
        return ins.break_profit();

    DBG(std::cout << "LB..." << std::endl;)
    Profit lb = 0;
    if (params.greedynlogn == 0) {
        lb = sol_bestgreedynlogn(ins).profit();
    } else if (params.greedy == 0) {
        lb = sol_bestgreedy(ins).profit();
    } else {
        lb = ins.break_profit();
    }

    DBG(std::cout << "REDUCTION..." << std::endl;)
    if (params.reduction == "1") {
        if (ins.reduce1(lb, Info::verbose(info)))
            return lb;
    } else {
        if (ins.reduce2(lb, Info::verbose(info)))
            return lb;
    }
    Weight  c = ins.total_capacity();
    ItemPos f = ins.first_item();
    ItemPos l = ins.last_item();
    ItemPos n = ins.item_number();
    Profit p0 = ins.reduced_solution()->profit();

    // Trivial cases
    if (n == 0) {
        return p0;
    } else if (n == 1) {
        return p0 + ins.item(f).p;
    }

    Weight w_bar = ins.break_solution()->weight();
    Profit p_bar = ins.break_solution()->profit();
    Profit u     = ub_dantzig(ins);

    DBG(std::cout
            << "N " << n << " C " << c
            << " F " << f << " L " << l
            << " B " << ins.item(b) << std::endl
            << "PBAR " << p_bar << " WBAR " << w_bar << std::endl;)
    if (Info::verbose(info))
        std::cout
            <<  "LB " << lb
            << " UB " << u
            << " GAP " << u - lb << std::endl;

    if (lb == u) // If UB == LB, then stop
        return lb;

    // Create memory table
    std::map<State, StateValue, State> map;

    // Initialization
    map.insert({{w_bar,p_bar},{b,f}}); // s(w_bar,p_bar) = b

    DBG(for (auto s = map.begin(); s != map.end(); ++s)
        std::cout << *s << " ";
    std::cout << std::endl;)

    DBG(std::cout << "RECURSION..." << std::endl;)
    for (ItemPos t=b; t<=l; ++t) {
        DBG(std::cout << "MAP " << map.size() << std::flush;)
        DBG(std::cout << " T " << t << " " << ins.item(t) << std::endl;)
        Weight wt = ins.item(t).w;
        Profit pt = ins.item(t).p;

        // Bounding
        if (params.upper_bound == "t") {
            DBG(std::cout << "BOUNDING..." << std::endl;)
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
        DBG(std::cout << "ADD..." << std::endl;)
        auto s = map.upper_bound({c+1,0});
        auto hint = s;
        hint--;
        while (s != map.begin() && (--s)->first.mu <= c) {
            DBG(std::cout << " + STATE " << *s << " ";)
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
            DBG(std::cout << "LB " << lb << " UBTMP " << ub << " UB " << u << " ";)
            if (ub <= lb) {
                DBG(std::cout << "X" << std::endl;)
                continue;
            } else {
                DBG(std::cout << "OK" << std::endl;)
            }

            hint = map.insert(hint, {{mu_, pi_}, {s->second.a, f}});
            if (hint->second.a < s->second.a)
                hint->second.a    = s->second.a;
            hint--;
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
                DBG(std::cout << " LB " << lb << " UBTMP " << ub << " UB " << u;)
                if (ub <= lb) {
                    DBG(std::cout << " X" << std::endl;)
                    continue;
                } else {
                    DBG(std::cout << " OK" << std::endl;)
                }

                auto res = map.insert({{mu_,pi_},{j, f}});
                if (!res.second)
                    if (res.first->second.a < j)
                        res.first->second.a = j;
            }
            s->second.a_prec = s->second.a;
        }
        DBG(std::cout << std::endl;)

        DBG(for (auto s = map.begin(); s != map.end(); ++s)
            std::cout << *s << " ";
        std::cout << std::endl;)
    }

    assert(ins.check_opt(lb));
    DBG(std::cout << "BALKNAPLISTOPT... END" << std::endl;)
    return lb;
}

/******************************************************************************/

#undef DBG


Solution sopt_balknap_list_all(
        Instance& ins, BalknapParams params, Info* info)
{
    (void)info;
    (void)params;
    assert(false); // TODO
    return Solution(ins);
}

Solution sopt_balknap_list_part(
        Instance& ins, BalknapParams params, ItemPos k, Info* info)
{
    (void)info;
    (void)params;
    (void)k;
    assert(false); // TODO
    return Solution(ins);
}


/******************************************************************************/

/*

#define DBG(x)
//#define DBG(x) x

struct State2
{
    Weight mu;
    Profit pi;
    ItemPos b;
    bool operator()(const State2& s1, const State2& s2)
    {
        if (s1.mu != s2.mu)
            return s1.mu < s2.mu;
        if (s1.pi != s2.pi)
            return s1.pi < s2.pi;
        return false;
    }
};

struct StateValue2
{
    ItemPos a;
    ItemPos a_prec; // 'a' from state '(mu,pi,a,b-1)'
    std::map<State2, StateValue2, State2>::iterator pred;
};

std::ostream& operator<<(std::ostream& os, const std::pair<State2, StateValue2>& s)
{
    auto pred = s.second.pred;
    os << "(" << s.first.mu << " " << s.first.pi
        << " " << s.second.a << " " << s.first.b
        << " " << s.second.a_prec
        << " [" << pred->first.mu << " " << pred->first.pi
        << " " << pred->second.a << " " << pred->first.b << "]"
        << ")";
    assert(
               (pred->first.pi <  s.first.pi && pred->second.a == s.second.a && pred->first.b + 1 == s.first.b)
            || (pred->first.pi == s.first.pi)
            || (pred->first.pi >  s.first.pi && pred->second.a >  s.second.a && pred->first.b     == s.first.b));
    return os;
}

Solution sopt_balknap_list(const Instance& instance,
        Profit lb, std::string ub_type, Info* info)
{
    DBG(std::cout << "BALKNAPLISTSOL..." << std::endl;);
    DBG(std::cout << instance << std::endl;)
    assert(instance.sort_type() == "eff" || instance.sort_type() == "peff");

    Weight  c = instance.capacity();
    ItemPos n = instance.item_number();
    Profit p0 = instance.reduced_solution()->profit();
    ItemPos b = instance.break_item();

    // Trivial cases
    if (n == 0) {
        return *instance.reduced_solution();
    } else if (n == 1) {
        Solution sol = *instance.reduced_solution();
        sol.set(0, true);
        return sol;
    } else if (b == n) { // all items are in the break solution
        return sol_break(instance);
    }

    Profit pb    = instance.item(b).p;
    Weight wb    = instance.item(b).w;
    Profit p_bar = instance.break_profit();
    Weight w_bar = instance.break_weight();
    Weight r     = instance.break_capacity();
    Profit z     = p_bar;
    Profit u     = p_bar + r * pb / wb;
    if (z < lb - p0)
        z = lb - p0;

    DBG(std::cout << "n " << n << " c " << c << std::endl;)
    DBG(std::cout << "b " << instance.item(b) << std::endl;)
    DBG(std::cout << "pbar " << p_bar << " wbar " << w_bar << std::endl;)
    if (Info::verbose(info))
        std::cout
            <<  "LB " << z
            << " UB " << u
            << " GAP " << u - z << std::endl;

    // Create memory table
    std::vector<std::map<State2, StateValue2, State2>> maps(n-b+2);
    maps[0] = std::map<State2, StateValue2, State2>();

    // Initialization
    maps[0].insert({{w_bar,p_bar,b-1},{b,0,maps[0].end()}}); // s(w_bar,p_bar) = b
    maps[0].begin()->second.pred = maps[0].begin();

    //DBG(std::cout << "-------- Memory --------" << std::endl;
    //for (ItemPos kk = 0; kk <= 0; ++kk) {
    //std::cout << "t " << kk + b - 1 << ": " << std::flush;
    //for (auto s = maps[kk].begin(); s != maps[kk].end(); ++s)
    //std::cout << s << " ";
    //std::cout << std::endl;
    //}
    //std::cout << "------------------------" << std::endl;)

    DBG(std::cout << "RECURSION..." << std::endl;)
    for (ItemPos t=b; t<n; ++t) { // Recursion
        DBG(std::cout << "t " << t << " " << instance.item(t) << std::endl;)
        Weight wt = instance.item(t).w;
        Profit pt = instance.item(t).p;
        ItemPos k = t - b + 1;

        maps[k] = std::map<State2, StateValue2, State2>();
        for (auto s = maps[k-1].begin(); s != maps[k-1].end(); ++s)
            maps[k].insert({
                    {s->first.mu, s->first.pi, t},
                    {s->second.a, s->second.a, s}});

        // Add item t
        auto s = maps[k].upper_bound({c+1,0,0});
        auto hint = s;
        if (s != maps[k].begin())
            hint--;
        while (s != maps[k].begin() && (--s)->first.mu <= c) {
            DBG(std::cout << " + STATE " << *s << " ";)
            Weight mu_ = s->first.mu + wt;
            Weight pi_ = s->first.pi + pt;

            // Bounding
            Profit ub = 0;
            if (ub_type == "dembo") {
                ub = (mu_ <= c)?
                    ub_dembo(instance, b, pi_, c-mu_):
                    ub_dembo_rev(instance, b, pi_, c-mu_);
            } else if (ub_type == "trivial") {
                ub = (mu_ <= c)?
                    ub_dembo(instance, t, pi_, c-mu_):
                    ub_dembo_rev(instance, s->second.a-1, pi_, c-mu_);
            } else if (ub_type == "dantzig"){
                assert(instance.sort_type() == "eff");
                ub = (mu_ <= c)?
                    ub_dantzig_from(instance, t, pi_, c-mu_):
                    ub_dantzig_from_rev(instance, s->second.a-1, pi_, c-mu_);
            } else {
                assert(false);
            }
            DBG(std::cout << "LB " << z << " UBTMP " << ub << " UB " << u << " ";)
            if (ub <= z || ub > u) {
                DBG(std::cout << "X" << std::endl;)
                continue;
            } else {
                DBG(std::cout << "OK" << std::endl;)
            }

            hint = maps[k].insert(hint, {{mu_, pi_, t}, {s->second.a, 0, s->second.pred}});
            // If (mu_,pi_,t) existed but should be updated
            if (hint->second.a < s->second.a) {
                hint->second.a    = s->second.a;
                hint->second.pred = s->second.pred;
            }
            hint--;
        }

        // Remove previously added items
        for (auto s = --(maps[k].end()); s->first.mu > c; --s) {
            if (s->first.mu > c + wt)
                continue;
            DBG(std::cout << " - STATE " << *s << std::endl;)

            for (ItemPos j = s->second.a_prec; j < s->second.a; ++j) {
                DBG(std::cout << "  j " << j << " " << instance.item(j);)
                Weight mu_ = s->first.mu - instance.item(j).w;
                Profit pi_ = s->first.pi - instance.item(j).p;

                // Bounding
                Profit ub = 0;
                if (ub_type == "dembo") {
                    ub = (mu_ <= c)?
                        ub_dembo(instance, b, pi_, c-mu_):
                        ub_dembo_rev(instance, b, pi_, c-mu_);
                } else if (ub_type == "trivial") {
                    ub = (mu_ <= c)?
                        ub_dembo(instance, t+1, pi_, c-mu_):
                        ub_dembo_rev(instance, j-1, pi_, c-mu_);
                } else if (ub_type == "dantzig"){
                    assert(instance.sort_type() == "eff");
                    ub = (mu_ <= c)?
                        ub_dantzig_from(instance, t+1, pi_, c-mu_):
                        ub_dantzig_from_rev(instance, j-1, pi_, c-mu_);
                } else {
                    assert(false);
                }
                DBG(std::cout << " LB " << z << " UBTMP " << ub << " UB " << u;)
                if (ub <= z || ub > u) {
                    DBG(std::cout << " X" << std::endl;)
                    continue;
                } else {
                    DBG(std::cout << " OK" << std::endl;)
                }

                auto res = maps[k].insert({{mu_, pi_, t}, {j, 0, s}});
                // If (mu_,pi_,t) existed and should be updated
                if (!res.second && res.first->second.a < j) {
                    res.first->second.a    = j;
                    res.first->second.pred = s;
                }
            }
            s->second.a_prec = s->second.a;
        }
        DBG(std::cout << std::endl;)

        DBG(for (auto s = maps[k].begin(); s != maps[k].end(); ++s)
            std::cout << *s << " ";
        std::cout << std::endl;)
    }

    StateIdx map_size = 0;
    for (ItemPos k=0; k<=n-b; ++k)
        map_size += maps[k].size();

    // Get optimal value
    DBG(std::cout << "FIND OPT..." << std::endl;)
    Profit opt = z;
    auto s = maps[n-b].end();
    for (auto st = maps[n-b].begin(); st != maps[n-b].end(); ++st) {
        if (st->first.mu <= c && st->first.pi > opt) {
            opt = st->first.pi;
            s = st;
        }
    }
    if (opt == z)
        return Solution(instance);
    opt += p0;
    assert(instance.check_opt(opt));

    // Retrieve optimal solution
    DBG(std::cout << "RETRIEVE SOL..." << std::endl;)
    Solution sol = sol_break(instance);
    DBG(std::cout << "p(S) " << sol.profit() << std::endl;)
    DBG(std::cout << "wbar " << w_bar << " pbar " << p_bar << " b " << instance.item(b) << std::endl;)
    DBG(auto s_tmp = s;
        while (s_tmp != s_tmp->second.pred) {
        std::cout << *s_tmp << std::endl;
        s_tmp = s_tmp->second.pred;
    })

    ItemPos t = n-1;
    ItemPos a = s->second.a;
    while (!(sol.profit() == opt && sol.remaining_capacity() >= 0)) {
        auto s_next = s->second.pred;
        DBG(std::cout << "s " << *s << " s_next " << *s_next << std::endl;)

        if (s_next->first.pi < s->first.pi) {
            while (s->first.mu != s_next->first.mu + instance.item(t).w
                    || s->first.pi != s_next->first.pi + instance.item(t).p) {
                t--;
                DBG(std::cout << "t " << t << " " << instance.item(t);)
                assert(t >= b - 1);
            }
            sol.set(t, true);
            DBG(std::cout << "ADD " << t << " p(S) " << sol.profit() << std::endl;)
            t--;
            DBG(std::cout << "t " << t << instance.item(t) << " ";)
            assert(t >= b - 1);
        } else if (s_next->first.pi > s->first.pi) {
            while (s->first.mu + instance.item(a).w != s_next->first.mu
                    || s->first.pi + instance.item(a).p != s_next->first.pi) {
                a++;
                DBG(std::cout << "a " << a << " ";)
                assert(a <= b);
            }
            sol.set(a, false);
            DBG(std::cout << "REMOVE " << s->second.a << " p(S) " << sol.profit() << std::endl;)
            a++;
            DBG(std::cout << "a " << a << " ";)
            assert(a <= b);
        }

        s = s_next;
    }

    assert(instance.check_sopt(sol));
    DBG(std::cout << "BALKNAPLISTSOL... END" << std::endl;);
    return sol;
}

#undef DBG

*/
