#include "balknap.hpp"

#include "../lb_greedy/greedy.hpp"
#include "../ub_dantzig/dantzig.hpp"

#include <map>

#define IDX2(k,w,p) rl2*(k) + rl1*(w) + (p)
#define IDX1(  w,p)           rl1*(w) + (p)

#define DBG(x)
//#define DBG(x) x

Weight max_weight(const Instance& instance)
{
    // Compute weight max
    Weight w_max = -1;
    for (ItemPos i=0; i<instance.item_number(); ++i)
        if (instance.item(i).w > w_max)
            w_max = instance.item(i).w;
    DBG(std::cout << "w_max " << w_max << std::endl;)
    return w_max;
}

Profit opt_balknap(const Instance& instance,
        Profit lb, std::string ub_type, Info* info)
{
    DBG(std::cout << "BALKNAPOPT..." << std::endl;)
    DBG(std::cout << instance << std::endl;)
    assert(instance.sort_type() == "eff" || instance.sort_type() == "peff");

    Weight  c = instance.capacity();
    ItemPos n = instance.item_number();
    Profit p0 = instance.reduced_solution()->profit();
    ItemPos b = instance.break_item();

    // Trivial cases
    if (n == 0) {
        return p0;
    } else if (n == 1) {
        return p0 + instance.item(0).p;
    } else if (b == n) { // all items are in the break solution
        return p0 + instance.break_profit();
    }

    Weight w_max = max_weight(instance);
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
    DBG(std::cout << "b " << b << " pb " << pb << " wb " << wb << std::endl;)
    DBG(std::cout << "p_bar " << p_bar << " w_bar " << w_bar << std::endl;)
    DBG(std::cout << "z " << z << " u " << u << std::endl;)

    if (Info::verbose(info))
        std::cout
            <<  "LB " << z
            << " UB " << u
            << " GAP " << u - z << std::endl;

    if (z == u) // If UB == LB, then stop
        return z;

    // Create memory table
    DBG(std::cout << "CREATE TABLE..." << std::endl;)
    Profit rl1 = u - z;
    StateIdx rl2 = rl1 * 2 * w_max;
    DBG(std::cout << "rl1 " << rl1 << " rl2 " << rl2 << std::endl;)
    if (Info::verbose(info))
        std::cout << "Memory " << (double)((rl2 * 2) * sizeof(ItemPos)) / 1000000000 << std::endl;
    std::vector<ItemPos> s0(rl2);
    std::vector<ItemPos> s1(rl2);

    // Initialization
    DBG(std::cout << "INITIALIZATION..." << std::endl;)
    for (Weight mu=c-w_max+1; mu<=c; ++mu) { // s(mu,pi) = 0 for mu <= c
        Weight w = mu + w_max - c - 1;
        Profit x = ((c - mu) * pb) / wb;
        x = (x >= 0)? x: x-1;
        for (Profit pi=z+1-x; pi<=u-x; ++pi) {
            Profit p = pi + x - z - 1;
            s0[IDX1(w,p)] = -1;
        }
    }
    for (Weight mu=c+1; mu<=c+w_max; ++mu) { // s(mu,pi) = 1 for mu > c
        Weight w = mu + w_max - c - 1;
        Profit x = ((c - mu) * pb) / wb;
        if (c < mu)
            x -= 1;
        for (Profit pi=z+1-x; pi<=u-x; ++pi) {
            Profit p = pi + x - z - 1;
            s0[IDX1(w,p)] = 0;
        }
    }
    Profit w_tmp = w_bar + w_max - 1 - c; // s(w_bar,p_bar) = b
    Profit x_tmp = ((c - w_bar) * pb) / wb;
    Profit p_tmp = p_bar + x_tmp - z - 1;
    s0[IDX1(w_tmp,p_tmp)] = b;

    DBG(std::cout << "RECURSION..." << std::endl;)
    for (ItemPos t=b; t<n; ++t) { // Recursion
        Weight wt = instance.item(t).w;
        Profit pt = instance.item(t).p;
        DBG(std::cout << "t " << t << " " << instance.item(t) << std::endl;)

        // Copy previous iteration table
        for (Weight mu=c-w_max+1; mu<=c+w_max; ++mu) {
            Weight w = mu + w_max - c - 1;
            Profit x = ((c - mu) * pb) / wb;
            if (c < mu)
                x -= 1;
            for (Profit pi=z+1-x; pi<=u-x; ++pi) {
                Profit p = pi + x - z - 1;
                s1[IDX1(w,p)] = s0[IDX1(w,p)];
            }
        }

        // Add item t
        for (Weight mu=c-w_max+1; mu<=c; ++mu) {
            DBG(std::cout << " + mu " << mu;)
            Weight mu_ = mu + wt;
            Weight w   = mu  + w_max - c - 1;
            Weight w_  = mu_ + w_max - c - 1;
            assert(w_ < 2 * w_max);

            Profit x   = ((c - mu)  * pb) / wb;
            Profit x_  = ((c - mu_) * pb) / wb;
            if (c < mu)
                x -= 1;
            if (c < mu_)
                x_ -= 1;
            Profit inf = z + 1 - ((x <= x_ + pt)? x: x_ + pt);
            Profit sup = u     - ((x >= x_ + pt)? x: x_ + pt);
            DBG(std::cout << " INF " << inf << " SUP " << sup << std::endl;)

            for (Profit pi=inf; pi<=sup; ++pi) {
                DBG(std::cout << "  pi " << pi;)
                Profit pi_ = pi + pt;
                DBG(std::cout << " pi_ " << pi_;)
                Profit p  = pi  + x  - z - 1;
                Profit p_ = pi_ + x_ - z - 1;
                DBG(std::cout << " " << s0[IDX1(w,p)]);
                if (s1[IDX1(w_,p_)] < s0[IDX1(w,p)]) {
                    DBG(std::cout << " OK" << std::endl;)
                    s1[IDX1(w_,p_)] = s0[IDX1(w,p)];
                } else {
                    DBG(std::cout <<  " X" << std::endl;)
                }
            }
        }

        // Remove previously added items
        for (Weight mu=c+wt; mu>c; --mu) {
            DBG(std::cout << " - mu " << mu << std::endl;)
            Weight w = mu + w_max - c - 1;
            Profit x = ((c - mu) * pb) / wb;
            if (c < mu)
                x -= 1;
            for (Profit pi=z+1-x; pi<=u-x; ++pi) {
                DBG(std::cout << "  pi " << pi << std::endl;)
                Profit p = pi + x - z - 1;
                for (ItemPos j=s0[IDX1(w,p)]; j<s1[IDX1(w,p)]; ++j) {
                    DBG(std::cout << "    j " << j;)
                    Weight mu_ = mu - instance.item(j).w;
                    Profit pi_ = pi - instance.item(j).p;
                    Profit x_  = ((c - mu_) * pb) / wb;
                    if (c < mu_)
                        x_ -= 1;
                    if (pi_ < z + 1 - x_ || pi_ > u - x_)
                        continue;
                    Weight w_  = mu_ + w_max - 1 - c;
                    Profit p_  = pi_ + x_ - z - 1;
                    if (s1[IDX1(w_,p_)] < j) {
                        DBG(std::cout << " OK" << std::endl;)
                        s1[IDX1(w_,p_)] = j;
                    } else {
                        DBG(std::cout << " X" << std::endl;)
                    }
                }
            }
        }
        DBG(std::cout << std::endl;)

        s1.swap(s0); // Swap pointers
    }

    // Get optimal value
    DBG(std::cout << "FIND OPT..." << std::endl;)
    Profit opt = z;
    for (Weight mu=c-w_max+1; mu<=c; ++mu) {
        Weight w = mu + w_max - c - 1;
        Profit x = ((c - mu) * pb) / wb;
        if (c < mu)
            x -= 1;
        for (Profit pi=z+1-x; pi<=u-x; ++pi) {
            Profit p = pi + x - z - 1;
            if (s0[IDX1(w,p)] >= 0 && pi > opt)
                opt = pi;
        }
    }
    opt += p0;
    assert(instance.check_opt(opt));
    DBG(std::cout << "BALKNAPOPT... END" << std::endl;)
    return opt;
}

