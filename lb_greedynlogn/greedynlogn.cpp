#include "knapsack/lb_greedynlogn/greedynlogn.hpp"

#include "knapsack/lb_greedy/greedy.hpp"

using namespace knapsack;

std::vector<Item> remove_dominated_items(std::vector<Item>& v)
{
    std::vector<Item> t;
    for (Item item: v) {
        if (t.size() == 0 || (item.w > t.back().w && item.p > t.back().p)) {
            t.push_back(item);
        } else if (item.w == t.back().w && item.p > t.back().p) {
            t.back() = item;
        }
    }
    return t;
}

Solution knapsack::sol_forwardgreedynlogn(const Instance& ins, Info& info)
{
    LOG_FOLD_START(info, "forwardgreedynlogn b " << ins.break_item()
            << " n " << ins.total_item_number() << std::endl;);
    VER(info, "*** forwardgreedynlogn ***" << std::endl);

    Solution sol = *ins.break_solution();

    // If all items fit in the knapsack or if the break solution doesn't
    // contain any item, return sol.
    if (ins.break_item() >= ins.total_item_number() - 1
            || ins.break_item() == 0) {
        LOG_FOLD_END(info, "");
        return algorithm_end(sol, info);
    }

    // Sort taken and left items by weight.
    std::vector<Item> taken;
    std::vector<Item> left;
    for (ItemPos j=0; j<ins.total_item_number(); ++j) {
        Item item = ins.item(j);
        item.j = j;
        if (sol.contains(j)) {
            taken.push_back(item);
        } else {
            left.push_back(item);
        }
    }
    std::sort(taken.begin(), taken.end(),
            [](const Item& i1, const Item& i2) {
            return i1.w < i2.w;});
    std::sort(left.begin(), left.end(),
            [](const Item& i1, const Item& i2) {
            return i1.w < i2.w;});
    std::vector<Item> t = remove_dominated_items(taken);
    std::vector<Item> l = remove_dominated_items(left);

    // Find best exchange.
    Weight r = sol.remaining_capacity();
    Profit pmax = -1;
    auto it_max = t.end();
    auto il_max = l.end();
    auto il = l.begin();
    for (auto it = t.begin(); it != t.end(); ++it) {
        while (il != l.end() && il->w <= it->w + r)
            il++;
        if (il == l.begin())
            continue;
        Profit p = std::prev(il)->p - it->p;
        if (p > pmax) {
            pmax = p;
            it_max = it;
            il_max = std::prev(il);
        }
    }
    if (pmax != -1) {
        sol.set(it_max->j, false);
        sol.set(il_max->j, true);
    }

    LOG_FOLD_END(info, "");
    return algorithm_end(sol, info);
}

/******************************************************************************/

Solution knapsack::sol_backwardgreedynlogn(const Instance& ins, Info& info)
{
    LOG_FOLD_START(info, "backwardgreedynlogn b " << ins.break_item()
            << " n " << ins.total_item_number() << std::endl;);
    VER(info, "*** backwardgreedynlogn ***" << std::endl);

    Solution sol0 = *ins.break_solution();

    // If all items fit in the knapsack or if the break solution doesn't
    // contain any item, return sol.
    if (ins.break_item() >= ins.total_item_number() - 1
            || ins.break_item() == 0) {
        LOG_FOLD_END(info, "");
        return algorithm_end(sol0, info);
    }

    Solution sol = sol0;
    sol.set(ins.break_item(), true);

    // Sort taken and left items by weight.
    std::vector<Item> taken;
    std::vector<Item> left;
    for (ItemPos j=0; j<ins.total_item_number(); ++j) {
        Item item = ins.item(j);
        item.j = j;
        if (sol.contains(j)) {
            taken.push_back(item);
        } else {
            left.push_back(item);
        }
    }
    std::sort(taken.begin(), taken.end(),
            [](const Item& i1, const Item& i2) {
            return i1.w < i2.w;});
    std::sort(left.begin(), left.end(),
            [](const Item& i1, const Item& i2) {
            return i1.w < i2.w;});
    std::vector<Item> t = remove_dominated_items(taken);
    std::vector<Item> l = remove_dominated_items(left);

    // Find best exchange.
    Weight r = sol.remaining_capacity();
    Profit pmax = -ins.item(0).p;
    auto it_max = t.end();
    auto il_max = l.end();
    auto il = l.begin();
    for (auto it = t.begin(); it != t.end(); ++it) {
        if (it->w <= -r)
            continue;
        while (il != l.end() && il->w <= it->w + r)
            il++;
        if (il == l.begin())
            continue;
        Profit p = std::prev(il)->p - it->p;
        if (p > pmax) {
            pmax = p;
            it_max = it;
            il_max = std::prev(il);
        }
    }

    if (it_max == t.end()) {
        LOG_FOLD_END(info, "");
        return algorithm_end(sol0, info);
    } else {
        sol.set(it_max->j, false);
        sol.set(il_max->j, true);
        LOG_FOLD_END(info, "");
        return algorithm_end(sol, info);
    }

}

/******************************************************************************/

Solution knapsack::sol_greedynlogn(const Instance& ins, Info& info)
{
    VER(info, "*** greedynlogn ***" << std::endl);
    LOG_FOLD_START(info, "greedynlogn" << std::endl;);

    Info info_tmp(info.logger);
    Solution sol = sol_greedy(ins, info_tmp);
    std::string best = "greedy";

    Info info_tmp1(info.logger);
    Solution sol_tmp1 = sol_forwardgreedynlogn(ins, info_tmp1);
    if (sol_tmp1.profit() > sol.profit()) {
        sol = sol_tmp1;
        best = "forward";
    }

    Info info_tmp2(info.logger);
    Solution sol_tmp2 = sol_backwardgreedynlogn(ins, info_tmp2);
    if (sol_tmp2.profit() > sol.profit()) {
        sol = sol_tmp2;
        best = "backward";
    }

    VER(info, "Best solution from: " << best << std::endl);
    PUT(info, "Algorithm.Best", best);
    LOG_FOLD_END(info, "");
    return algorithm_end(sol, info);
}

