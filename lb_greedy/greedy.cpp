#include "greedy.hpp"

#define DBG(x)
//#define DBG(x) x

Solution sol_break(const Instance& ins, Info* info)
{
    assert(ins.sort_type() == "eff" || ins.sort_type() == "peff");

    Solution sol = *ins.reduced_solution();
    for (ItemPos i=0; i<ins.item_number(); ++i) {
        if (sol.weight() + ins.item(i).w > ins.total_capacity())
            break;
        sol.set(i, true);
    }

    assert(ins.check_sol(sol));
    return sol;
}

Solution sol_greedy(const Instance& ins, Info* info)
{
    assert(ins.sort_type() == "eff" || ins.sort_type() == "peff");
    DBG(std::cout << "GREEDY..." << std::endl;)

    Solution sol = *ins.reduced_solution();
    for (ItemPos i=0; i<ins.item_number(); ++i) {
        if (sol.weight() + ins.item(i).w > ins.total_capacity())
            continue;
        sol.set(i, true);
    }

    assert(ins.check_sol(sol));
    DBG(std::cout << "GREEDY... END" << std::endl;)
    return sol;
}

Solution sol_greedymax(const Instance& ins, Info* info)
{
    DBG(std::cout << "GREEDYMAX..." << std::endl;)
    assert(ins.sort_type() == "eff" || ins.sort_type() == "peff");
    assert(ins.item_number() > 0);

    ItemPos imax = 0;
    for (ItemPos i=1; i<ins.item_number(); ++i)
        if (ins.item(i).p > ins.item(imax).p)
            imax = i;
    Solution sol = *ins.reduced_solution();
    sol.set(imax, true);
    for (ItemPos i=0; i<ins.item_number(); ++i) {
        if (i == imax)
            continue;
        if (sol.weight() + ins.item(i).w > ins.total_capacity())
            continue;
        sol.set(i, true);
    }
    assert(ins.check_sol(sol));
    DBG(std::cout << "GREEDYMAX... END" << std::endl;)
    return sol;
}

Solution sol_forwardgreedy(const Instance& ins, Info* info)
{
    DBG(std::cout << "GREEDYFW..." << std::endl;)
    assert(ins.sort_type() == "eff" || ins.sort_type() == "peff");
    assert(ins.item_number() > 0);

    Solution sol = *ins.reduced_solution();
    ItemPos b = -1;
    for (ItemPos i=0; i<ins.item_number(); ++i) {
        if (sol.weight() + ins.item(i).w > ins.total_capacity()) {
            b = i;
            break;
        }
        sol.set(i, true);
    }
    if (b == -1)
        return sol;

    ItemPos imax = -1;
    for (ItemPos i=b+1; i<ins.item_number(); ++i)
        if ((imax == -1 && sol.weight() + ins.item(i).w <= ins.total_capacity())
                || (imax != -1 && sol.weight() + ins.item(i).w <= ins.total_capacity() && ins.item(i).p > ins.item(imax).p))
            imax = i;
    if (imax == -1)
        return sol;
    sol.set(imax, true);

    for (ItemPos i=b+1; i<ins.item_number(); ++i) {
        if (sol.weight() + ins.item(i).w > ins.total_capacity())
            continue;
        sol.set(i, true);
    }

    assert(ins.check_sol(sol));
    DBG(std::cout << "GREEDYFW... END" << std::endl;)
    return sol;
}

Solution sol_backwardgreedy(const Instance& ins, Info* info)
{
    DBG(std::cout << "GREEDYBW..." << std::endl;)
    assert(ins.sort_type() == "eff" || ins.sort_type() == "peff");
    assert(ins.item_number() > 0);

    Solution sol = *ins.reduced_solution();
    ItemPos b = -1;
    for (ItemPos i=0; i<ins.item_number(); ++i) {
        sol.set(i, true);
        if (sol.weight() > ins.total_capacity()) {
            b = i;
            break;
        }
    }
    if (b == -1)
        return sol;

    ItemPos imax = -1;
    for (ItemPos i=0; i<=b; ++i)
        if ((imax == -1 && sol.weight() - ins.item(i).w <= ins.total_capacity())
                || (imax != -1 && sol.weight() - ins.item(i).w <= ins.total_capacity() && ins.item(i).p < ins.item(imax).p))
            imax = i;
    assert(imax != -1);
    sol.set(imax, false);

    for (ItemPos i=b+1; i<ins.item_number(); ++i) {
        if (sol.weight() + ins.item(i).w > ins.total_capacity())
            continue;
        sol.set(i, true);
    }

    assert(ins.check_sol(sol));
    DBG(std::cout << "GREEDYBW... END" << std::endl;)
    return sol;
}

Solution sol_bestgreedy(const Instance& ins, Info* info)
{
    DBG(std::cout << "GREEDYBEST..." << std::endl;)
    Solution sol = sol_greedy(ins);
    std::string best = "Greedy";
    if (ins.item_number() == 0)
        return sol;
    if (sol.update(sol_greedymax(ins)))
        best = "Max";
    if (sol.update(sol_forwardgreedy(ins)))
        best = "Forward";
    if (sol.update(sol_backwardgreedy(ins)))
        best = "Backward";
    if (Info::verbose(info))
        std::cout << "ALG " << best << std::endl;
    DBG(std::cout << "GREEDYBEST... END" << std::endl;)
    return sol;
}

#undef DBG

/******************************************************************************/

Profit lb_greedy_from(const Instance& instance,
        ItemPos i, Profit p, Weight r)
{
    return lb_greedy_from_to(instance, i, instance.item_number()-1, p, r);
}

Profit lb_greedy_from_to(const Instance& instance,
        ItemPos i, ItemPos l, Profit p, Weight r)
{
    for (; i<=l; ++i) {
        if (instance.item(i).w > r)
            continue;
        r -= instance.item(i).w;
        p += instance.item(i).p;
    }
    return p;
}

Profit lb_greedy_skip(const Instance& instance,
        ItemPos f, ItemPos l, Profit p, Weight r)
{
    for (ItemPos i=0; ; ++i) {
        if (i == f)
            i = l+1;
        if (i == instance.item_number())
            break;
        if (instance.item(i).w > r)
            continue;
        r -= instance.item(i).w;
        p += instance.item(i).p;
    }
    return p;
}
