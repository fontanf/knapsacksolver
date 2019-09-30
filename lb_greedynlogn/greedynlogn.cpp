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

/*************************** sol_forwardgreedynlogn ***************************/

knapsack::Output knapsack::sol_forwardgreedynlogn(const Instance& ins, Info info)
{
    LOG_FOLD_START(info, "forwardgreedynlogn b " << ins.break_item()
            << " n " << ins.total_item_number() << std::endl;);
    VER(info, "*** forwardgreedynlogn ***" << std::endl);
    knapsack::Output output(ins);
    init_display(output.lower_bound, output.upper_bound, info);

    Solution sol = *ins.break_solution();

    // If all items fit in the knapsack or if the break solution doesn't
    // contain any item, return sol.
    if (ins.break_item() >= ins.total_item_number() - 1
            || ins.break_item() == 0) {
        algorithm_end(output, info);
        LOG_FOLD_END(info, "");
        return output;
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

    update_sol(output, sol, std::stringstream(), info);
    algorithm_end(output, info);
    LOG_FOLD_END(info, "");
    return output;
}

/************************** sol_backwardgreedynlogn ***************************/

knapsack::Output knapsack::sol_backwardgreedynlogn(const Instance& ins, Info info)
{
    LOG_FOLD_START(info, "backwardgreedynlogn b " << ins.break_item()
            << " n " << ins.total_item_number() << std::endl;);
    VER(info, "*** backwardgreedynlogn ***" << std::endl);
    knapsack::Output output(ins);
    init_display(output.lower_bound, output.upper_bound, info);

    update_sol(output, *ins.break_solution(), std::stringstream("break"), info);

    // If all items fit in the knapsack or if the break solution doesn't
    // contain any item, return sol.
    if (ins.break_item() >= ins.total_item_number() - 1
            || ins.break_item() == 0) {
        LOG_FOLD_END(info, "");
        algorithm_end(output, info);
        return output;
    }

    Solution sol = output.solution;
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

    if (it_max != t.end()) {
        sol.set(it_max->j, false);
        sol.set(il_max->j, true);
        update_sol(output, sol, std::stringstream(), info);
    }

    algorithm_end(output, info);
    LOG_FOLD_END(info, "");
    return output;

}

/****************************** sol_greedynlogn *******************************/

knapsack::Output knapsack::sol_greedynlogn(const Instance& ins, Info info)
{
    VER(info, "*** greedynlogn ***" << std::endl);
    LOG_FOLD_START(info, "greedynlogn" << std::endl;);
    knapsack::Output output(ins);
    init_display(output.lower_bound, output.upper_bound, info);

    auto g_output = sol_greedy(ins);
    update_sol(output, g_output.solution, std::stringstream("greedy"), info);
    std::string best = "greedy";

    auto f_output = sol_forwardgreedynlogn(ins);
    if (output.lower_bound < f_output.lower_bound) {
        update_sol(output, f_output.solution, std::stringstream("forward"), info);
        best = "forward";
    }

    auto b_output = sol_backwardgreedynlogn(ins);
    if (output.lower_bound < b_output.lower_bound) {
        update_sol(output, b_output.solution, std::stringstream("backward"), info);
        best = "backward";
    }

    PUT(info, "Algorithm", "Best", best);
    LOG_FOLD_END(info, "");
    algorithm_end(output, info);
    return output;
}

