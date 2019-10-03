#include "knapsack/ub_surrogate/surrogate.hpp"

#include "knapsack/ub_dantzig/dantzig.hpp"

#include <stdlib.h>

using namespace knapsack;

ItemIdx max_card(const Instance& ins, Info& info)
{
    LOG_FOLD_START(info, "max_card" << std::endl);
    if (ins.item_number() == 1) {
        LOG_FOLD_END(info, "1 item");
        return 1;
    }

    std::vector<ItemIdx> index(ins.total_item_number());
    std::iota(index.begin(), index.end(), 0);
    ItemIdx f = ins.first_item();
    ItemIdx l = ins.last_item();
    Weight  w = 0;
    Weight  c = ins.capacity();
    while (f < l) {
        if (l - f < 128) {
            std::sort(index.begin()+f, index.begin()+l+1,
                    [&ins](ItemPos j1, ItemPos j2) {
                    return ins.item(j1).w < ins.item(j2).w;});
            break;
        }

        ItemIdx pivot = f + 1 + rand() % (l - f); // Select pivot

        iter_swap(index.begin() + pivot, index.begin() + l);
        ItemIdx j = f;
        for (ItemIdx i=f; i<l; ++i) {
            if (ins.item(index[i]).w > ins.item(index[l]).w)
                continue;
            iter_swap(index.begin() + i, index.begin() + j);
            j++;
        }
        iter_swap(index.begin() + j, index.begin() + l);

        Weight w_curr = w;
        for (ItemIdx i=f; i<j; ++i)
            w_curr += ins.item(index[i]).w;

        if (w_curr + ins.item(index[j]).w <= c) {
            f = j + 1;
            w = w_curr + ins.item(index[j]).w;
        } else if (w_curr > c) {
            l = j - 1;
        } else {
            break;
        }
    }

    ItemPos k = ins.reduced_solution()->item_number();
    Weight r = ins.capacity();
    for (ItemPos j=ins.first_item(); j<=ins.last_item(); ++j) {
        if (r < ins.item(index[j]).w) {
            k = j;
            break;
        }
        r -= ins.item(index[j]).w;
    }

    LOG_FOLD_END(info, "k " << k);
    return k;
}

ItemIdx min_card(const Instance& ins, Info& info, Profit lb)
{
    LOG_FOLD_START(info, "min_card" << std::endl);

    lb -= ins.reduced_solution()->profit();
    if (ins.item_number() <= 1) {
        LOG_FOLD_END(info, "1 item");
        return (ins.item(1).p <= lb)? 1: 0;
    }

    std::vector<ItemIdx> index(ins.total_item_number());
    std::iota(index.begin(), index.end(), 0);
    ItemIdx f = ins.first_item();
    ItemIdx l = ins.last_item();
    Profit p = 0;
    while (f < l) {
        if (l - f < 128) {
            std::sort(index.begin()+f, index.begin()+l+1,
                    [&ins](ItemPos j1, ItemPos j2) {
                    return ins.item(j1).p > ins.item(j2).p;});
            break;
        }

        ItemIdx pivot = f + 1 + rand() % (l - f); // Select pivot

        iter_swap(index.begin() + pivot, index.begin() + l);
        ItemIdx j = f;
        for (ItemIdx i=f; i<l; ++i) {
            if (ins.item(index[i]).p < ins.item(index[l]).p)
                continue;
            iter_swap(index.begin() + i, index.begin() + j);
            j++;
        }
        iter_swap(index.begin() + j, index.begin() + l);

        Profit p_curr = p;
        for (ItemIdx i=f; i<j; ++i)
            p_curr += ins.item(index[i]).p;

        if (p_curr > lb) {
            l = j - 1;
        } else if (p_curr + ins.item(index[j]).p <= lb) {
            f = j + 1;
            p = p_curr + ins.item(index[j]).p;
        } else {
            break;
        }
    }

    ItemPos k = -1;
    Profit z = 0;
    for (ItemPos j=ins.first_item(); j<=ins.last_item(); ++j) {
        if (z + ins.item(index[j]).p > lb) {
            k = j + 1;
            break;
        }
        z += ins.item(index[j]).p;
    }

    LOG_FOLD_END(info, "k " << k);
    return k;
}

struct UBS
{
    Profit ub;
    Weight s;
};

