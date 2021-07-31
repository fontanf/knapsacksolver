#include "knapsacksolver/algorithms/surrelax.hpp"

#include "knapsacksolver/algorithms/dantzig.hpp"
#include "knapsacksolver/algorithms/minknap.hpp"

#include <stdlib.h>

using namespace knapsacksolver;

ItemIdx max_card(const Instance& instance, Info& info)
{
    LOG_FOLD_START(info, "max_card" << std::endl);
    if (instance.reduced_number_of_items() == 1) {
        LOG_FOLD_END(info, "1 item");
        return 1;
    }

    std::vector<ItemIdx> index(instance.number_of_items());
    std::iota(index.begin(), index.end(), 0);
    ItemIdx f = instance.first_item();
    ItemIdx l = instance.last_item();
    Weight  w = 0;
    Weight  c = instance.reduced_capacity();
    while (f < l) {
        if (l - f < 128) {
            std::sort(index.begin()+f, index.begin()+l+1,
                    [&instance](ItemPos j1, ItemPos j2) {
                    return instance.item(j1).w < instance.item(j2).w;});
            break;
        }

        ItemIdx pivot = f + 1 + rand() % (l - f); // Select pivot

        iter_swap(index.begin() + pivot, index.begin() + l);
        ItemIdx j = f;
        for (ItemIdx i = f; i < l; ++i) {
            if (instance.item(index[i]).w > instance.item(index[l]).w)
                continue;
            iter_swap(index.begin() + i, index.begin() + j);
            j++;
        }
        iter_swap(index.begin() + j, index.begin() + l);

        Weight w_curr = w;
        for (ItemIdx i = f; i < j; ++i)
            w_curr += instance.item(index[i]).w;

        if (w_curr + instance.item(index[j]).w <= c) {
            f = j + 1;
            w = w_curr + instance.item(index[j]).w;
        } else if (w_curr > c) {
            l = j - 1;
        } else {
            break;
        }
    }

    ItemPos k = instance.reduced_solution()->number_of_items();
    Weight r = instance.reduced_capacity();
    for (ItemPos j = instance.first_item(); j <= instance.last_item(); ++j) {
        if (r < instance.item(index[j]).w) {
            k = j;
            break;
        }
        r -= instance.item(index[j]).w;
    }

    LOG_FOLD_END(info, "k " << k);
    return k;
}

ItemIdx min_card(const Instance& instance, Info& info, Profit lb)
{
    LOG_FOLD_START(info, "min_card" << std::endl);

    lb -= instance.reduced_solution()->profit();
    if (instance.reduced_number_of_items() <= 1) {
        LOG_FOLD_END(info, "1 item");
        return (instance.item(1).p <= lb)? 1: 0;
    }

    std::vector<ItemIdx> index(instance.number_of_items());
    std::iota(index.begin(), index.end(), 0);
    ItemIdx f = instance.first_item();
    ItemIdx l = instance.last_item();
    Profit p = 0;
    while (f < l) {
        if (l - f < 128) {
            std::sort(index.begin()+f, index.begin()+l+1,
                    [&instance](ItemPos j1, ItemPos j2) {
                    return instance.item(j1).p > instance.item(j2).p;});
            break;
        }

        ItemIdx pivot = f + 1 + rand() % (l - f); // Select pivot

        iter_swap(index.begin() + pivot, index.begin() + l);
        ItemIdx j = f;
        for (ItemIdx i = f; i < l; ++i) {
            if (instance.item(index[i]).p < instance.item(index[l]).p)
                continue;
            iter_swap(index.begin() + i, index.begin() + j);
            j++;
        }
        iter_swap(index.begin() + j, index.begin() + l);

        Profit p_curr = p;
        for (ItemIdx i = f; i < j; ++i)
            p_curr += instance.item(index[i]).p;

        if (p_curr > lb) {
            l = j - 1;
        } else if (p_curr + instance.item(index[j]).p <= lb) {
            f = j + 1;
            p = p_curr + instance.item(index[j]).p;
        } else {
            break;
        }
    }

    ItemPos k = -1;
    Profit z = 0;
    for (ItemPos j = instance.first_item(); j <= instance.last_item(); ++j) {
        if (z + instance.item(index[j]).p > lb) {
            k = j + 1;
            break;
        }
        z += instance.item(index[j]).p;
    }

    LOG_FOLD_END(info, "k " << k);
    return k;
}

struct UBS
{
    Profit ub;
    Weight s;
};

