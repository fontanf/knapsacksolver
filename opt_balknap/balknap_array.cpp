#include "knapsack/opt_balknap/balknap.hpp"

#include "knapsack/lb_greedy/greedy.hpp"
#include "knapsack/lb_greedynlogn/greedynlogn.hpp"
#include "knapsack/ub_dembo/dembo.hpp"
#include "knapsack/ub_dantzig/dantzig.hpp"
#include "knapsack/ub_surrogate/surrogate.hpp"

using namespace knapsack;

#define IDX2(k,w,p) rl2*(k) + rl1*(w) + (p)
#define IDX1(  w,p)           rl1*(w) + (p)

#define ALPHA(mu) (mu <= c)? z + 1 - ((c - mu) * pb) / wb: z + 1 + ((mu - c) * pb) / wb
#define BETA(mu) (mu >= w_bar)? p_bar + ((mu - w_bar) * pb) / wb: p_bar - ((w_bar - mu) * pb) / wb

Profit knapsack::opt_balknap_array(Instance& ins, Info& info, BalknapParams p)
{
    info.verbose("*** balknap (list) ***\n");

    DBG(info.debug("Sort items...\n");)
    ins.sort_partially();
    if (ins.break_item() == ins.last_item()+1) { // all items are in the break solution
        DBG(info.debug("All items fit in the knapsack.\n");)
        return algorithm_end(ins.break_profit(), info);
    }

    info.verbose("Compute lower bound...");
    Profit lb = 0;
    if (p.lb_greedy == 0) {
        Info info_tmp;
        lb = sol_greedy(ins, info_tmp).profit();
    } else {
        lb = ins.break_solution()->profit();
    }
    info.verbose(" " + std::to_string(lb) + "\n");

    ins.reduce1(lb, info);
    if (ins.capacity() < 0) {
        DBG(info.debug("All items have been reduced.\n");)
        return algorithm_end(lb, info);
    }
    DBG(info.debug("Reduced solution: " + ins.reduced_solution()->to_string_binary() + "\n");)
    DBG(info.debug("Reduced solution: " + ins.reduced_solution()->to_string_items() + "\n");)

    Weight  c = ins.capacity();
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

    ItemPos b = ins.break_item();
    Weight w_max = ins.item(ins.max_weight_item()).w;
    Weight w_bar = ins.break_weight();
    Profit p_bar = ins.break_profit();
    Weight wb    = ins.item(b).w;
    Profit pb    = ins.item(b).p;
    Weight r     = ins.break_capacity();

    DBG(info.debug("Break solution: " + ins.break_solution()->to_string_binary() + "\n");)
    DBG(info.debug("Break solution: " + ins.break_solution()->to_string_items() + "\n");)
    DBG(info.debug(STR1(n) + STR2(c) + STR2(f) + STR2(l) + "\n"
                + STR1(w_bar) + STR2(p_bar) + "\n");)

    if (ins.break_solution()->profit() > lb) {
        DBG(info.debug("Update best solution.");)
        lb = ins.break_solution()->profit();
    }

    Profit z = lb - p0;
    Profit u = p_bar + r * pb / wb;
    info.verbose(STR1(lb) + STR2(u) + STR4(gap, u-lb) + "\n");

    if (z >= u) { // If UB == LB, then stop
        DBG(info.debug("Lower bound equals upper bound.");)
        return algorithm_end(lb, info);
    }

    // Create memory table
    Profit rl1 = u - z + 1;
    StateIdx rl2 = rl1 * 2 * w_max;
    std::vector<ItemPos> s0(rl2);
    std::vector<ItemPos> s1(rl2);

    // Initialization
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

    info.verbose("Recursion...\n");
    for (ItemPos t=b; t<=l; ++t) { // Recursion
        Weight wt = ins.item(t).w;
        Profit pt = ins.item(t).p;

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
            Weight mu_ = mu + wt;
            Weight w   = mu  + w_max - c - 1;
            Weight w_  = mu_ + w_max - c - 1;
            assert(w_ < 2 * w_max);

            Profit alpha = ALPHA(mu);
            Profit beta  = BETA(mu);
            Profit alpha_ = ALPHA(mu_);
            Profit beta_  = BETA(mu_);

            //for (Profit pi=inf; pi<=sup; ++pi) {
            for (Profit pi=alpha; pi<=beta; ++pi) {
                Profit pi_ = pi + pt;
                if (pi_ < alpha_) {
                    continue;
                }
                if (pi_ > beta_) {
                    break;
                }
                Profit p  = pi  - alpha;
                Profit p_ = pi_ - alpha_;
                if (s1[IDX1(w_,p_)] < s0[IDX1(w,p)]) {
                    s1[IDX1(w_,p_)] = s0[IDX1(w,p)];
                } else {
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
                for (ItemPos j=s0[IDX1(w,p)]; j<s1[IDX1(w,p)]; ++j) {
                    Weight mu_ = mu - ins.item(j).w;
                    Profit pi_ = pi - ins.item(j).p;
                    Profit alpha_ = ALPHA(mu_);
                    Profit beta_  = BETA(mu_);
                    if (pi_ < alpha_ || pi_ > beta_)
                        continue;
                    Weight w_ = mu_ + w_max - 1 - c;
                    Profit p_ = pi_ - alpha_;
                    if (s1[IDX1(w_,p_)] < j) {
                        s1[IDX1(w_,p_)] = j;
                    } else {
                    }
                }
            }
        }

        s1.swap(s0); // Swap pointers
    }

    // Get optimal value
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
    return algorithm_end(opt, info);
}