#undef DBG

/******************************************************************************/

#define DBG(x)
//#define DBG(x) x

Solution sopt_balknap(const Instance& instance,
        Profit lb, std::string ub_type, Info* info)
{
    DBG(std::cout << "BALKNAPSOL..." << std::endl;);
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

    Weight w_max = max_weight(instance);
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
    DBG(std::cout << "b " << b << " pb " << pb << " wb " << wb << std::endl;)
    DBG(std::cout << "p_bar " << p_bar << " w_bar " << w_bar << std::endl;)
    DBG(std::cout << "z " << z << " u " << u << std::endl;)

    if (Info::verbose(info))
        std::cout
            <<  "LB " << z
            << " UB " << u
            << " GAP " << u - z << std::endl;

    if (z == u) // If UB == LB, then stop
        return Solution(instance);

    // Create memory table
    DBG(std::cout << "CREATE TABLE..." << std::endl;)
    Profit rl1 = u - z;
    StateIdx rl2 = rl1 * 2 * w_max;
    DBG(std::cout << "rl1 " << rl1 << " rl2 " << rl2 << std::endl;)
    if (Info::verbose(info))
        std::cout << "Memory " << (double)(((n-b+2) * rl2 * 2) * sizeof(ItemPos)) / 1000000000 << std::endl;
    std::vector<ItemPos> s   ((n-b+2)*rl2);
    std::vector<ItemPos> pred((n-b+2)*rl2);

    // Initialization
    DBG(std::cout << "INITIALIZATION..." << std::endl;)
    for (Weight mu=c-w_max+1; mu<=c; ++mu) { // s(mu,pi) = 0 for mu <= c
        Weight w = mu + w_max - c - 1;
        Profit x = ((c - mu) * pb) / wb;
        x = (x >= 0)? x: x-1;
        for (Profit pi=z+1-x; pi<=u-x; ++pi) {
            Profit p = pi + x - z - 1;
            s[IDX2(0,w,p)] = -1;
        }
    }
    for (Weight mu=c+1; mu<=c+w_max; ++mu) { // s(mu,pi) = 1 for mu > c
        Weight w = mu + w_max - c - 1;
        Profit x = ((c - mu) * pb) / wb;
        if (c < mu)
            x -= 1;
        for (Profit pi=z+1-x; pi<=u-x; ++pi) {
            Profit p = pi + x - z - 1;
            s[IDX2(0,w,p)] = 0;
        }
    }
    Profit w_tmp = w_bar + w_max - 1 - c; // s(w_bar,p_bar) = b
    Profit x_tmp = ((c - w_bar) * pb) / wb;
    Profit p_tmp = p_bar + x_tmp - z - 1;
    s[IDX2(0,w_tmp,p_tmp)] = b;

    DBG(std::cout << "RECURSION..." << std::endl;)
    for (ItemPos t=b; t<n; ++t) { // Recursion
        DBG(std::cout << "t " << t << " " << instance.item(t) << std::endl;)
        ItemPos k = t - b + 1;
        Weight wt = instance.item(t).w;
        Profit pt = instance.item(t).p;

        // Copy previous iteration table
        for (Weight mu=c-w_max+1; mu<=c+w_max; ++mu) {
            Weight w = mu + w_max - c - 1;
            Profit x = ((c - mu) * pb) / wb;
            if (c < mu)
                x -= 1;
            for (Profit pi=z+1-x; pi<=u-x; ++pi) {
                Profit p = pi + x - z - 1;
                s[IDX2(k,w,p)] = s[IDX2(k-1,w,p)];
                pred[IDX2(k,w,p)] = IDX2(k-1,w,p);
            }
        }

        // Add item t
        for (Weight mu=c-w_max+1; mu<=c; ++mu) {
            Weight mu_ = mu + wt;
            Weight w   = mu  + w_max - c - 1;
            Weight w_  = mu_ + w_max - c - 1;
            Profit x   = ((c - mu)  * pb) / wb;
            Profit x_  = ((c - mu_) * pb) / wb;
            if (c < mu)
                x -= 1;
            if (c < mu_)
                x_ -= 1;
            Profit inf = z + 1 - ((x <= x_ + pt)? x: x_ + pt);
            Profit sup = u     - ((x >= x_ + pt)? x: x_ + pt);
            assert(w_ < 2 * w_max);
            for (Profit pi=inf; pi<=sup; ++pi) {
                Profit pi_ = pi + pt;
                Profit p  = pi  + x  - z - 1;
                Profit p_ = pi_ + x_ - z - 1;
                if (s[IDX2(k,w_,p_)] < s[IDX2(k-1,w,p)]) {
                    s[IDX2(k,w_,p_)] = s[IDX2(k-1,w,p)];
                    pred[IDX2(k,w_,p_)] = IDX2(k-1,w,p);
                }
            }
        }

        // Remove previously added items
        for (Weight mu=c+wt; mu>c; --mu) {
            Weight w = mu + w_max - c - 1;
            Profit x = ((c - mu) * pb) / wb;
            if (c < mu)
                x -= 1;
            for (Profit pi=z+1-x; pi<=u-x; ++pi) {
                Profit p = pi + x - z - 1;
                for (ItemPos j=s[IDX2(k-1,w,p)]; j<s[IDX2(k,w,p)]; ++j) {
                    Weight mu_ = mu - instance.item(j).w;
                    Profit pi_ = pi - instance.item(j).p;
                    Profit x_  = ((c - mu_) * pb) / wb;
                    if (c < mu_)
                        x_ -= 1;
                    if (pi_ < z + 1 - x_ || pi_ > u - x_)
                        continue;
                    Weight w_  = mu_ + w_max - 1 - c;
                    Profit p_  = pi_ + x_ - z - 1;
                    if (s[IDX2(k,w_,p_)] < j) {
                        s[IDX2(k,w_,p_)] = j;
                        pred[IDX2(k,w_,p_)] = IDX2(k,w,p);
                    }
                }
            }
        }
    }

    // Get optimal value
    DBG(std::cout << "FIND OPT..." << std::endl;)
    Profit opt = z;
    StateIdx idx_opt = 0;
    for (Weight mu=c-w_max+1; mu<=c; ++mu) {
        Weight w = mu + w_max - c - 1;
        Profit x = ((c - mu) * pb) / wb;
        if (c < mu)
            x -= 1;
        for (Profit pi=z+1-x; pi<=u-x; ++pi) {
            Profit p = pi + x - z - 1;
            if (s[IDX2(n-b,w,p)] >= 0 && pi > opt) {
                opt = pi;
                idx_opt = IDX2(n-b,w,p);
            }
        }
    }
    DBG(std::cout << "OPT " << opt << std::endl;)
    if (opt <= z)
        return Solution(instance);

    // Retrieve optimal solution
    DBG(std::cout << "Retrieve optimal solution..." << std::endl;)
    Solution sol = sol_break(instance);
    DBG(std::cout << "Profit curr " << sol_curr.profit() << std::endl;)

    StateIdx idx = idx_opt;
    ItemPos  k   = idx / rl2;
    ItemPos  t   = k + b - 1;
    Weight   w   = (idx % rl2) / rl1;
    Weight   mu  = c + 1 + w - w_max;
    Profit   p   = (idx % rl2) % rl1;
    Profit   x   = ((c - mu) * pb) / wb;
    if (c < mu)
        x -= 1;
    Profit pi = p + z + 1 - x;
    DBG(std::cout << "t " << t << " wt " << instance.weight(t) << " pt " << instance.profit(t) << " mu " << mu << " pi " << pi << std::endl;)

    while (!(sol.profit() == opt + p0 && sol.remaining_capacity() >= 0)) {
        StateIdx idx_next = pred[idx];
        ItemPos  k_next   = idx_next / rl2;
        ItemPos  t_next   = k_next + b - 1;
        Weight   w_next   = (idx_next % rl2) / rl1;
        Weight   mu_next  = c + 1 + w_next - w_max;
        Profit   p_next   = (idx_next % rl2) % rl1;
        Profit   x_next   = ((c - mu_next) * pb) / wb;
        if (c < mu_next)
            x_next -= 1;
        Profit pi_next = p_next + z + 1 - x_next;
        //DBG(std::cout << "t " << t_next << " wt " << instance.weight(t_next) << " pt " << instance.profit(t_next) << " mu " << mu_next << " pi " << pi_next << std::endl;)

        if (k_next < k && pi_next < pi) {
            sol.set(t, true);
            DBG(std::cout << "Add " << t << " profit " << sol_curr.profit() << std::endl;)
            assert(mu_next == mu - instance.item(t).w);
            assert(pi_next == pi - instance.item(t).p);
        }
        if (k_next == k) {
            sol.set(s[idx], false);
            DBG(std::cout << "Remove " << s[idx] << " profit " << sol_curr.profit() << std::endl;)
        }

        idx = idx_next;
        k   = k_next;
        t   = t_next;
        w   = w_next;
        mu  = mu_next;
        p   = p_next;
        pi  = pi_next;
    }

    assert(instance.check_sopt(sol));
    DBG(std::cout << "BALKNAPSOL... END" << std::endl;)
    return sol;
}

