#include "knapsacksolver/algorithms/surrogate_relaxation.hpp"

#include "knapsacksolver/algorithm_formatter.hpp"
#include "knapsacksolver/algorithms/upper_bound_dantzig.hpp"
#include "knapsacksolver/algorithms/dynamic_programming_primal_dual.hpp"

#include <stdlib.h>

using namespace knapsacksolver;

ItemId maximum_cardinality(
        const Instance& instance)
{
    if (instance.number_of_items() == 1) {
        return 1;
    }

    std::vector<ItemId> sorted_items(instance.number_of_items());
    std::iota(sorted_items.begin(), sorted_items.end(), 0);
    ItemId f = 0;
    ItemId l = instance.number_of_items() - 1;
    Weight w = 0;
    while (f < l) {
        if (l - f < 128) {
            std::sort(
                    sorted_items.begin() + f,
                    sorted_items.begin() + l + 1,
                    [&instance](ItemPos item_id_1, ItemPos item_id_2) {
                    return instance.item(item_id_1).weight < instance.item(item_id_2).weight;});
            break;
        }

        ItemPos pivot_item_pos = f + 1 + (l - f) / 2;

        iter_swap(sorted_items.begin() + pivot_item_pos, sorted_items.begin() + l);
        ItemId item_pos = f;
        for (ItemPos item_pos_2 = f; item_pos_2 < l; ++item_pos_2) {
            if (instance.item(sorted_items[item_pos_2]).weight > instance.item(sorted_items[l]).weight)
                continue;
            iter_swap(sorted_items.begin() + item_pos_2, sorted_items.begin() + item_pos);
            item_pos++;
        }
        iter_swap(sorted_items.begin() + item_pos, sorted_items.begin() + l);

        Weight w_curr = w;
        for (ItemId i = f; i < item_pos; ++i)
            w_curr += instance.item(sorted_items[i]).weight;

        if (w_curr + instance.item(sorted_items[item_pos]).weight
                <= instance.capacity()) {
            f = item_pos + 1;
            w = w_curr + instance.item(sorted_items[item_pos]).weight;
        } else if (w_curr > instance.capacity()) {
            l = item_pos - 1;
        } else {
            break;
        }
    }

    ItemPos k = 0;
    Weight weight = 0;
    for (ItemPos item_pos = 0;
            item_pos < instance.number_of_items();
            ++item_pos) {
        weight += instance.item(sorted_items[item_pos]).weight;
        if (weight > instance.capacity()) {
            k = item_pos;
            break;
        }
    }

    return k;
}

ItemId minimum_cardinality(
        const Instance& instance,
        Profit lower_bound)
{
    if (instance.number_of_items() <= 1) {
        return 1;
    }

    std::vector<ItemId> sorted_items(instance.number_of_items());
    std::iota(sorted_items.begin(), sorted_items.end(), 0);
    ItemId f = 0;
    ItemId l = instance.number_of_items() - 1;
    Profit p = 0;
    while (f < l) {
        if (l - f < 128) {
            std::sort(
                    sorted_items.begin() + f,
                    sorted_items.begin() + l + 1,
                    [&instance](ItemPos item_id_1, ItemPos item_id_2) {
                    return instance.item(item_id_1).profit > instance.item(item_id_2).profit;});
            break;
        }

        ItemPos pivot_item_pos = f + 1 + (l - f) / 2;

        iter_swap(sorted_items.begin() + pivot_item_pos, sorted_items.begin() + l);
        ItemId item_pos = f;
        for (ItemId item_pos_2 = f; item_pos_2 < l; ++item_pos_2) {
            if (instance.item(sorted_items[item_pos_2]).profit < instance.item(sorted_items[l]).profit)
                continue;
            iter_swap(sorted_items.begin() + item_pos_2, sorted_items.begin() + item_pos);
            item_pos++;
        }
        iter_swap(sorted_items.begin() + item_pos, sorted_items.begin() + l);

        Profit p_curr = p;
        for (ItemId item_pos_2 = f; item_pos_2 < item_pos; ++item_pos_2)
            p_curr += instance.item(sorted_items[item_pos_2]).profit;

        if (p_curr > lower_bound) {
            l = item_pos - 1;
        } else if (p_curr + instance.item(sorted_items[item_pos]).profit <= lower_bound) {
            f = item_pos + 1;
            p = p_curr + instance.item(sorted_items[item_pos]).profit;
        } else {
            break;
        }
    }

    ItemPos k = -1;
    Profit profit = 0;
    for (ItemPos item_pos = 0; item_pos < instance.number_of_items(); ++item_pos) {
        profit += instance.item(sorted_items[item_pos]).profit;
        if (profit > lower_bound) {
            k = item_pos + 1;
            break;
        }
    }

    return k;
}

