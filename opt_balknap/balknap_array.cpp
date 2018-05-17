#include "knapsack/opt_balknap/balknap.hpp"

#include "knapsack/lb_greedy/greedy.hpp"
#include "knapsack/lb_greedynlogn/greedynlogn.hpp"
#include "knapsack/ub_dembo/dembo.hpp"
#include "knapsack/ub_dantzig/dantzig.hpp"
#include "knapsack/ub_surrogate/surrogate.hpp"

using namespace knapsack;

#define IDX2(k,w,p) rl2*(k) + rl1*(w) + (p)
#define IDX1(  w,p)           rl1*(w) + (p)

#define DBG(x)
//#define DBG(x) x

#define ALPHA(mu) (mu <= c)? z + 1 - ((c - mu) * pb) / wb: z + 1 + ((mu - c) * pb) / wb
#define BETA(mu) (mu >= w_bar)? p_bar + ((mu - w_bar) * pb) / wb: p_bar - ((w_bar - mu) * pb) / wb

Profit knapsack::opt_balknap_array(Instance& ins, BalknapParams p, Info* info)
{
    DBG(std::cout << "BALKNAPOPT..." << std::endl;)
    DBG(std::cout << ins << std::endl;)

    ins.sort_partially();
    if (ins.break_item() == ins.last_item()+1) { // all items are in the break solution
        std::cout << ins << std::endl;
        DBG(std::cout << "BALKNAPSOL... END ALL ITEMS" << std::endl;)
        return ins.break_profit();
    }

    DBG(std::cout << "LB... " << std::flush;)
    Profit lb = 0;
    if (p.lb_greedy == 0) {
        lb = sol_greedy(ins).profit();
    } else {
        lb = ins.break_solution()->profit();
    }
    DBG(std::cout << lb << std::endl;)

    DBG(std::cout << "REDUCTION..." << std::endl;)
    ins.reduce1(lb, Info::verbose(info));
    if (ins.capacity() < 0) {
        DBG(std::cout << "BALKNAPSOL... END SOLRED OPT" << std::endl;)
        return lb;
    }
    DBG(std::cout << ins << std::endl;)
    Weight  c = ins.capacity();
    ItemPos f = ins.first_item();
    ItemPos l = ins.last_item();
    ItemPos n = ins.item_number();
    Profit p0 = ins.reduced_solution()->profit();

    // Trivial cases
    if (n == 0 || c == 0) {
        DBG(std::cout << "BALKNAPSOL... END EMPTY INSTANCE" << std::endl;)
        return std::max(lb, p0);
    } else if (n == 1) {
        DBG(std::cout << "BALKNAPSOL... END 1 ITEM INSTANCE" << std::endl;)
        return std::max(lb, p0 + ins.item(f).p);
    } else if (ins.break_item() == ins.last_item()+1) {
        DBG(std::cout << "BALKNAPSOL... END NO BREAK ITEM" << std::endl;)
        return std::max(lb, ins.break_solution()->profit());
    }

    ItemPos b = ins.break_item();
    Weight w_max = ins.max_weight_item().w;
    Weight w_bar = ins.break_weight();
    Profit p_bar = ins.break_profit();
    Weight wb    = ins.item(b).w;
    Profit pb    = ins.item(b).p;
    Weight r     = ins.break_capacity();
    Profit z     = lb - p0;
    Profit u     = p_bar + r * pb / wb;

    DBG(std::cout << "N " << n << " C " << c << std::endl;)
    DBG(std::cout << " F " << f << " L " << l << std::endl;)
    DBG(std::cout << " B " << ins.item(b) << std::endl;)
    DBG(std::cout << "PBAR " << p_bar << " WBAR " << w_bar << std::endl;)

    if (Info::verbose(info))
        std::cout
            <<  "LB " << z
            << " UB " << u
            << " GAP " << u - z << std::endl;

    if (z >= u) { // If UB == LB, then stop
        DBG(std::cout << "BALKNAPOPT... END Z == U" << std::endl;)
        return lb;
    }

    // Create memory table
    DBG(std::cout << "CREATE TABLE..." << std::endl;)
    Profit rl1 = u - z + 1;
    StateIdx rl2 = rl1 * 2 * w_max;
    DBG(std::cout << "RL1 " << rl1 << " RL2 " << rl2 << std::endl;)
    if (Info::verbose(info))
        std::cout << "MEMORY " << (double)((rl2 * 2) * sizeof(ItemPos)) / 1000000000 << std::endl;
    std::vector<ItemPos> s0(rl2);
    std::vector<ItemPos> s1(rl2);

    // Initialization
    DBG(std::cout << "INITIALIZATION..." << std::endl;)
    for (Weight mu=c-w_max+1; mu<=c; ++mu) { // s(mu,pi) = 0 for mu <= c
        Weight w = mu + w_max - c - 1;
        Profit alpha = ALPHA(mu);
        Profit beta  = BETA(mu);
        for (Profit pi=alpha; pi<=beta; ++pi) {
            Profit p = pi - alpha;
            s0[IDX1(w,p)] = f-1;
        }
    }
    for (Weight mu=c+1; mu<=c+w_max; ++mu) { // s(mu,pi) = 1 for mu > c
        Weight w = mu + w_max - c - 1;
        Profit alpha = ALPHA(mu);
        Profit beta  = BETA(mu);
        for (Profit pi=alpha; pi<=beta; ++pi) {
            Profit p = pi - alpha;
            s0[IDX1(w,p)] = f;
        }
    }
    Profit w_tmp = w_bar + w_max - 1 - c; // s(w_bar,p_bar) = b
    Profit x_tmp = ((c - w_bar) * pb) / wb;
    Profit p_tmp = p_bar + x_tmp - z - 1;
    s0[IDX1(w_tmp,p_tmp)] = b;

    DBG(std::cout << "RECURSION..." << std::endl;)
    for (ItemPos t=b; t<=l; ++t) { // Recursion
        Weight wt = ins.item(t).w;
        Profit pt = ins.item(t).p;
        DBG(std::cout << "t " << t << " " << ins.item(t) << std::endl;)

        // Copy previous iteration table
        for (Weight mu=c-w_max+1; mu<=c+w_max; ++mu) {
            Weight w = mu + w_max - c - 1;
            Profit alpha = ALPHA(mu);
            Profit beta  = BETA(mu);
            for (Profit pi=alpha; pi<=beta; ++pi) {
                Profit p = pi - alpha;
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

            Profit alpha = ALPHA(mu);
            Profit beta  = BETA(mu);
            Profit alpha_ = ALPHA(mu_);
            Profit beta_  = BETA(mu_);
            DBG(std::cout << " ALPHA " << alpha << " BETA " << beta << std::endl;)

            //for (Profit pi=inf; pi<=sup; ++pi) {
            for (Profit pi=alpha; pi<=beta; ++pi) {
                DBG(std::cout << "  pi " << pi;)
                Profit pi_ = pi + pt;
                DBG(std::cout << " mu_ " << mu_;)
                DBG(std::cout << " pi_ " << pi_;)
                if (pi_ < alpha_) {
                    std::cout << std::endl;
                    continue;
                }
                if (pi_ > beta_) {
                    std::cout << std::endl;
                    break;
                }
                Profit p  = pi  - alpha;
                Profit p_ = pi_ - alpha_;
                DBG(std::cout << " " << s0[IDX1(w,p)]);
                if (s1[IDX1(w_,p_)] < s0[IDX1(w,p)]) {
                    DBG(std::cout << " OK " << s0[IDX1(w,p)] << std::endl;)
                    s1[IDX1(w_,p_)] = s0[IDX1(w,p)];
                } else {
                    DBG(std::cout <<  " X" << std::endl;)
                }
            }
        }

        // Remove previously added items
        for (Weight mu=c+wt; mu>c; --mu) {
            Weight w = mu + w_max - c - 1;
            DBG(std::cout << " - mu " << mu << " (" << w << ")" << std::endl;)
            Profit alpha = ALPHA(mu);
            Profit beta  = BETA(mu);
            for (Profit pi=alpha; pi<=beta; ++pi) {
                Profit p = pi - alpha;
                DBG(std::cout << "  pi " << pi << " (" << p << ")" << std::flush;)
                DBG(std::cout << " j0 " << s0[IDX1(w,p)] << " j1 " << s1[IDX1(w,p)] << std::endl;)
                for (ItemPos j=s0[IDX1(w,p)]; j<s1[IDX1(w,p)]; ++j) {
                    DBG(std::cout << "    j " << j;)
                    Weight mu_ = mu - ins.item(j).w;
                    Profit pi_ = pi - ins.item(j).p;
                    Profit alpha_ = ALPHA(mu_);
                    Profit beta_  = BETA(mu_);
                    if (pi_ < alpha_ || pi_ > beta_)
                        continue;
                    Weight w_ = mu_ + w_max - 1 - c;
                    Profit p_ = pi_ - alpha_;
                    DBG(std::cout << " mu_ " << mu_ << " (" << w_ << ")";)
                    DBG(std::cout << " pi_ " << pi_ << " (" << p_ << ")";)
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
        Profit alpha = ALPHA(mu);
        Profit beta  = BETA(mu);
        for (Profit pi=alpha; pi<=beta; ++pi) {
            Profit p = pi - alpha;
            if (s0[IDX1(w,p)] >= f && pi > opt)
                opt = pi;
        }
    }
    opt += p0;
    DBG(std::cout << "OPT " << opt << std::endl;)
    assert(ins.check_opt(opt));
    DBG(std::cout << "BALKNAPOPT... END" << std::endl;)
    return opt;
}

#undef DBG

/******************************************************************************/

#define DBG(x)
//#define DBG(x) x

Solution knapsack::sopt_balknap_array_all(Instance& ins,
        BalknapParams params, Info* info)
{
    DBG(std::cout << "BALKNAPSOL..." << std::endl;);
    DBG(std::cout << ins << std::endl;)

    ins.sort_partially();
    if (ins.break_item() == ins.last_item()+1) { // all items are in the break solution
        DBG(std::cout << "BALKNAPSOL... END ALL ITEMS" << std::endl;)
        return *ins.break_solution();
    }

    DBG(std::cout << "LB..." << std::endl;)
    Solution sol(ins);
    if (params.lb_greedy == 0) {
        sol = sol_greedy(ins);
    } else {
        sol = *ins.break_solution();
    }

    DBG(std::cout << "REDUCTION..." << std::endl;)
    ins.reduce1(sol.profit(), Info::verbose(info));
    if (ins.capacity() < 0) {
        DBG(std::cout << "BALKNAPSOL... END SOLRED OPT" << std::endl;)
        return sol;
    }
    Weight  c = ins.capacity();
    ItemPos f = ins.first_item();
    ItemPos l = ins.last_item();
    ItemPos n = ins.item_number();
    Profit p0 = ins.reduced_solution()->profit();

    // Trivial cases
    if (n == 0 || c == 0) {
        DBG(std::cout << "BALKNAPSOL... END EMPTY INSTANCE" << std::endl;)
        return (ins.reduced_solution()->profit() > sol.profit())?
            *ins.reduced_solution(): sol;
    } else if (n == 1) {
        DBG(std::cout << "BALKNAPSOL... END 1 ITEM INSTANCE" << std::endl;)
        Solution sol1 = *ins.reduced_solution();
        sol1.set(f, true);
        return (sol1.profit() > sol.profit())? sol1: sol;
    } else if (ins.break_item() == ins.last_item()+1) {
        DBG(std::cout << "BALKNAPSOL... END NO BREAK ITEM" << std::endl;)
        return (ins.break_solution()->profit() > sol.profit())?
            *ins.break_solution(): sol;
    }

    ItemPos b = ins.break_item();
    Weight w_max = ins.max_weight_item().w;
    Weight w_bar = ins.break_weight();
    Profit p_bar = ins.break_profit();
    Weight wb    = ins.item(b).w;
    Profit pb    = ins.item(b).p;
    Weight r     = ins.break_capacity();
    Profit z     = sol.profit() - ins.reduced_solution()->profit();
    Profit u     = ins.break_profit() + r * pb / wb;

    DBG(std::cout
            << "N " << n << " C " << c
            << " F " << f << " L " << l
            << " B " << ins.item(b) << std::endl
            << "WMAX " << w_max << " PBAR " << p_bar << " WBAR " << w_bar << std::endl;)

    if (Info::verbose(info))
        std::cout
            <<  "LB " << z
            << " UB " << u
            << " GAP " << u - z << std::endl;

    if (z >= u) { // If UB == LB, then stop
        DBG(std::cout << "BALKNAPSOL... END Z == U" << std::endl;)
        return sol;
    }

    // Create memory table
    DBG(std::cout << "CREATE TABLE..." << std::endl;)
    Profit rl1 = u - z + 1;
    StateIdx rl2 = rl1 * 2 * w_max;
    DBG(std::cout << "RL1 " << rl1 << " RL2 " << rl2 << std::endl;)
    if (Info::verbose(info))
        std::cout << "MEMORY " << (double)(((l-b+2) * rl2 * 2) * sizeof(ItemPos)) / 1000000000 << std::endl;
    std::vector<ItemPos> s   ((l-b+2)*rl2);
    std::vector<ItemPos> pred((l-b+2)*rl2);

    // Initialization
    DBG(std::cout << "INITIALIZATION..." << std::endl;)
    for (Weight mu=c-w_max+1; mu<=c; ++mu) { // s(mu,pi) = 0 for mu <= c
        Weight w = mu + w_max - c - 1;
        Profit alpha = ALPHA(mu);
        Profit beta  = BETA(mu);
        for (Profit pi=alpha; pi<=beta; ++pi) {
            Profit p = pi - alpha;
            s[IDX2(0,w,p)] = f-1;
        }
    }
    for (Weight mu=c+1; mu<=c+w_max; ++mu) { // s(mu,pi) = 1 for mu > c
        Weight w = mu + w_max - c - 1;
        Profit alpha = ALPHA(mu);
        Profit beta  = BETA(mu);
        for (Profit pi=alpha; pi<=beta; ++pi) {
            Profit p = pi - alpha;
            s[IDX2(0,w,p)] = f;
        }
    }
    Profit w_tmp = w_bar + w_max - 1 - c; // s(w_bar,p_bar) = b
    Profit x_tmp = ((c - w_bar) * pb) / wb;
    Profit p_tmp = p_bar + x_tmp - z - 1;
    s[IDX2(0,w_tmp,p_tmp)] = b;

    DBG(std::cout << "RECURSION..." << std::endl;)
    for (ItemPos t=b; t<=l; ++t) { // Recursion
        DBG(std::cout << "T " << t << " " << ins.item(t) << std::endl;)
        ItemPos k = t - b + 1;
        Weight wt = ins.item(t).w;
        Profit pt = ins.item(t).p;

        // Copy previous iteration table
        for (Weight mu=c-w_max+1; mu<=c+w_max; ++mu) {
            Weight w = mu + w_max - c - 1;
            Profit alpha = ALPHA(mu);
            Profit beta  = BETA(mu);
            for (Profit pi=alpha; pi<=beta; ++pi) {
                Profit p = pi - alpha;
                s[IDX2(k,w,p)] = s[IDX2(k-1,w,p)];
                pred[IDX2(k,w,p)] = IDX2(k-1,w,p);
            }
        }

        // Add item t
        for (Weight mu=c-w_max+1; mu<=c; ++mu) {
            Weight mu_ = mu + wt;
            Weight w   = mu  + w_max - c - 1;
            Weight w_  = mu_ + w_max - c - 1;
            Profit alpha = ALPHA(mu);
            Profit beta  = BETA(mu);
            Profit alpha_ = ALPHA(mu_);
            Profit beta_  = BETA(mu_);
            assert(w_ < 2 * w_max);
            for (Profit pi=alpha; pi<=beta; ++pi) {
                DBG(std::cout << "  pi " << pi;)
                Profit pi_ = pi + pt;
                if (pi_ < alpha_) {
                    std::cout << std::endl;
                    continue;
                }
                if (pi_ > beta_) {
                    std::cout << std::endl;
                    break;
                }
                Profit p  = pi  - alpha;
                Profit p_ = pi_ - alpha_;
                if (s[IDX2(k,w_,p_)] < s[IDX2(k-1,w,p)]) {
                    s[IDX2(k,w_,p_)] = s[IDX2(k-1,w,p)];
                    pred[IDX2(k,w_,p_)] = IDX2(k-1,w,p);
                }
            }
        }

        // Remove previously added items
        for (Weight mu=c+wt; mu>c; --mu) {
            Weight w = mu + w_max - c - 1;
            Profit alpha = ALPHA(mu);
            Profit beta  = BETA(mu);
            for (Profit pi=alpha; pi<=beta; ++pi) {
                Profit p = pi - alpha;
                for (ItemPos j=s[IDX2(k-1,w,p)]; j<s[IDX2(k,w,p)]; ++j) {
                    Weight mu_ = mu - ins.item(j).w;
                    Profit pi_ = pi - ins.item(j).p;
                    Profit alpha_ = ALPHA(mu_);
                    Profit beta_  = BETA(mu_);
                    if (pi_ < alpha_ || pi_ > beta_)
                        continue;
                    Weight w_  = mu_ + w_max - 1 - c;
                    Profit p_ = pi_ - alpha_;
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
        Profit alpha = ALPHA(mu);
        Profit beta  = BETA(mu);
        for (Profit pi=alpha; pi<=beta; ++pi) {
            Profit p = pi - alpha;
            if (s[IDX2(l-b+1,w,p)] >= f && pi > opt) {
                opt = pi;
                idx_opt = IDX2(l-b+1,w,p);
            }
        }
    }
    opt += p0;
    DBG(std::cout << "OPT " << opt << std::endl;)
    if (opt <= sol.profit())
        return sol;
    assert(ins.check_opt(opt));

    // Retrieve optimal solution
    DBG(std::cout << "RETRIEVE SOL..." << std::endl;)
    sol = *ins.break_solution();

    StateIdx idx = idx_opt;
    ItemPos  k   = idx / rl2;
    ItemPos  t   = k + b - 1;
    Weight   w   = (idx % rl2) / rl1;
    Weight   mu  = c + 1 + w - w_max;
    Profit   p   = (idx % rl2) % rl1;
    Profit alpha = ALPHA(mu);
    Profit pi = p + alpha;
    DBG(std::cout << "A " << s[k] << " T " << t << " WT " << ins.item(t).w << " PT " << ins.item(t).p << " MU " << mu << " PI " << pi << std::endl;)

    while (!(sol.profit() == opt && sol.remaining_capacity() >= 0)) {
        DBG(std::cout << "P(S) " << sol.profit() << std::endl;)
        StateIdx idx_next = pred[idx];
        ItemPos  k_next   = idx_next / rl2;
        ItemPos  t_next   = k_next + b - 1;
        Weight   w_next   = (idx_next % rl2) / rl1;
        Weight   mu_next  = c + 1 + w_next - w_max;
        Profit   p_next   = (idx_next % rl2) % rl1;
        Profit   alpha_next = ALPHA(mu_next);
        Profit   pi_next    = p_next + alpha_next;
        DBG(std::cout << "A" << s[k_next] << " T " << t_next << " WT " << ins.item(t_next).w << " PT " << ins.item(t_next).p << " MU " << mu_next << " PI " << pi_next << std::endl;)

        if (k_next < k && pi_next < pi) {
            sol.set(t, true);
            DBG(std::cout << "ADD " << t << " P(S) " << sol.profit() << std::endl;)
            assert(mu_next == mu - ins.item(t).w);
            assert(pi_next == pi - ins.item(t).p);
        }
        if (k_next == k) {
            sol.set(s[idx], false);
            DBG(std::cout << "REMOVE " << s[idx] << " P(S) " << sol.profit() << std::endl;)
        }

        idx = idx_next;
        k   = k_next;
        t   = t_next;
        w   = w_next;
        mu  = mu_next;
        p   = p_next;
        pi  = pi_next;
    }

    assert(ins.check_sopt(sol));
    DBG(std::cout << "BALKNAPSOL... END" << std::endl;)
    return sol;
}

#undef DBG

/******************************************************************************/

Solution knapsack::sopt_balknap_array_part(Instance& ins,
        BalknapParams p, ItemPos k, Info* info)
{
    (void)info;
    (void)p;
    (void)k;
    assert(false); // TODO
    return Solution(ins);
}