UBS ub_surrogate_solve(Instance& ins, Info& info, ItemIdx k,
        Weight s_min, Weight s_max, bool* end)
{
    LOG_FOLD_START(info, "ub_surrogate_solve k " << k << " s_min " << s_min << " s_max " << s_max << std::endl);
    ItemPos first = ins.first_item();
    Profit ub = ub_dantzig(ins);
    Weight  s_prec = 0;
    Weight  s = 0;
    Weight  s_best = 0;
    Weight s1 = s_min;
    Weight s2 = s_max;
    Weight wmax = ins.item(ins.first_item()).w;
    Weight wmin = ins.item(ins.first_item()).w;
    Profit pmax = ins.item(ins.first_item()).p;
    for (ItemPos j=ins.first_item()+1; j<=ins.last_item(); ++j) {
        if (wmax < ins.item(j).w)
            wmax = ins.item(j).w;
        if (wmin > ins.item(j).w)
            wmin = ins.item(j).w;
        if (pmax < ins.item(j).p)
            pmax = ins.item(j).p;
    }
    Weight wabs = wmax;
    Weight wlim = INT_FAST64_MAX / pmax;

    while (s1 <= s2) {
        if (*end)
            return {ub, s_best};
        s = (s1 + s2) / 2;
        LOG_FOLD_START(info, "s1 " << s1 << " s " << s << " s2 " << s2 << std::endl);

        // Avoid INT overflow
        if (s_min == 0 && s != 0) {
            if (INT_FAST64_MAX / s < k
                    || ins.total_capacity() > INT_FAST64_MAX - s * k
                    || INT_FAST64_MAX / ins.total_item_number() < wmax+s
                    || wmax + s > wlim) {
                s2 = s - 1;
                LOG_FOLD_END(info, "");
                continue;
            } else {
                wmax += s - s_prec;
            }
        }
        if (s_max == 0 && s != 0) {
            wabs = (wmax+s > -wmin+s)? wmax+s: wmin+s;
            if (INT_FAST64_MAX / -s < k
                    || INT_FAST64_MAX / ins.total_item_number() < wabs
                    || wabs > wlim) {
                s1 = s + 1;
                LOG_FOLD_END(info, "");
                continue;
            } else {
                wmax += s - s_prec;
                wmin += s - s_prec;
            }
        }

        ins.surrogate(info, s-s_prec, k, first);
        LOG_FOLD(info, ins);
        Profit p = ub_dantzig(ins);
        ItemPos b = ins.break_item();

        LOG(info, "b " << b << " p " << p << std::endl);

        if (ub > p) {
            ub = p;
            s_best = s;
        }

        if (b == k && ins.break_capacity() == 0) {
            LOG_FOLD_END(info, "");
            break;
        }

        if ((s_min == 0 && b >= k) || (s_max == 0 && b >= k)) {
            s1 = s + 1;
        } else {
            s2 = s - 1;
        }
        s_prec = s;
        LOG_FOLD_END(info, "");
    }
    ins.surrogate(info, -s, k);
    LOG_FOLD_END(info, "ub " << ub << " s " << s_best);
    return {ub, s_best};
}