struct UBS
{
    Profit ub;
    Weight s;
};

UBS surrogate_solve(
        const Instance& instance,
        ItemId k,
        Weight s_min,
        Weight s_max)
{
    FFOT_LOG_FOLD_START(logger, "surrogate_solve k " << k << " s_min " << s_min << " s_max " << s_max << std::endl);
    ItemPos first = 0;
    Profit ub = upper_bound_dantzig(instance).bound;
    Weight s_prec = 0;
    Weight s = 0;
    Weight s_best = 0;
    Weight s1 = s_min;
    Weight s2 = s_max;
    Weight wabs = instance.highest_item_profit();
    Weight wlim = INT_FAST64_MAX / instance.highest_item_profit();

    while (s1 <= s2) {
        s = (s1 + s2) / 2;
        FFOT_LOG_FOLD_START(logger, "s1 " << s1 << " s " << s << " s2 " << s2 << std::endl);

        // Avoid INT overflow
        if (s_min == 0 && s != 0) {
            if (INT_FAST64_MAX / s < k
                    || instance.capacity() > INT_FAST64_MAX - s * k
                    || INT_FAST64_MAX / instance.number_of_items() < wmax + s
                    || wmax + s > wlim) {
                s2 = s - 1;
                FFOT_LOG_FOLD_END(logger, "");
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
                FFOT_LOG_FOLD_END(logger, "");
                continue;
            } else {
                wmax += s - s_prec;
                wmin += s - s_prec;
            }
        }

        instance.surrogate(s - s_prec, k, first FFOT_DBG(FFOT_COMMA logger));
        Profit p = upper_bound_dantzig(instance);
        ItemPos b = instance.break_item();

        if (ub > p) {
            ub = p;
            s_best = s;
        }

        if (b == k && instance.break_capacity() == 0) {
            break;
        }

        if ((s_min == 0 && b >= k) || (s_max == 0 && b >= k)) {
            s1 = s + 1;
        } else {
            s2 = s - 1;
        }
        s_prec = s;
    }

    return {ub, s_best};
}

