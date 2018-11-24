#include "knapsack/lb_greedynlogn/greedynlogn.hpp"

#include "knapsack/lb_greedy/greedy.hpp"

using namespace knapsack;

std::vector<Item> remove_dominated_items(std::vector<Item>& v)
{
    std::vector<Item> t;
    for (Item item: v) {
        if (t.size() == 0
                || (item.w > t.back().w && item.p > t.back().p)) {
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

    if (ins.break_item() == ins.total_item_number()
            || ins.break_item() == 0) {
        Solution sol(ins);
        return algorithm_end(sol, info);
    }

    Solution sol = *ins.break_solution();

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

    Weight r = sol.remaining_capacity();
    Profit pmax = -1;
    auto i1_max = t.end();
    auto i2_max = l.end();
    auto i2 = l.begin();
    for (auto i1 = t.begin(); i1 != t.end(); ++i1) {
        while (i2 != l.end() && i2->w <= i1->w + r)
            i2++;
        if (i2 == l.begin())
            continue;
        Profit p = std::prev(i2)->p - i1->p;
        if (p > pmax) {
            pmax = p;
            i1_max = i1;
            i2_max = std::prev(i2);
        }
    }
    if (pmax == -1) {
        sol.set(i1_max->j, false);
        sol.set(i2_max->j, true);
    }

    return algorithm_end(sol, info);
}

/******************************************************************************/

Solution knapsack::sol_backwardgreedynlogn(const Instance& ins, Info& info)
{
    info.verbose("*** backwardgreedynlogn ***\n");

    if (ins.break_item() == ins.total_item_number()
            || ins.break_item() == 0) {
        Solution sol(ins);
        return algorithm_end(sol, info);
    }

    Solution sol = *ins.break_solution();
    Solution sol0 = sol;

    sol.set(ins.break_item(), true);

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

    Weight r = sol.remaining_capacity();
    assert(r < 0);
    Profit pmax = -INT_FAST64_MIN;
    auto i1_max = t.end();
    auto i2_max = l.end();
    auto i2 = l.begin();
    for (auto i1 = t.begin(); i1 != t.end(); ++i1) {
        if (i1->w <= -r)
            continue;
        while (i2 != l.end() && i2->w <= i1->w + r)
            i2++;
        if (i2 == l.begin())
            continue;
        Profit p = std::prev(i2)->p - i1->p;
        if (p > pmax) {
            pmax = p;
            i1_max = i1;
            i2_max = std::prev(i2);
        }
    }

    if (pmax == -INT_FAST64_MIN) {
        return algorithm_end(sol0, info);
    } else {
        sol.set(i1_max->j, false);
        sol.set(i2_max->j, true);
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