UBS surrogate_solve(Instance& instance, Info& info, ItemIdx k,
        Weight s_min, Weight s_max, bool* end)
{
    LOG_FOLD_START(info, "surrogate_solve k " << k << " s_min " << s_min << " s_max " << s_max << std::endl);
    ItemPos first = instance.first_item();
    Profit ub = ub_dantzig(instance);
    Weight  s_prec = 0;
    Weight  s = 0;
    Weight  s_best = 0;
    Weight s1 = s_min;
    Weight s2 = s_max;
    Weight wmax = instance.item(instance.first_item()).w;
    Weight wmin = instance.item(instance.first_item()).w;
    Profit pmax = instance.item(instance.first_item()).p;
    for (ItemPos j = instance.first_item() + 1; j <= instance.last_item(); ++j) {
        if (wmax < instance.item(j).w)
            wmax = instance.item(j).w;
        if (wmin > instance.item(j).w)
            wmin = instance.item(j).w;
        if (pmax < instance.item(j).p)
            pmax = instance.item(j).p;
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
                    || instance.capacity() > INT_FAST64_MAX - s * k
                    || INT_FAST64_MAX / instance.number_of_items() < wmax+s
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
                    || INT_FAST64_MAX / instance.number_of_items() < wabs
                    || wabs > wlim) {
                s1 = s + 1;
                LOG_FOLD_END(info, "");
                continue;
            } else {
                wmax += s - s_prec;
                wmin += s - s_prec;
            }
        }

        instance.surrogate(info, s-s_prec, k, first);
        LOG_FOLD(info, instance);
        Profit p = ub_dantzig(instance);
        ItemPos b = instance.break_item();

        LOG(info, "b " << b << " p " << p << std::endl);

        if (ub > p) {
            ub = p;
            s_best = s;
        }

        if (b == k && instance.break_capacity() == 0) {
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
    instance.surrogate(info, -s, k);
    LOG_FOLD_END(info, "ub " << ub << " s " << s_best);
    return {ub, s_best};
}

