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
    info.verbose("*** forwardgreedynlogn ***\n");

    Solution sol = *ins.break_solution();

    // If all items fit in the knapsack or if the break solution doesn't
    // contain any item, return sol.
    if (ins.break_item() == ins.total_item_number()
            || ins.break_item() == 0) {
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
    if (pmax == -1) {
        sol.set(it_max->j, false);
        sol.set(il_max->j, true);
    }

    return algorithm_end(sol, info);
}

/******************************************************************************/

Solution knapsack::sol_backwardgreedynlogn(const Instance& ins, Info& info)
{
    info.verbose("*** backwardgreedynlogn ***\n");

    Solution sol0 = *ins.break_solution();

    // If all items fit in the knapsack or if the break solution doesn't
    // contain any item, return sol.
    if (ins.break_item() == ins.total_item_number()
            || ins.break_item() == 0) {
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
        return algorithm_end(sol0, info);
    } else {
        sol.set(it_max->j, false);
        sol.set(il_max->j, true);
        return algorithm_end(sol, info);
    }

}

/******************************************************************************/

Solution knapsack::sol_bestgreedynlogn(const Instance& ins, Info& info)
{
    info.verbose("*** bestgreedynlogn ***\n");

    Info info_tmp;
    Solution sol = sol_greedy(ins, info_tmp);
    std::string best = "Greedy";

    Info info_tmp1;
    if (sol.update(sol_forwardgreedynlogn(ins, info_tmp1)))
        best = "ForwardBest";

    Info info_tmp2;
    if (sol.update(sol_backwardgreedynlogn(ins, info_tmp2)))
        best = "BackwardBest";

    info.verbose("Best solution from: " + best);
    info.pt.put("Algorithm.Best", best);
    return algorithm_end(sol, info);
}