#undef DBG

/******************************************************************************/

#define DBG(x)
//#define DBG(x) x

struct State1
{
    Weight mu;
    Profit pi;
    bool operator()(const State1& s1, const State1& s2)
    {
        if (s1.mu != s2.mu)
            return s1.mu < s2.mu;
        if (s1.pi != s2.pi)
            return s1.pi < s2.pi;
        return false;
    }
};

struct StateValue1
{
    ItemPos a;
    ItemPos a_prec;
};

std::ostream& operator<<(std::ostream& os, const std::pair<State1, StateValue1>& s)
{
    os << "(mu " << s.first.mu << " pi " << s.first.pi << " a " << s.second.a << " ap " << s.second.a_prec << ")";
    return os;
}

Profit opt_balknap_list(const Instance& instance, Profit lb,
        std::string ub_type, Info* info)
{
    DBG(std::cout << "BALKNAPLISTOPT..." << std::endl;)
    DBG(std::cout << instance << std::endl;)
    assert(instance.sort_type() == "eff" || instance.sort_type() == "peff");

    Weight  c = instance.capacity();
    ItemPos n = instance.item_number();
    Profit p0 = instance.reduced_solution()->profit();
    ItemPos b = instance.break_item();

    // Trivial cases
    if (n == 0) {
        return p0;
    } else if (n == 1) {
        return p0 + instance.item(0).p;
    } else if (b == n) { // all items are in the break solution
        return p0 + instance.break_profit();
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
    DBG(std::cout << "b " << b << " pb " << pb << " wb " << wb << std::endl;)
    DBG(std::cout << "p_bar " << p_bar << " w_bar " << w_bar << std::endl;)
    DBG(std::cout << "z " << z << " u " << u << std::endl;)

    if (Info::verbose(info))
        std::cout
            <<  "LB " << z
            << " UB " << u
            << " GAP " << u - z << std::endl;

    if (z == u) // If UB == LB, then stop
        return z;

    // Create memory table
    std::map<State1, StateValue1, State1> map;

    // Initialization
    map.insert({{w_bar,p_bar},{b,b}}); // s(w_bar,p_bar) = b

    DBG(for (auto s = map.begin(); s != map.end(); ++s)
        std::cout << *s << " ";
        std::cout << std::endl;)

    DBG(std::cout << "RECURSION..." << std::endl;)
    for (ItemPos t=b; t<n; ++t) { // Recursion
        DBG(std::cout << "t " << t << " " << instance.item(t) << std::endl;)
        Weight wt = instance.item(t).w;
        Profit pt = instance.item(t).p;

        // Add item t
        auto s = map.upper_bound({c+1,0});
        auto hint = s;
        hint--;
        while (s != map.begin() && (--s)->first.mu <= c) {
            DBG(std::cout << " + STATE " << *s << " ";)
            Weight mu_ = s->first.mu + wt;
            Weight pi_ = s->first.pi + pt;

            // Bounding
            Profit ub = 0;
            if (ub_type == "dembo") {
                ub = (mu_ <= c)?
                    ub_trivial_from(instance, b, pi_, c-mu_):
                    ub_trivial_from_rev(instance, b, pi_, c-mu_);
            } else if (ub_type == "trivial") {
                ub = (mu_ <= c)?
                    ub_trivial_from(instance, t, pi_, c-mu_):
                    ub_trivial_from_rev(instance, s->second.a-1, pi_, c-mu_);
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

            hint = map.insert(hint, {{mu_, pi_}, {s->second.a, 0}});
            hint--;
        }

        // Remove previously added items
        for (auto s = map.rbegin(); s->first.mu > c; ++s) {
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
                        ub_trivial_from(instance, b, pi_, c-mu_):
                        ub_trivial_from_rev(instance, b, pi_, c-mu_);
                } else if (ub_type == "trivial") {
                    ub = (mu_ <= c)?
                        ub_trivial_from(instance, t+1, pi_, c-mu_):
                        ub_trivial_from_rev(instance, j-1, pi_, c-mu_);
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

                auto res = map.insert({{mu_,pi_},{j, 0}});
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

    // Get optimal value
    Profit opt = z;
    for (auto& s: map)
        if (s.first.mu <= c && s.first.pi > opt)
            opt = s.first.pi;
    opt += p0;
    assert(instance.check_opt(opt));
    DBG(std::cout << "BALKNAPLISTOPT... END" << std::endl;)
    return opt;
}

#undef DBG

/******************************************************************************/

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
    DBG(std::cout << "b " << b << " pb " << pb << " wb " << wb << std::endl;)
    DBG(std::cout << "p_bar " << p_bar << " w_bar " << w_bar << std::endl;)
    DBG(std::cout << "z " << z << " u " << u << std::endl;)

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

        //DBG(std::cout << "-------- Memory --------" << std::endl;
        //for (ItemPos kk = 0; kk <= k; ++kk) {
        //std::cout << "t " << kk + b - 1 << ": " << std::flush;
        //for (auto s = maps[kk].begin(); s != maps[kk].end(); ++s)
        //std::cout << s << " ";
        //std::cout << std::endl;
        //}
        //std::cout << "------------------------" << std::endl;)

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
                    ub_trivial_from(instance, b, pi_, c-mu_):
                    ub_trivial_from_rev(instance, b, pi_, c-mu_);
            } else if (ub_type == "trivial") {
                ub = (mu_ <= c)?
                    ub_trivial_from(instance, t, pi_, c-mu_):
                    ub_trivial_from_rev(instance, s->second.a-1, pi_, c-mu_);
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

        //DBG(std::cout << "-------- Memory --------" << std::endl;
        //for (ItemPos kk = 0; kk <= k; ++kk) {
        //std::cout << "t " << kk + b - 1 << ": " << std::flush;
        //for (auto s = maps[kk].begin(); s != maps[kk].end(); ++s)
        //std::cout << s << " ";
        //std::cout << std::endl;
        //}
        //std::cout << "------------------------" << std::endl;)

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
                        ub_trivial_from(instance, b, pi_, c-mu_):
                        ub_trivial_from_rev(instance, b, pi_, c-mu_);
                } else if (ub_type == "trivial") {
                    ub = (mu_ <= c)?
                        ub_trivial_from(instance, t+1, pi_, c-mu_):
                        ub_trivial_from_rev(instance, j-1, pi_, c-mu_);
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

    // Retrieve optimal solution
    DBG(std::cout << "RETRIEVE SOL..." << std::endl;)
    Solution sol = sol_break(instance);
    DBG(std::cout << "p(S) " << sol.profit() << std::endl;)
    DBG(std::cout << "w_bar " << w_bar << " p_bar " << p_bar << " b " << b << std::endl;)
    DBG(auto s_tmp = s;
        while (s_tmp != s_tmp->second.pred) {
        std::cout << *s_tmp << std::endl;
        s_tmp = s_tmp->second.pred;
    })

    ItemPos t = n-1;
    ItemPos a = s->second.a;
    while (!(sol.profit() == opt + p0 && sol.remaining_capacity() >= 0)) {
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
