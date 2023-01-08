#include "knapsacksolver/algorithms/surrogate_relaxation.hpp"

#include "knapsacksolver/algorithms/upper_bound_dantzig.hpp"
#include "knapsacksolver/algorithms/dynamic_programming_primal_dual.hpp"

#include <stdlib.h>

using namespace knapsacksolver;

ItemIdx max_card(const Instance& instance FFOT_DBG(FFOT_COMMA Info& info))
{
    FFOT_LOG_FOLD_START(info, "max_card" << std::endl);
    if (instance.reduced_number_of_items() == 1) {
        FFOT_LOG_FOLD_END(info, "1 item");
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

    FFOT_LOG_FOLD_END(info, "k " << k);
    return k;
}

ItemIdx min_card(const Instance& instance, Profit lb FFOT_DBG(FFOT_COMMA Info& info))
{
    FFOT_LOG_FOLD_START(info, "min_card" << std::endl);

    lb -= instance.reduced_solution()->profit();
    if (instance.reduced_number_of_items() <= 1) {
        FFOT_LOG_FOLD_END(info, "1 item");
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

    FFOT_LOG_FOLD_END(info, "k " << k);
    return k;
}

struct UBS
{
    Profit ub;
    Weight s;
};

UBS surrogate_solve(
        Instance& instance,
        ItemIdx k,
        Weight s_min,
        Weight s_max,
        bool* end
        FFOT_DBG(FFOT_COMMA Info& info))
{
    FFOT_LOG_FOLD_START(info, "surrogate_solve k " << k << " s_min " << s_min << " s_max " << s_max << std::endl);
    ItemPos first = instance.first_item();
    Profit ub = upper_bound_dantzig(instance);
    Weight s_prec = 0;
    Weight s = 0;
    Weight s_best = 0;
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
        FFOT_LOG_FOLD_START(info, "s1 " << s1 << " s " << s << " s2 " << s2 << std::endl);

        // Avoid INT overflow
        if (s_min == 0 && s != 0) {
            if (INT_FAST64_MAX / s < k
                    || instance.capacity() > INT_FAST64_MAX - s * k
                    || INT_FAST64_MAX / instance.number_of_items() < wmax+s
                    || wmax + s > wlim) {
                s2 = s - 1;
                FFOT_LOG_FOLD_END(info, "");
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
                FFOT_LOG_FOLD_END(info, "");
                continue;
            } else {
                wmax += s - s_prec;
                wmin += s - s_prec;
            }
        }

        instance.surrogate(s - s_prec, k, first FFOT_DBG(FFOT_COMMA info));
        FFOT_LOG_FOLD(info, instance);
        Profit p = upper_bound_dantzig(instance);
        ItemPos b = instance.break_item();

        FFOT_LOG(info, "b " << b << " p " << p << std::endl);

        if (ub > p) {
            ub = p;
            s_best = s;
        }

        if (b == k && instance.break_capacity() == 0) {
            FFOT_LOG_FOLD_END(info, "");
            break;
        }

        if ((s_min == 0 && b >= k) || (s_max == 0 && b >= k)) {
            s1 = s + 1;
        } else {
            s2 = s - 1;
        }
        s_prec = s;
        FFOT_LOG_FOLD_END(info, "");
    }
    instance.surrogate(-s, k FFOT_DBG(FFOT_COMMA info));
    FFOT_LOG_FOLD_END(info, "ub " << ub << " s " << s_best);
    return {ub, s_best};
}

void knapsacksolver::solve_surrogate_relaxation(
        Instance instance,
        Output& output,
        std::function<Output (Instance&, Info, bool*)> func,
        bool* end,
        Info info)
{
    FFOT_LOG_FOLD_START(info, "surrogate relaxation lb " << output.lower_bound << std::endl);

    instance.sort_partially(FFOT_DBG(info));
    ItemPos b = instance.break_item();

    // Trivial cases
    if (instance.reduced_number_of_items() == 0) {
        Profit ub = (instance.reduced_solution() == NULL)?
            0:
            instance.reduced_solution()->profit();
        if (output.upper_bound == -1 || output.upper_bound > ub) {
            output.update_upper_bound(
                    ub,
                    std::stringstream("surrogate relaxation"),
                    info);
        }
        FFOT_LOG_FOLD_END(info, "no items");
        return;
    }
    Profit ub = upper_bound_dantzig(instance);
    if (instance.break_capacity() == 0 || b == instance.last_item() + 1) {
        if (output.upper_bound == -1 || output.upper_bound > ub) {
            output.update_upper_bound(
                    ub,
                    std::stringstream("surrogate relaxation"),
                    info);
        }
        FFOT_LOG_FOLD_END(info, "dantzig");
        return;
    }

    // Compte s_min and s_max
    // s_min and s_max should ideally be (-)pmax*wmax, but this may cause
    // overflow
    Weight wmax = instance.item(instance.max_weight_item(FFOT_DBG(info))).w;
    Profit pmax = instance.item(instance.max_profit_item(FFOT_DBG(info))).p;
    Weight s_max = (INT_FAST64_MAX / pmax > wmax)?  pmax*wmax:  INT_FAST64_MAX;
    Weight s_min = (INT_FAST64_MAX / pmax > wmax)? -pmax*wmax: -INT_FAST64_MAX;

    if (max_card(instance FFOT_DBG(FFOT_COMMA info)) == b) {
        UBS o = surrogate_solve(instance, b, 0, s_max, end FFOT_DBG(FFOT_COMMA info));
        if (*end)
            return;
        Profit ub = std::max(o.ub, output.lower_bound);
        output.update_upper_bound(
                ub,
                std::stringstream("surrogate relaxation"),
                info);
        if (output.upper_bound == output.lower_bound || o.s == 0)
            return;

        instance.surrogate(o.s, b FFOT_DBG(FFOT_COMMA info));
        Output output0 = func(instance, Info(info, false, ""), end);
        if (output0.solution.profit() != output0.upper_bound)
            return;
        Solution sol_sur(output.solution.instance());
        sol_sur.update(output0.solution);
        output.update_solution(
                sol_sur,
                std::stringstream("surrogate ins res (lb)"),
                info);
        ub = std::max(sol_sur.profit(), output.lower_bound);
        output.update_upper_bound(
                ub,
                std::stringstream("surrogate ins res (ub)"),
                info);
    } else if (min_card(instance, output.lower_bound FFOT_DBG(FFOT_COMMA info)) == b + 1) {
        UBS o = surrogate_solve(instance, b + 1, s_min, 0, end FFOT_DBG(FFOT_COMMA info));
        if (*end)
            return;
        Profit ub = std::max(o.ub, output.lower_bound);
        output.update_upper_bound(
                ub,
                std::stringstream("surrogate relaxation"),
                info);
        if (output.upper_bound == output.lower_bound || o.s == 0)
            return;

        instance.surrogate(o.s, b + 1 FFOT_DBG(FFOT_COMMA info));
        Output output0 = func(instance, Info(info, false, ""), end);
        if (output0.solution.profit() != output0.upper_bound)
            return;
        Solution sol_sur(output.solution.instance());
        sol_sur.update(output0.solution);
        output.update_solution(
                sol_sur,
                std::stringstream("surrogate ins res (lb)"),
                info);
        ub = std::max(sol_sur.profit(), output.lower_bound);
        output.update_upper_bound(
                ub,
                std::stringstream("surrogate ins res (ub)"),
                info);
    } else {
        Instance instance_2(instance);
        UBS o1 = surrogate_solve(
                instance,
                b,
                0,
                s_max,
                end
                FFOT_DBG(FFOT_COMMA info));
        if (*end)
            return;
        UBS o2 = surrogate_solve(
                instance_2,
                b + 1,
                s_min,
                0,
                end
                FFOT_DBG(FFOT_COMMA info));
        if (*end)
            return;
        //std::cout << o1.ub << " " << o2.ub << " " << output.lower_bound << std::endl;
        Profit ub = std::max(std::max(o1.ub, o2.ub), output.lower_bound);
        output.update_upper_bound(
                ub,
                std::stringstream("surrogate relaxation"),
                info);
        if (output.upper_bound == output.lower_bound || o1.s == 0 || o2.s == 0)
            return;

        instance.surrogate(o1.s, b FFOT_DBG(FFOT_COMMA info));
        Output output1 = func(instance, Info(info, false, ""), end);
        if (output1.solution.profit() != output1.upper_bound)
            return;
        Solution sol_sur1(output.solution.instance());
        sol_sur1.update(output1.solution);
        output.update_solution(
                sol_sur1,
                std::stringstream("surrogate ins res (lb)"),
                info);
        if (*end || output.lower_bound == output.upper_bound)
            return;

        instance_2.surrogate(o2.s, b + 1 FFOT_DBG(FFOT_COMMA info));
        Output output2 = func(instance_2, Info(info, false, ""), end);
        if (output2.solution.profit() != output2.upper_bound)
            return;
        Solution sol_sur2(output.solution.instance());
        sol_sur2.update(output2.solution);
        output.update_solution(
                sol_sur2,
                std::stringstream("surrogate ins res (lb)"),
                info);

        ub = std::max(
                std::max(sol_sur1.profit(), sol_sur2.profit()),
                output.lower_bound);
        output.update_upper_bound(
                ub,
                std::stringstream("surrogate ins res (ub)"),
                info);
    }

    FFOT_LOG_FOLD_END(info, "");
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

Output knapsacksolver::surrogate_relaxation(
        const Instance& instance,
        Info info)
{
    init_display(instance, info);
    info.os()
            << "Algorithm" << std::endl
            << "---------" << std::endl
            << "Surrogate Relaxation" << std::endl
            << std::endl;

    Output output(instance, info);

    bool end = false;

    std::function<Output (Instance&, Info, bool*)> func
        = [](Instance& instance, Info info, bool* end)
        {
            (void)end;
            return Output(instance, info);
        };

    solve_surrogate_relaxation(
            Instance::reset(instance),
            output,
            func,
            &end,
            Info(info));

    return output.algorithm_end(info);
}

Output knapsacksolver::solve_surrogate_instance(
        const Instance& instance,
        Info info)
{
    init_display(instance, info);
    info.os()
            << "Algorithm" << std::endl
            << "---------" << std::endl
            << "Solve surrogate instance" << std::endl
            << std::endl;

    Output output(instance, info);

    bool end = false;

    std::function<Output (Instance&, Info, bool*)> func
        = [](Instance& instance, Info info, bool* end)
        {
            DynamicProgrammingPrimalDualOptionalParameters parameters;
            parameters.info = info;
            parameters.end = end;
            parameters.stop_if_end = true;
            parameters.set_end = false;
            return dynamic_programming_primal_dual(instance, parameters);
        };

    solve_surrogate_relaxation(
            Instance::reset(instance),
            output,
            func,
            &end,
            Info(info));

    return output.algorithm_end(info);
}