void knapsack::ub_solvesurrelax(SurrelaxData d)
{
    LOG_FOLD_START(d.info, "surrogate relaxation lb " << d.output.lower_bound << std::endl);

    d.ins.sort_partially(d.info);
    ItemPos b = d.ins.break_item();

    // Trivial cases
    if (d.ins.item_number() == 0) {
        Profit ub = (d.ins.reduced_solution() == NULL)? 0: d.ins.reduced_solution()->profit();
        if (d.output.upper_bound == -1 || d.output.upper_bound > ub)
            d.output.update_ub(ub, std::stringstream("surrogate relaxation"), d.info);
        LOG_FOLD_END(d.info, "no items");
        return;
    }
    Profit ub = ub_dantzig(d.ins);
    if (d.ins.break_capacity() == 0 || b == d.ins.last_item() + 1) {
        if (d.output.upper_bound == -1 || d.output.upper_bound > ub)
            d.output.update_ub(ub, std::stringstream("surrogate relaxation"), d.info);
        LOG_FOLD_END(d.info, "dantzig");
        return;
    }

    // Compte s_min and s_max
    // s_min and s_max should ideally be (-)pmax*wmax, but this may cause
    // overflow
    Weight wmax = d.ins.item(d.ins.max_weight_item(d.info)).w;
    Profit pmax = d.ins.item(d.ins.max_profit_item(d.info)).p;
    Weight s_max = (INT_FAST64_MAX / pmax > wmax)?  pmax*wmax:  INT_FAST64_MAX;
    Weight s_min = (INT_FAST64_MAX / pmax > wmax)? -pmax*wmax: -INT_FAST64_MAX;

    if (max_card(d.ins, d.info) == b) {
        UBS o = ub_surrogate_solve(d.ins, d.info, b, 0, s_max, d.end);
        if (*d.end)
            return;
        Profit ub = std::max(o.ub, d.output.lower_bound);
        d.output.update_ub(ub, std::stringstream("surrogate relaxation"), d.info);
        if (d.output.upper_bound == d.output.lower_bound || o.s == 0)
            return;

        Solution sol_sur(d.output.solution.instance());
        d.ins.surrogate(d.info, o.s, b);
        Output output = d.func(d.ins, Info(d.info, false, ""), d.end);
        if (output.solution.profit() != output.upper_bound)
            return;
        sol_sur = output.solution;
        d.output.update_sol(sol_sur, std::stringstream("surrogate ins resolution (lb)"), d.info);
        ub = std::max(sol_sur.profit(), d.output.lower_bound);
        d.output.update_ub(ub, std::stringstream("surrogate ins resolution (ub)"), d.info);
    } else if (min_card(d.ins, d.info, d.output.lower_bound) == b + 1) {
        UBS o = ub_surrogate_solve(d.ins, d.info, b + 1, s_min, 0, d.end);
        if (*d.end)
            return;
        Profit ub = std::max(o.ub, d.output.lower_bound);
        d.output.update_ub(ub, std::stringstream("surrogate relaxation"), d.info);
        if (d.output.upper_bound == d.output.lower_bound || o.s == 0)
            return;

        Solution sol_sur(d.output.solution.instance());
        d.ins.surrogate(d.info, o.s, b + 1);
        Output output = d.func(d.ins, Info(d.info, false, ""), d.end);
        if (output.solution.profit() != output.upper_bound)
            return;
        sol_sur = output.solution;
        d.output.update_sol(sol_sur, std::stringstream("surrogate ins resolution (lb)"), d.info);
        ub = std::max(sol_sur.profit(), d.output.lower_bound);
        d.output.update_ub(ub, std::stringstream("surrogate ins resolution (ub)"), d.info);
    } else {
        Instance ins_2(d.ins);
        UBS o1 = ub_surrogate_solve(d.ins, d.info, b,     0,     s_max, d.end);
        if (*d.end)
            return;
        UBS o2 = ub_surrogate_solve(ins_2, d.info, b + 1, s_min, 0,     d.end);
        if (*d.end)
            return;
        Profit ub = std::max(std::max(o1.ub, o2.ub), d.output.lower_bound);
        d.output.update_ub(ub, std::stringstream("surrogate relaxation"), d.info);
        if (d.output.upper_bound == d.output.lower_bound || o1.s == 0 || o2.s == 0)
            return;

        Solution sol_sur1(d.output.solution.instance());
        d.ins.surrogate(d.info, o1.s, b);
        Output output1 = d.func(d.ins, Info(d.info, false, ""), d.end);
        if (output1.solution.profit() != output1.upper_bound)
            return;
        sol_sur1 = output1.solution;
        d.output.update_sol(sol_sur1, std::stringstream("surrogate ins resolution (lb)"), d.info);
        if (*d.end || d.output.lower_bound == d.output.upper_bound)
            return;

        Solution sol_sur2(d.output.solution.instance());
        ins_2.surrogate(d.info, o2.s, b + 1);
        Output output2 = d.func(ins_2, Info(d.info, false, ""), d.end);
        if (output2.solution.profit() != output2.upper_bound)
            return;
        sol_sur2 = output2.solution;
        d.output.update_sol(sol_sur2, std::stringstream("surrogate ins resolution (lb)"), d.info);

        ub = std::max(std::max(sol_sur1.profit(), sol_sur2.profit()), d.output.lower_bound);
        d.output.update_ub(ub, std::stringstream("surrogate ins resolution (ub)"), d.info);
    }

    LOG_FOLD_END(d.info, "");
}

