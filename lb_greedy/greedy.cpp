#include "greedy.hpp"

#define DBG(x)
//#define DBG(x) x

Solution sol_break(const Instance& ins,
        boost::property_tree::ptree* pt, bool verbose)
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

Solution sol_greedy(const Instance& ins,
        boost::property_tree::ptree* pt, bool verbose)
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

Solution sol_greedymax(const Instance& ins,
        boost::property_tree::ptree* pt, bool verbose)
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

Solution sol_forwardgreedy(const Instance& ins,
        boost::property_tree::ptree* pt, bool verbose)
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

Solution sol_backwardgreedy(const Instance& ins,
        boost::property_tree::ptree* pt, bool verbose)
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

Solution sol_bestgreedy(const Instance& ins,
        boost::property_tree::ptree* pt, bool verbose)
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
    if (verbose)
        std::cout << "ALG " << best << std::endl;
    DBG(std::cout << "GREEDYBEST... END" << std::endl;)
    return sol;
}

#undef DBG

/******************************************************************************/

Profit lb_greedy_from_to(const Instance& instance, ItemIdx n1, ItemIdx n2, Weight c)
{
    Weight r = c;
    Profit p = 0;
    for (ItemIdx i=n1; i<=n2; ++i) {
        Profit pi = instance.item(i).p;
        Weight wi = instance.item(i).w;
        if (wi > r)
            continue;
        r -= wi;
        p += pi;
    }
    return p;
}

Profit lb_greedy_except(const Instance& instance,
        ItemIdx first, ItemIdx i1, ItemIdx i2, ItemIdx last, Weight c)
{
    ItemIdx i = first;
    if (i == i1)
        i = i2+1;
    Profit p = 0;
    Weight r = c;
    for (; i<=last; i++) {
        Weight wi = instance.item(i).w;
        if (wi <= r) {
            p += instance.item(i).p;
            r -= wi;
        }
        if (i == i1-1)
            i = i2;
    }
    return p;
}