Output knapsacksolver::surrogate_relaxation(
        const Instance& instance,
        const SurrogateRelaxationParameters& parameters)
{
    Output output(instance);
    AlgorithmFormatter algorithm_formatter(parameters, output);
    algorithm_formatter.start("Surrogate relaxation");
    FFOT_DBG(std::shared_ptr<optimizationtools::Logger> logger = parameters.get_logger();)

    FFOT_LOG_FOLD_START(logger, "surrogate relaxation lb " << output.lower_bound << std::endl);

    instance.sort_partially(FFOT_DBG(info));
    ItemPos b = instance.break_item();

    // Trivial cases
    if (instance.reduced_number_of_items() == 0) {
        Profit ub = (instance.reduced_solution() == NULL)?
            0:
            instance.reduced_solution()->profit();
        if (output.upper_bound == -1 || output.upper_bound > ub) {
            algorithm_formatter.update_bound(
                    ub,
                    "surrogate relaxation");
        }
        FFOT_LOG_FOLD_END(logger, "no items");
        return;
    }
    Profit ub = upper_bound_dantzig(instance);
    if (instance.break_capacity() == 0 || b == instance.last_item() + 1) {
        if (output.upper_bound == -1 || output.upper_bound > ub) {
            algorithm_formatter.update_bound(
                    ub,
                    "surrogate relaxation");
        }
        FFOT_LOG_FOLD_END(logger, "dantzig");
        return;
    }

    // Compte s_min and s_max
    // s_min and s_max should ideally be (-)pmax*wmax, but this may cause
    // overflow
    Weight wmax = instance.item(instance.max_weight_item(FFOT_DBG(info))).w;
    Profit pmax = instance.item(instance.max_profit_item(FFOT_DBG(info))).p;
    Weight s_max = (INT_FAST64_MAX / pmax > wmax)?  pmax*wmax:  INT_FAST64_MAX;
    Weight s_min = (INT_FAST64_MAX / pmax > wmax)? -pmax*wmax: -INT_FAST64_MAX;

    if (maximum_cardinality(instance FFOT_DBG(FFOT_COMMA info)) == b) {
        UBS o = surrogate_solve(instance, b, 0, s_max, end FFOT_DBG(FFOT_COMMA info));
        if (*end)
            return;
        Profit ub = std::max(o.ub, output.lower_bound);
        algorithm_formatter.update_bound(
                ub,
                "surrogate relaxation");
        if (output.upper_bound == output.lower_bound || o.s == 0)
            return;

        instance.surrogate(o.s, b FFOT_DBG(FFOT_COMMA info));
        Output output0 = func(instance, Info(info, false, ""), end);
        if (output0.solution.profit() != output0.upper_bound)
            return;
        Solution sol_sur(output.solution.instance());
        sol_sur.update(output0.solution);
        algorithm_formatter.update_solution(
                sol_sur,
                "surrogate ins res (lb)");
        ub = std::max(sol_sur.profit(), output.lower_bound);
        algorithm_formatter.update_bound(
                ub,
                "surrogate ins res (ub)");
    } else if (minimum_cardinality(instance, output.lower_bound FFOT_DBG(FFOT_COMMA info)) == b + 1) {
        UBS o = surrogate_solve(instance, b + 1, s_min, 0, end FFOT_DBG(FFOT_COMMA info));
        if (*end)
            return;
        Profit ub = std::max(o.ub, output.lower_bound);
        algorithm_formatter.update_bound(
                ub,
                "surrogate relaxation");
        if (output.upper_bound == output.lower_bound || o.s == 0)
            return;

        instance.surrogate(o.s, b + 1 FFOT_DBG(FFOT_COMMA info));
        Output output0 = func(instance, Info(info, false, ""), end);
        if (output0.solution.profit() != output0.upper_bound)
            return;
        Solution sol_sur(output.solution.instance());
        sol_sur.update(output0.solution);
        algorithm_formatter.update_solution(sol_sur, "surrogate ins res (lb)");
        ub = std::max(sol_sur.profit(), output.lower_bound);
        algorithm_formatter.update_bound(ub, "surrogate ins res (ub)");
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
        algorithm_formatter.update_bound(ub, "surrogate relaxation");
        if (output.upper_bound == output.lower_bound || o1.s == 0 || o2.s == 0)
            return;

        instance.surrogate(o1.s, b FFOT_DBG(FFOT_COMMA info));
        Output output1 = func(instance, Info(info, false, ""), end);
        if (output1.solution.profit() != output1.upper_bound)
            return;
        Solution sol_sur1(output.solution.instance());
        sol_sur1.update(output1.solution);
        algorithm_formatter.update_solution(sol_sur1, "surrogate ins res (lb)");
        if (*end || output.lower_bound == output.upper_bound)
            return;

        instance_2.surrogate(o2.s, b + 1 FFOT_DBG(FFOT_COMMA info));
        Output output2 = func(instance_2, Info(info, false, ""), end);
        if (output2.solution.profit() != output2.upper_bound)
            return;
        Solution sol_sur2(output.solution.instance());
        sol_sur2.update(output2.solution);
        algorithm_formatter.update_solution(sol_sur2, "surrogate ins res (lb)");

        ub = std::max(
                std::max(sol_sur1.profit(), sol_sur2.profit()),
                output.lower_bound);
        algorithm_formatter.update_bound(ub, "surrogate ins res (ub)");
    }

    FFOT_LOG_FOLD_END(logger, "");
}
