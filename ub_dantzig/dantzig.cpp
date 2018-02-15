#include "dantzig.hpp"

#define DBG(x)
//#define DBG(x) x

Profit ub_dantzig(const Instance& instance,
        boost::property_tree::ptree* pt, bool verbose)
{
    DBG(std::cout << "UBDANTZIG..." << std::endl;)
    assert(instance.sort_type() == "eff" || instance.sort_type() == "peff");

    ItemPos b = instance.break_item();
    Weight  r = instance.break_capacity();
    Profit  p = instance.reduced_solution()->profit() + instance.break_profit();
    DBG(std::cout << "b " << b << " r " << r << std::endl;)
    if (b != -1 && r > 0)
        p += (instance.item(b).p * r) / instance.item(b).w;

    DBG(std::cout << "UB " << p << std::endl;)
    assert(instance.check_ub(p));
    DBG(std::cout << "UBDANTZIG... END" << std::endl;)
    if (pt != NULL) {
        pt->put("UB.Value", p);
    }
    if (verbose)
        std::cout
            << "UB " << p
            << " GAP " << p - instance.optimum()
            << std::endl;
    return p;
}

#undef DBG

Profit ub_dantzig_from(const Instance& instance, ItemIdx j, Weight c)
{
    assert(instance.sort_type() == "eff");
    ItemIdx i = 1;
    Profit p = 0;
    Weight remaining_capacity = c;
    for (i=j; i<instance.item_number(); i++) {
        Weight wi = instance.item(i).w;
        if (wi > remaining_capacity)
            break;
        p += instance.item(i).p;
        remaining_capacity -= wi;
    }
    if (i != instance.item_number() && remaining_capacity > 0)
        p += (instance.item(i).p * remaining_capacity) / instance.item(i).w;
    return p;
}

Profit ub_dantzig_rev_from(const Instance& instance, ItemIdx j, Weight r)
{
    assert(instance.sort_type() == "eff");
    assert(r <= 0);
    ItemIdx i = j;
    Profit  p = 0;
    for (; i>=0; i--) {
        Weight wi = instance.item(i).w;
        if (wi + r > 0)
            break;
        p -= instance.item(i).p;
        r += wi;
    }
    if (i != -1 && r < 0)
        p -= (instance.item(i).p * -r) / instance.item(i).w;
    assert(p <= 0);
    return p;
}

Profit ub_dantzig_from_to(const Instance& instance, ItemIdx i1, ItemIdx i2, Weight c)
{
    assert(instance.sort_type() == "eff");
    ItemIdx i = i1;
    Profit  p = 0;
    Weight  r = c;
    for (; i<=i2; i++) {
        Weight wi = instance.item(i).w;
        if (wi > r)
            break;
        p += instance.item(i).p;
        r -= wi;
    }
    if (i != i2 + 1 && r > 0)
        p += (instance.item(i).p * r) / instance.item(i).w;
    return p;
}

#define DBG(x)
//#define DBG(x) x

Profit ub_dantzig_except(const Instance& instance,
        ItemIdx n1, ItemIdx i1, ItemIdx i2, ItemIdx n2, Weight c)
{
    assert(instance.sort_type() == "eff");
    DBG(std::cout << "ub_dantzig_except " << n1 << " " << i1 << " " << i2 << " " << n2 << " " << c << std::endl;)
    ItemIdx i = n1;
    if (i == i1)
        i = i2+1;
    Profit  p = 0;
    Weight remaining_capacity = c;
    for (; i<=n2; i++) {
        DBG(std::cout << "i " << i << std::endl;)
        Weight wi = instance.item(i).w;
        if (wi > remaining_capacity)
            break;
        p += instance.item(i).p;
        remaining_capacity -= wi;
        if (i == i1-1)
            i = i2;
    }
    if (i != instance.item_number() && remaining_capacity > 0)
        p += (instance.item(i).p * remaining_capacity) / instance.item(i).w;
    DBG(std::cout << "p " << p << std::endl;)
    return p;
}

#undef DBG