void knapsacksolver::solvesurrelax(SurrelaxData d)
{
    LOG_FOLD_START(d.info, "surrogate relaxation lb " << d.output.lower_bound << std::endl);

    d.instance.sort_partially(d.info);
    ItemPos b = d.instance.break_item();

    // Trivial cases
    if (d.instance.reduced_number_of_items() == 0) {
        Profit ub = (d.instance.reduced_solution() == NULL)? 0: d.instance.reduced_solution()->profit();
        if (d.output.upper_bound == -1 || d.output.upper_bound > ub)
            d.output.update_ub(ub, std::stringstream("surrogate relaxation"), d.info);
        LOG_FOLD_END(d.info, "no items");
        return;
    }
    Profit ub = ub_dantzig(d.instance);
    if (d.instance.break_capacity() == 0 || b == d.instance.last_item() + 1) {
        if (d.output.upper_bound == -1 || d.output.upper_bound > ub)
            d.output.update_ub(ub, std::stringstream("surrogate relaxation"), d.info);
        LOG_FOLD_END(d.info, "dantzig");
        return;
    }

    // Compte s_min and s_max
    // s_min and s_max should ideally be (-)pmax*wmax, but this may cause
    // overflow
    Weight wmax = d.instance.item(d.instance.max_weight_item(d.info)).w;
    Profit pmax = d.instance.item(d.instance.max_profit_item(d.info)).p;
    Weight s_max = (INT_FAST64_MAX / pmax > wmax)?  pmax*wmax:  INT_FAST64_MAX;
    Weight s_min = (INT_FAST64_MAX / pmax > wmax)? -pmax*wmax: -INT_FAST64_MAX;

    if (max_card(d.instance, d.info) == b) {
        UBS o = surrogate_solve(d.instance, d.info, b, 0, s_max, d.end);
        if (*d.end)
            return;
        Profit ub = std::max(o.ub, d.output.lower_bound);
        d.output.update_ub(ub, std::stringstream("surrogate relaxation"), d.info);
        if (d.output.upper_bound == d.output.lower_bound || o.s == 0)
            return;

        Solution sol_sur(d.output.solution.instance());
        d.instance.surrogate(d.info, o.s, b);
        Output output = d.func(d.instance, Info(d.info, false, ""), d.end);
        if (output.solution.profit() != output.upper_bound)
            return;
        sol_sur = output.solution;
        d.output.update_sol(sol_sur, std::stringstream("surrogate instance resolution (lb)"), d.info);
        ub = std::max(sol_sur.profit(), d.output.lower_bound);
        d.output.update_ub(ub, std::stringstream("surrogate instance resolution (ub)"), d.info);
    } else if (min_card(d.instance, d.info, d.output.lower_bound) == b + 1) {
        UBS o = surrogate_solve(d.instance, d.info, b + 1, s_min, 0, d.end);
        if (*d.end)
            return;
        Profit ub = std::max(o.ub, d.output.lower_bound);
        d.output.update_ub(ub, std::stringstream("surrogate relaxation"), d.info);
        if (d.output.upper_bound == d.output.lower_bound || o.s == 0)
            return;

        Solution sol_sur(d.output.solution.instance());
        d.instance.surrogate(d.info, o.s, b + 1);
        Output output = d.func(d.instance, Info(d.info, false, ""), d.end);
        if (output.solution.profit() != output.upper_bound)
            return;
        sol_sur = output.solution;
        d.output.update_sol(sol_sur, std::stringstream("surrogate instance resolution (lb)"), d.info);
        ub = std::max(sol_sur.profit(), d.output.lower_bound);
        d.output.update_ub(ub, std::stringstream("surrogate instance resolution (ub)"), d.info);
    } else {
        Instance instance_2(d.instance);
        UBS o1 = surrogate_solve(d.instance, d.info, b,     0,     s_max, d.end);
        if (*d.end)
            return;
        UBS o2 = surrogate_solve(instance_2, d.info, b + 1, s_min, 0,     d.end);
        if (*d.end)
            return;
        Profit ub = std::max(std::max(o1.ub, o2.ub), d.output.lower_bound);
        d.output.update_ub(ub, std::stringstream("surrogate relaxation"), d.info);
        if (d.output.upper_bound == d.output.lower_bound || o1.s == 0 || o2.s == 0)
            return;

        Solution sol_sur1(d.output.solution.instance());
        d.instance.surrogate(d.info, o1.s, b);
        Output output1 = d.func(d.instance, Info(d.info, false, ""), d.end);
        if (output1.solution.profit() != output1.upper_bound)
            return;
        sol_sur1 = output1.solution;
        d.output.update_sol(sol_sur1, std::stringstream("surrogate instance resolution (lb)"), d.info);
        if (*d.end || d.output.lower_bound == d.output.upper_bound)
            return;

        Solution sol_sur2(d.output.solution.instance());
        instance_2.surrogate(d.info, o2.s, b + 1);
        Output output2 = d.func(instance_2, Info(d.info, false, ""), d.end);
        if (output2.solution.profit() != output2.upper_bound)
            return;
        sol_sur2 = output2.solution;
        d.output.update_sol(sol_sur2, std::stringstream("surrogate instance resolution (lb)"), d.info);

        ub = std::max(std::max(sol_sur1.profit(), sol_sur2.profit()), d.output.lower_bound);
        d.output.update_ub(ub, std::stringstream("surrogate instance resolution (ub)"), d.info);
    }

    LOG_FOLD_END(d.info, "");
}

/******************************************************************************/

Output knapsacksolver::surrelax(const Instance& instance, Info info)
{
    VER(info, "*** surrelax ***" << std::endl);
    Output output(instance, info);

    bool end = false;

    std::function<Output (Instance&, Info, bool*)> func
        = [](Instance& instance, Info info, bool* end)
        {
            (void)end;
            return Output(instance, info);
        };

    solvesurrelax(SurrelaxData{
                .instance = Instance::reset(instance),
                .output   = output,
                .func     = func,
                .end      = &end,
                .info     = Info(info)});

    return output.algorithm_end(info);
}

Output knapsacksolver::surrelax_minknap(const Instance& instance, Info info)
{
    VER(info, "*** surrelax_minknap ***" << std::endl);
    Output output(instance, info);

    bool end = false;

    std::function<Output (Instance&, Info, bool*)> func
        = [](Instance& instance, Info info, bool* end)
        {
            MinknapOptionalParameters p;
            p.info = info;
            p.end = end;
            p.stop_if_end = true;
            p.set_end = false;
            return minknap(instance, p);
        };

    solvesurrelax(SurrelaxData{
                .instance = Instance::reset(instance),
                .output   = output,
                .func     = func,
                .end      = &end,
                .info     = Info(info)});

    return output.algorithm_end(info);
}

