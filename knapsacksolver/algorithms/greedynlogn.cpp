#include "knapsacksolver/algorithms/greedynlogn.hpp"

#include "knapsacksolver/algorithms/greedy.hpp"

using namespace knapsacksolver;

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

/***************************** forwardgreedynlogn *****************************/

Output knapsacksolver::forwardgreedynlogn(const Instance& instance, Info info)
{
    LOG_FOLD_START(info, "forwardgreedynlogn b " << instance.break_item()
            << " n " << instance.number_of_items() << std::endl;);
    VER(info, "*** forwardgreedynlogn ***" << std::endl);
    Output output(instance, info);

    Solution sol = *instance.break_solution();

    // If all items fit in the knapsack or if the break solution doesn't
    // contain any item, return sol.
    if (instance.break_item() >= instance.number_of_items() - 1
            || instance.break_item() == 0) {
        LOG_FOLD_END(info, "");
        return output.algorithm_end(info);
    }

    // Sort taken and left items by weight.
    std::vector<Item> taken;
    std::vector<Item> left;
    for (ItemPos j = 0; j < instance.number_of_items(); ++j) {
        Item item = instance.item(j);
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

    output.update_sol(sol, std::stringstream(), info);
    LOG_FOLD_END(info, "");
    return output.algorithm_end(info);
}

/**************************** backwardgreedynlogn *****************************/

Output knapsacksolver::backwardgreedynlogn(const Instance& instance, Info info)
{
    LOG_FOLD_START(info, "backwardgreedynlogn b " << instance.break_item()
            << " n " << instance.number_of_items() << std::endl;);
    VER(info, "*** backwardgreedynlogn ***" << std::endl);
    Output output(instance, info);

    output.update_sol(*instance.break_solution(), std::stringstream("break"), info);

    // If all items fit in the knapsack or if the break solution doesn't
    // contain any item, return sol.
    if (instance.break_item() >= instance.number_of_items() - 1
            || instance.break_item() == 0) {
        LOG_FOLD_END(info, "");
        return output.algorithm_end(info);
    }

    Solution sol = output.solution;
    sol.set(instance.break_item(), true);

    // Sort taken and left items by weight.
    std::vector<Item> taken;
    std::vector<Item> left;
    for (ItemPos j = 0; j < instance.number_of_items(); ++j) {
        Item item = instance.item(j);
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
    Profit pmax = -instance.item(0).p;
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
        output.update_sol(sol, std::stringstream(), info);
    }

    LOG_FOLD_END(info, "");
    return output.algorithm_end(info);
}

/******************************** greedynlogn *********************************/

Output knapsacksolver::greedynlogn(const Instance& instance, Info info)
{
    VER(info, "*** greedynlogn ***" << std::endl);
    LOG_FOLD_START(info, "greedynlogn" << std::endl;);
    Output output(instance, info);

    auto g_output = greedy(instance);
    output.update_sol(g_output.solution, std::stringstream("greedy"), info);
    std::string best = "greedy";

    auto f_output = forwardgreedynlogn(instance);
    if (output.lower_bound < f_output.lower_bound) {
        output.update_sol(f_output.solution, std::stringstream("forward"), info);
        best = "forward";
    }

    auto b_output = backwardgreedynlogn(instance);
    if (output.lower_bound < b_output.lower_bound) {
        output.update_sol(b_output.solution, std::stringstream("backward"), info);
        best = "backward";
    }

    LOG_FOLD_END(info, "");
    PUT(info, "Algorithm", "Best", best);
    return output.algorithm_end(info);
}

