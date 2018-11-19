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

Profit knapsack::opt_balknap_array(Instance& ins, Info& info, BalknapParams p)
{
    DBG(std::cout << "BALKNAPOPT..." << std::endl;)
    DBG(std::cout << ins << std::endl;)

    ins.sort_partially();
    if (ins.break_item() == ins.last_item()+1) { // all items are in the break solution
        DBG(std::cout << "BALKNAPSOL... END ALL ITEMS" << std::endl;)
        return algorithm_end(ins.break_profit(), info);
    }

    DBG(std::cout << "LB... " << std::flush;)
    Profit lb = 0;
    if (p.lb_greedy == 0) {
        Info info_tmp;
        lb = sol_greedy(ins, info_tmp).profit();
    } else {
        lb = ins.break_solution()->profit();
    }
    DBG(std::cout << lb << std::endl;)

    DBG(std::cout << "REDUCTION..." << std::endl;)
    ins.reduce1(lb, info);
    if (ins.capacity() < 0) {
        DBG(std::cout << "BALKNAPSOL... END SOLRED OPT" << std::endl;)
        return algorithm_end(lb, info);
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
        return algorithm_end(std::max(lb, p0), info);
    } else if (n == 1) {
        DBG(std::cout << "BALKNAPSOL... END 1 ITEM INSTANCE" << std::endl;)
        return algorithm_end(std::max(lb, p0 + ins.item(f).p), info);
    } else if (ins.break_item() == ins.last_item()+1) {
        DBG(std::cout << "BALKNAPSOL... END NO BREAK ITEM" << std::endl;)
        return algorithm_end(std::max(lb, ins.break_solution()->profit()), info);
    }

    ItemPos b = ins.break_item();
    Weight w_max = ins.max_weight_item().w;
    Weight w_bar = ins.break_weight();
    Profit p_bar = ins.break_profit();
    Weight wb    = ins.item(b).w;
    Profit pb    = ins.item(b).p;
    Weight r     = ins.break_capacity();

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

    Profit z = lb - p0;
    Profit u = p_bar + r * pb / wb;
    info.verbose(
            "z " + std::to_string(z) +
            " u " + std::to_string(u) +
            " gap " + std::to_string(u - z) +
            "\n");

    if (z >= u) { // If UB == LB, then stop
        DBG(std::cout << "BALKNAPOPT... END Z == U" << std::endl;)
        return algorithm_end(lb, info);
    }

    // Create memory table
    DBG(std::cout << "CREATE TABLE..." << std::endl;)
    Profit rl1 = u - z + 1;
    StateIdx rl2 = rl1 * 2 * w_max;
    DBG(std::cout << "RL1 " << rl1 << " RL2 " << rl2 << std::endl;)
    if (info.verbose())
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
                    DBG(std::cout << std::endl;)
                    continue;
                }
                if (pi_ > beta_) {
                    DBG(std::cout << std::endl;)
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
    return algorithm_end(opt, info);
}

#undef DBG

/******************************************************************************/

