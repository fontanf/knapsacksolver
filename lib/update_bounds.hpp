#pragma once

#include "knapsack/lib/instance.hpp"
#include "knapsack/lib/solution.hpp"
#include "knapsack/lb_greedynlogn/greedynlogn.hpp"
#include "knapsack/ub_surrogate/surrogate.hpp"

namespace knapsack
{

/**
 * The first struct should be used with algorithms returning the optimal
 * solution.
 * The second one with algorithms only returning the optimal value.
 */

struct UpdateBoundsSolData
{
    UpdateBoundsSolData(const Instance& ins, Info& info,
            Solution& sol_best, Profit& lb, Profit& ub,
            Cpt& sol_number, Cpt& ub_number):
        ins(ins), info(info), sol_best(sol_best), lb(lb), ub(ub),
        sol_number(sol_number), ub_number(ub_number) {  }

    const Instance& ins;
    Info& info;
    Cpt cpt_ubsur;
    Cpt cpt_solvesur;
    Cpt cpt_greedynlogn;
    SurrogateOut so;

    Solution& sol_best;
    Profit &lb;
    Profit &ub;
    Cpt& sol_number;
    Cpt& ub_number;

    template <typename Func>
    void run(Func func, Cpt cpt) {
        // Compute Surrogate relaxation Upper bound
        if (0 <= cpt_ubsur && cpt_ubsur <= cpt) {
            info.verbose("Surrogate...\n");
            cpt_ubsur = -1;
            Info info_tmp;
            so = ub_surrogate(ins, sol_best.profit(), info_tmp);
            if (ub > so.ub)
                Solution::update_ub(ub, so.ub, info, ub_number, lb);
        }

        // Solve Surrogate instance
        if (0 <= cpt_solvesur && cpt_solvesur <= cpt) {
            if (sol_best.profit() == ub)
                return;
            info.verbose("Solve surrogate instance...\n");
            cpt_solvesur = -1;

            Instance ins_sur(ins);
            ins_sur.surrogate(so.multiplier, so.bound, ins_sur.first_item());
            Solution sol_sur = func(ins_sur);
            info.verbose("<- End solve surrogate instance\n");

            if (ub > sol_sur.profit())
                Solution::update_ub(ub, so.ub, info, ub_number, lb);

            DBG(info.debug(STR3(k, sol_sur.item_number()) + "/" + std::to_string(so.bound) + "\n");)
            if (sol_sur.item_number() == so.bound) { // => sol_sur optimal
                sol_best.update(sol_sur, info, sol_number, ub);
                lb = sol_best.profit();
            }
        }

        // Compute Greedynlogn
        if (0 <= cpt_greedynlogn && cpt_greedynlogn <= cpt) {
            info.verbose("Run greedynlogn...");
            cpt_greedynlogn = -1;
            Info info_tmp;
            Solution sol_tmp = sol_bestgreedynlogn(ins, info_tmp);
            if (sol_tmp.profit() > lb) {
                sol_best.update(sol_tmp, info, sol_number, ub);
                lb = sol_best.profit();
            }
        }
    }

};

struct UpdateBoundsData
{
    UpdateBoundsData(const Instance& ins, Info& info,
            Profit& lb, Profit& ub,
            Cpt& lb_number, Cpt& ub_number):
        ins(ins), info(info), lb(lb), ub(ub),
        lb_number(lb_number), ub_number(ub_number) {  }

    const Instance& ins;
    Info& info;
    Cpt cpt_ubsur;
    Cpt cpt_solvesur;
    Cpt cpt_greedynlogn;
    SurrogateOut so;

    Profit &lb;
    Profit &ub;
    Cpt& lb_number;
    Cpt& ub_number;

    template <typename Func>
    void run(Func func, Cpt cpt) {
        // Compute Surrogate relaxation Upper bound
        if (0 <= cpt_ubsur && cpt_ubsur <= cpt) {
            info.verbose("Surrogate...\n");
            cpt_ubsur = -1;
            Info info_tmp;
            so = ub_surrogate(ins, lb, info_tmp);
            if (ub > so.ub)
                Solution::update_ub(ub, so.ub, info, ub_number, lb);
        }

        // Solve Surrogate instance
        if (0 <= cpt_solvesur && cpt_solvesur <= cpt) {
            if (lb == ub)
                return;
            info.verbose("Solve surrogate instance...\n");
            cpt_solvesur = -1;

            Instance ins_sur(ins);
            ins_sur.surrogate(so.multiplier, so.bound, ins_sur.first_item());
            std::pair<Profit, ItemIdx> res_sur = func(ins_sur);
            info.verbose("<- End solve surrogate instance\n");

            if (ub > res_sur.first)
                Solution::update_ub(ub, res_sur.first, info, ub_number, lb);

            if (res_sur.second == so.bound) {
                Solution::update_lb(lb, res_sur.first, info, lb_number, ub);
                return;
            }
        }

        // Compute Greedynlogn
        if (0 <= cpt_greedynlogn && cpt_greedynlogn <= cpt) {
            info.verbose("Run greedynlogn...");
            cpt_greedynlogn = -1;
            Info info_tmp;
            Solution sol = sol_bestgreedynlogn(ins, info_tmp);
            if (sol.profit() > lb)
                Solution::update_lb(lb, sol.profit(), info, lb_number, ub);
        }
    }

};

}