/******************************************************************************/

Solution knapsack::sopt_balknap_array_all(Instance& ins, Info& info,
        BalknapParams params)
{
    info.verbose("*** balknap (array, all) ***\n");

    ins.sort_partially();

    // If all items can fit in the knapsack, then stop
    if (ins.break_item() == ins.last_item()+1) {
        DBG(info.debug("All items fit in the knapsack.\n");)
        Solution sol = *ins.break_solution();
        return algorithm_end(sol, info);
    }

    // Compute lower bound
    Solution sol(ins);
    if (params.lb_greedy == 0) {
        Info info_tmp;
        sol = sol_greedy(ins, info_tmp);
    } else {
        sol = *ins.break_solution();
    }

    // Variable reduction
    ins.reduce1(sol.profit(), info);
    if (ins.capacity() < 0) {
        DBG(info.debug("All items have been reduced.\n");)
        return algorithm_end(sol, info);
    }
    DBG(info.debug("Reduced solution: " + ins.reduced_solution()->to_string_binary() + "\n");)
    DBG(info.debug("Reduced solution: " + ins.reduced_solution()->to_string_items() + "\n");)

    Weight  c = ins.capacity();
    ItemPos f = ins.first_item();
    ItemPos l = ins.last_item();
    ItemPos n = ins.item_number();
    Profit p0 = ins.reduced_solution()->profit();

    // Trivial cases
    if (n == 0 || c == 0) {
        DBG(info.debug("Empty instance (after reduction).\n");)
        if (ins.reduced_solution()->profit() > sol.profit())
            Solution sol = *ins.reduced_solution();
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
    Weight w_max = ins.item(ins.max_weight_item()).w;
    Weight w_bar = ins.break_weight();
    Profit p_bar = ins.break_profit();
    Weight wb    = ins.item(b).w;
    Profit pb    = ins.item(b).p;
    Weight r     = ins.break_capacity();
    Profit z     = sol.profit() - ins.reduced_solution()->profit();
    Profit u     = ins.break_profit() + r * pb / wb;

    DBG(info.debug("Break solution: " + ins.break_solution()->to_string_binary() + "\n");)
    DBG(info.debug("Break solution: " + ins.break_solution()->to_string_items() + "\n");)
    DBG(info.debug(
            "n " + std::to_string(n) +
            " c " + std::to_string(c) +
            " f " + std::to_string(f) + " l " + std::to_string(l) +
            "\n" +
            "wmax " + std::to_string(w_max) +
            " wbar " + std::to_string(w_bar) +
            " pbar " + std::to_string(p_bar) +
            "\n");)

    if (ins.break_solution()->profit() > sol.profit()) {
        DBG(info.debug("Update best solution.");)
        sol = *ins.break_solution();
    }

    // If UB == LB, then stop
    if (z >= u) {
        DBG(info.debug("Lower bound equals upper bound.");)
        return algorithm_end(sol, info);
    }

    // Create memory table
    DBG(info.debug("Create memory table.\n");)
    Profit rl1 = u - z + 1;
    StateIdx rl2 = rl1 * 2 * w_max;
    DBG(info.debug("rl1 " + std::to_string(rl1) + " rl2 " + std::to_string(rl2) + "\n");)
    std::vector<ItemPos> s   ((l-b+2)*rl2);
    std::vector<ItemPos> pred((l-b+2)*rl2);

    // Initialization
    DBG(info.debug("Initialization\n");)
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
    DBG(info.debug("Recursion\n");)
    for (ItemPos t=b; t<=l; ++t) {
        DBG(info.debug("t " + std::to_string(ins.item(t).j) + "\n");)
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
            DBG(info.debug("mu " + std::to_string(mu));)
            Weight mu_ = mu + wt;
            Weight w   = mu  + w_max - c - 1;
            Weight w_  = mu_ + w_max - c - 1;
            Profit alpha = ALPHA(mu);
            Profit beta  = BETA(mu);
            Profit alpha_ = ALPHA(mu_);
            Profit beta_  = BETA(mu_);
            assert(w_ < 2 * w_max);
            for (Profit pi=alpha; pi<=beta; ++pi) {
                DBG(info.debug("  pi " + std::to_string(pi));)
                Profit pi_ = pi + pt;
                if (pi_ < alpha_)
                    continue;
                if (pi_ > beta_)
                    break;
                Profit p  = pi  - alpha;
                Profit p_ = pi_ - alpha_;
                if (s[IDX2(k,w_,p_)] < s[IDX2(k-1,w,p)]) {
                    DBG(info.debug("*");)
                    s[IDX2(k,w_,p_)] = s[IDX2(k-1,w,p)];
                    pred[IDX2(k,w_,p_)] = IDX2(k-1,w,p);
                }
            }
            DBG(info.debug("\n");)
        }
        DBG(info.debug("\n");)

        // Remove previously added items
        for (Weight mu=c+wt; mu>c; --mu) {
            DBG(info.debug("mu " + std::to_string(mu));)
            Weight w = mu + w_max - c - 1;
            Profit alpha = ALPHA(mu);
            Profit beta  = BETA(mu);
            for (Profit pi=alpha; pi<=beta; ++pi) {
                Profit p = pi - alpha;
                DBG(info.debug("  pi " + std::to_string(pi));)
                for (ItemPos j=s[IDX2(k-1,w,p)]; j<s[IDX2(k,w,p)]; ++j) {
                    DBG(info.debug("  j " + std::to_string(j) +
                            "(" + std::to_string(ins.item(j).j) + ")");)
                    Weight mu_ = mu - ins.item(j).w;
                    Profit pi_ = pi - ins.item(j).p;
                    Profit alpha_ = ALPHA(mu_);
                    Profit beta_  = BETA(mu_);
                    if (pi_ < alpha_ || pi_ > beta_) {
                        DBG(info.debug("×");)
                        continue;
                    }
                    Weight w_  = mu_ + w_max - 1 - c;
                    Profit p_ = pi_ - alpha_;
                    DBG(info.debug("," + std::to_string(s[IDX2(k,w_,p_)]));)
                    if (s[IDX2(k,w_,p_)] < j) {
                        DBG(info.debug("*");)
                        s[IDX2(k,w_,p_)] = j;
                        pred[IDX2(k,w_,p_)] = IDX2(k,w,p);
                    }
                }
            }
            DBG(info.debug("\n");)
        }

        DBG(info.debug("\n");)
    }

    // Get optimal value
    DBG(info.debug("Find optimal value:");)
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
    DBG(info.debug(" " + std::to_string(opt) + "\n");)
    if (opt <= sol.profit()) {
        DBG(info.debug("Optimal value equals lower bound.\n");)
        return algorithm_end(sol, info);
    }

    // Retrieve optimal solution
    DBG(info.debug("Retrieve optimal solution.\n");)
    sol = *ins.break_solution();

    StateIdx idx = idx_opt;
    ItemPos  k   = idx / rl2;
    ItemPos  t   = k + b - 1;
    Weight   w   = (idx % rl2) / rl1;
    Weight   mu  = c + 1 + w - w_max;
    Profit   p   = (idx % rl2) % rl1;
    Profit alpha = ALPHA(mu);
    Profit pi = p + alpha;
    DBG(info.debug(
            "a " + std::to_string(s[k]) +
            " t " + std::to_string(t) +
            " wt " + std::to_string(ins.item(t).w) +
            " pt " + std::to_string(ins.item(t).p) +
            " mu " + std::to_string(mu) +
            " pi " + std::to_string(pi) +
            " p(S) " + std::to_string(sol.profit()) +
            "\n");)

    while (!(sol.profit() == opt && sol.remaining_capacity() >= 0)) {
        StateIdx idx_next = pred[idx];
        ItemPos  k_next   = idx_next / rl2;
        ItemPos  t_next   = k_next + b - 1;
        Weight   w_next   = (idx_next % rl2) / rl1;
        Weight   mu_next  = c + 1 + w_next - w_max;
        Profit   p_next   = (idx_next % rl2) % rl1;
        Profit   alpha_next = ALPHA(mu_next);
        Profit   pi_next    = p_next + alpha_next;
        DBG(info.debug(
                "a " + std::to_string(s[k_next]) +
                " t " + std::to_string(t_next) +
                " wt " + std::to_string(ins.item(t_next).w) +
                " pt " + std::to_string(ins.item(t_next).p) +
                " mu " + std::to_string(mu_next) +
                " pi " + std::to_string(pi_next) +
                "\n");)

        if (k_next < k && pi_next < pi) {
            sol.set(t, true);
            DBG(info.debug("Add " + std::to_string(t) + " p(S) " + std::to_string(sol.profit()) + "\n");)
            assert(mu_next == mu - ins.item(t).w);
            assert(pi_next == pi - ins.item(t).p);
        }
        if (k_next == k) {
            sol.set(s[idx], false);
            DBG(info.debug("Remove " + std::to_string(s[idx]) + " p(S) " + std::to_string(sol.profit()) + "\n");)
        }

        idx = idx_next;
        k   = k_next;
        t   = t_next;
        w   = w_next;
        mu  = mu_next;
        p   = p_next;
        pi  = pi_next;
    }

    return algorithm_end(sol, info);
}

/******************************************************************************/

Solution knapsack::sopt_balknap_array_part(Instance& ins, Info& info,
        BalknapParams p, ItemPos k)
{
    info.verbose("*** balknap (array, part " + std::to_string(k) + ") ***\n");
    (void)p;
    Solution sol(ins);
    info.verbose("Not yet implemented.\n");
    return algorithm_end(sol, info);
}