Solution knapsack::sopt_balknap_array_all(Instance& ins, Info& info,
        BalknapParams params)
{
    if (info.verbose())
        std::cout << "*** balknap (array, all) ***" << std::endl;

    ins.sort_partially();

    // If all items can fit in the knapsack, then stop
    if (ins.break_item() == ins.last_item()+1) {
        info.debug("All items fit in the knapsack.\n");
        Solution sol = *ins.break_solution();
        return algorithm_end(sol, info);
    }

    // Compute lower bound
    if (info.verbose())
        std::cout << "COMPUTE LOWER BOUND..." << std::flush;
    Solution sol(ins);
    if (params.lb_greedy == 0) {
        Info info_tmp;
        sol = sol_greedy(ins, info_tmp);
    } else {
        sol = *ins.break_solution();
    }
    if (info.verbose())
        std::cout << " " << sol.profit() << std::endl;

    // Variable reduction
    ins.reduce1(sol.profit(), info);
    if (ins.capacity() < 0) {
        info.debug("All items have been reduced.\n");
        return algorithm_end(sol, info);
    }
    info.debug("Reduced solution: " + ins.reduced_solution()->print_bin() + "\n");
    info.debug("Reduced solution: " + ins.reduced_solution()->print_in() + "\n");

    Weight  c = ins.capacity();
    ItemPos f = ins.first_item();
    ItemPos l = ins.last_item();
    ItemPos n = ins.item_number();
    Profit p0 = ins.reduced_solution()->profit();

    // Trivial cases
    if (n == 0 || c == 0) {
        info.debug("Empty instance (after reduction).\n");
        if (ins.reduced_solution()->profit() > sol.profit())
            Solution sol = *ins.reduced_solution();
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
    Weight w_max = ins.max_weight_item().w;
    Weight w_bar = ins.break_weight();
    Profit p_bar = ins.break_profit();
    Weight wb    = ins.item(b).w;
    Profit pb    = ins.item(b).p;
    Weight r     = ins.break_capacity();
    Profit z     = sol.profit() - ins.reduced_solution()->profit();
    Profit u     = ins.break_profit() + r * pb / wb;

    info.debug("Break solution: " + ins.break_solution()->print_bin() + "\n");
    info.debug("Break solution: " + ins.break_solution()->print_in() + "\n");
    info.debug(
            "n " + std::to_string(n) +
            " c " + std::to_string(c) +
            " f " + std::to_string(f) + " l " + std::to_string(l) +
            "\n" +
            "wmax " + std::to_string(w_max) +
            " wbar " + std::to_string(w_bar) +
            " pbar " + std::to_string(p_bar) +
            "\n");

    if (ins.break_solution()->profit() > sol.profit()) {
        info.debug("Update best solution.");
        sol = *ins.break_solution();
    }

    info.verbose(
            "lb " + std::to_string(z) +
            " ub " + std::to_string(u) +
            " gap " + std::to_string(u - z) +
            "\n");

    // If UB == LB, then stop
    if (z >= u) {
        info.debug("Lower bound equals upper bound.");
        return algorithm_end(sol, info);
    }

    // Create memory table
    info.debug("Create memory table.\n");
    Profit rl1 = u - z + 1;
    StateIdx rl2 = rl1 * 2 * w_max;
    info.debug("rl1 " + std::to_string(rl1) + " rl2 " + std::to_string(rl2) + "\n");
    std::vector<ItemPos> s   ((l-b+2)*rl2);
    std::vector<ItemPos> pred((l-b+2)*rl2);
    if (info.verbose())
        std::cout << "Memory: " << (double)(((l-b+2) * rl2 * 2) * sizeof(ItemPos)) / 1000000000 << std::endl;

    // Initialization
    if (info.verbose())
        std::cout << "Recursion..." << std::endl;
    info.debug("Initialization\n");
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

    // Recursion
    info.debug("Recursion\n");
    for (ItemPos t=b; t<=l; ++t) {
        info.debug("t " + std::to_string(ins.item(t).j) + "\n");
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
            info.debug("mu " + std::to_string(mu));
            Weight mu_ = mu + wt;
            Weight w   = mu  + w_max - c - 1;
            Weight w_  = mu_ + w_max - c - 1;
            Profit alpha = ALPHA(mu);
            Profit beta  = BETA(mu);
            Profit alpha_ = ALPHA(mu_);
            Profit beta_  = BETA(mu_);
            assert(w_ < 2 * w_max);
            for (Profit pi=alpha; pi<=beta; ++pi) {
                info.debug("  pi " + std::to_string(pi));
                Profit pi_ = pi + pt;
                if (pi_ < alpha_)
                    continue;
                if (pi_ > beta_)
                    break;
                Profit p  = pi  - alpha;
                Profit p_ = pi_ - alpha_;
                if (s[IDX2(k,w_,p_)] < s[IDX2(k-1,w,p)]) {
                    info.debug("*");
                    s[IDX2(k,w_,p_)] = s[IDX2(k-1,w,p)];
                    pred[IDX2(k,w_,p_)] = IDX2(k-1,w,p);
                }
            }
            info.debug("\n");
        }
        info.debug("\n");

        // Remove previously added items
        for (Weight mu=c+wt; mu>c; --mu) {
            info.debug("mu " + std::to_string(mu));
            Weight w = mu + w_max - c - 1;
            Profit alpha = ALPHA(mu);
            Profit beta  = BETA(mu);
            for (Profit pi=alpha; pi<=beta; ++pi) {
                Profit p = pi - alpha;
                info.debug("  pi " + std::to_string(pi));
                for (ItemPos j=s[IDX2(k-1,w,p)]; j<s[IDX2(k,w,p)]; ++j) {
                    info.debug("  j " + std::to_string(j) +
                            "(" + std::to_string(ins.item(j).j) + ")");
                    Weight mu_ = mu - ins.item(j).w;
                    Profit pi_ = pi - ins.item(j).p;
                    Profit alpha_ = ALPHA(mu_);
                    Profit beta_  = BETA(mu_);
                    if (pi_ < alpha_ || pi_ > beta_) {
                        info.debug("×");
                        continue;
                    }
                    Weight w_  = mu_ + w_max - 1 - c;
                    Profit p_ = pi_ - alpha_;
                    info.debug("," + std::to_string(s[IDX2(k,w_,p_)]));
                    if (s[IDX2(k,w_,p_)] < j) {
                        info.debug("*");
                        s[IDX2(k,w_,p_)] = j;
                        pred[IDX2(k,w_,p_)] = IDX2(k,w,p);
                    }
                }
            }
            info.debug("\n");
        }

        info.debug("\n");
    }

    // Get optimal value
    info.debug("Find optimal value:");
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
    info.debug(" " + std::to_string(opt) + "\n");
    if (opt <= sol.profit()) {
        info.debug("Optimal value equals lower bound.\n");
        return algorithm_end(sol, info);
    }
    assert(ins.check_opt(opt));

    // Retrieve optimal solution
    info.debug("Retrieve optimal solution.\n");
    sol = *ins.break_solution();

    StateIdx idx = idx_opt;
    ItemPos  k   = idx / rl2;
    ItemPos  t   = k + b - 1;
    Weight   w   = (idx % rl2) / rl1;
    Weight   mu  = c + 1 + w - w_max;
    Profit   p   = (idx % rl2) % rl1;
    Profit alpha = ALPHA(mu);
    Profit pi = p + alpha;
    info.debug(
            "a " + std::to_string(s[k]) +
            " t " + std::to_string(t) +
            " wt " + std::to_string(ins.item(t).w) +
            " pt " + std::to_string(ins.item(t).p) +
            " mu " + std::to_string(mu) +
            " pi " + std::to_string(pi) +
            " p(S) " + std::to_string(sol.profit()) +
            "\n");

    while (!(sol.profit() == opt && sol.remaining_capacity() >= 0)) {
        StateIdx idx_next = pred[idx];
        ItemPos  k_next   = idx_next / rl2;
        ItemPos  t_next   = k_next + b - 1;
        Weight   w_next   = (idx_next % rl2) / rl1;
        Weight   mu_next  = c + 1 + w_next - w_max;
        Profit   p_next   = (idx_next % rl2) % rl1;
        Profit   alpha_next = ALPHA(mu_next);
        Profit   pi_next    = p_next + alpha_next;
        info.debug(
                "a " + std::to_string(s[k_next]) +
                " t " + std::to_string(t_next) +
                " wt " + std::to_string(ins.item(t_next).w) +
                " pt " + std::to_string(ins.item(t_next).p) +
                " mu " + std::to_string(mu_next) +
                " pi " + std::to_string(pi_next) +
                "\n");

        if (k_next < k && pi_next < pi) {
            sol.set(t, true);
            info.debug("Add " + std::to_string(t) + " p(S) " + std::to_string(sol.profit()) + "\n");
            assert(mu_next == mu - ins.item(t).w);
            assert(pi_next == pi - ins.item(t).p);
        }
        if (k_next == k) {
            sol.set(s[idx], false);
            info.debug("Remove " + std::to_string(s[idx]) + " p(S) " + std::to_string(sol.profit()) + "\n");
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
    return algorithm_end(sol, info);
}

/******************************************************************************/

Solution knapsack::sopt_balknap_array_part(Instance& ins, Info& info,
        BalknapParams p, ItemPos k)
{
    (void)info;
    (void)p;
    (void)k;
    assert(false); // TODO
    return Solution(ins);
}


