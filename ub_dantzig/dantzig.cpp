#include "dantzig.hpp"

#define DBG(x)
//#define DBG(x) x

Profit ub_dantzig(const Instance& ins, Info* info)
{
    DBG(std::cout << "UBDANTZIG..." << std::endl;)
    (void)info;
    assert(ins.break_item_found());

    ItemPos b = ins.break_item();
    Weight  r = ins.break_capacity();
    Profit  p = ins.reduced_solution()->profit() + ins.break_profit();
    DBG(std::cout << "b " << b << " r " << r << std::endl;)
    if (b != ins.item_number() && r > 0)
        p += (ins.item(b).p * r) / ins.item(b).w;

    DBG(std::cout << "UB " << p << std::endl;)
    assert(ins.check_ub(p));
    DBG(std::cout << "UBDANTZIG... END" << std::endl;)
    return p;
}

#undef DBG

#define DBG(x)
//#define DBG(x) x

Profit ub_dantzig_from(const Instance& ins, ItemIdx j, const Solution& sol_curr)
{
    return ub_dantzig_from(ins, j, sol_curr.profit(), sol_curr.remaining_capacity());
}

Profit ub_dantzig_from(const Instance& ins, ItemIdx j, Profit p, Weight r)
{
    DBG(std::cout << "UBDANTZIGFROM... j " << j << " r " << sol_curr.remaining_capacity() << std::endl;)
    assert(ins.sorted());
    Profit  u = p;
    ItemPos b;

    //Weight r = sol_curr.remaining_capacity();
    //Item ubitem = {0, ins.isum(j).w + r, 0};
    //b  = ins.ub_item(ubitem);
    //u  = ins.isum(b).p - ins.isum(j).p;
    //r += ins.isum(j).w - ins.isum(b).w;
    //DBG(std::cout << "ubitem " << ins.isum(b) << std::endl;)
    //DBG(std::cout << "u " << u << " b " << b << " r " << r << std::endl;)

    for (b=j; b<ins.item_number(); b++) {
        if (ins.item(b).w > r)
            break;
        u += ins.item(b).p;
        r -= ins.item(b).w;
    }
    DBG(std::cout << "u " << u << " b " << b << " r " << r << std::endl;)

    if (b != ins.item_number() && r > 0)
        u += (ins.item(b).p * r) / ins.item(b).w;
    assert(j > 0 || ins.check_ub(u));
    DBG(std::cout << "UBDANTZIGFROM... END" << std::endl;)
    return u;
}

Profit ub_dantzig_from_rev(const Instance& ins, ItemIdx j, const Solution& sol_curr)
{
    return ub_dantzig_from_rev(ins, j, sol_curr.profit(), sol_curr.remaining_capacity());
}

Profit ub_dantzig_from_rev(const Instance& ins, ItemIdx j, Profit p, Weight r)
{
    assert(ins.sorted());
    assert(r < 0);
    ItemIdx i = j;
    for (; i>=0; i--) {
        if (ins.item(i).w + r > 0)
            break;
        p -= ins.item(i).p;
        r += ins.item(i).w;
    }
    if (i != -1 && r < 0)
        p += (ins.item(i).p * r + 1) / ins.item(i).w - 1;
    return p;
}

#undef DBG

#define DBG(x)
//#define DBG(x) x

Profit ub_dantzig_2_from(const Instance& ins, ItemIdx j, const Solution& sol_curr)
{
    DBG(std::cout << "UBDANTZIGFROM2... j " << j << " r " << sol_curr.remaining_capacity() << std::endl;)
    assert(ins.sorted());
    Profit  u = sol_curr.profit();
    Weight  r = sol_curr.remaining_capacity();
    ItemPos b;

    for (b=j; b<ins.item_number(); b++) {
        if (ins.item(b).w > r)
            break;
        u += ins.item(b).p;
        r -= ins.item(b).w;
    }
    DBG(std::cout << "u " << u << " b " << b << " r " << r << std::endl;)

    if (r == 0 || b == ins.item_number())
        return u;

    Profit pb = ins.item(b).p;
    Profit wb = ins.item(b).w;
    if (b > 0 && b < ins.item_number()-1) {
        DBG(std::cout << 0 << std::endl;)
        Item bm1 = ins.item(b-1);
        Item bp1 = ins.item(b+1);
        Profit ub1 = u +      ( r       * bp1.p    ) / bp1.w;
        Profit ub2 = u + pb + ((r - wb) * bm1.p + 1) / bm1.w - 1;
        u = (ub1 > ub2)? ub1: ub2;
        DBG(std::cout << "UB1 " << ub1 << " UB2 " << ub2 << " UB " << u << std::endl;)
    } else if (b > 0) {
        DBG(std::cout << 1 << std::endl;)
        Item bm1 = ins.item(b-1);
        Profit ub1 = u;
        Profit ub2 = u + pb + ((r - wb) * bm1.p + 1) / bm1.w - 1;
        u = (ub1 > ub2)? ub1: ub2;
        DBG(std::cout << "UB1 " << ub1 << " UB2 " << ub2 << " UB " << u << std::endl;)
    } else {
        DBG(std::cout << 2 << std::endl;)
        u += (r * pb) / wb;
    }
    assert(u <= ub_dantzig_from(ins, j, sol_curr));
    assert(j > 0 || ins.check_ub(u));
    DBG(std::cout << "UBDANTZIGFROM2... END" << std::endl;)
    return u;
}

#undef DBG

#define DBG(x)
//#define DBG(x) x

Profit ub_dantzig_from_to(const Instance& ins,
        ItemIdx i, ItemIdx l, Profit p, Weight r)
{
    assert(ins.sorted());
    DBG(std::cout << "DANTZIGFROMTO... " << i << " " << l << " " << r << std::endl;)
    for (; i<=l; i++) {
        DBG(std::cout << "i " << i << std::endl;)
        if (ins.item(i).w > r)
            break;
        r -= ins.item(i).w;
        p += ins.item(i).p;
    }
    if (i != l+1 && r > 0)
        p += (ins.item(i).p * r) / ins.item(i).w;
    DBG(std::cout << "p " << p << std::endl;)
    DBG(std::cout << "DANTZIGFROMTO... END" << std::endl;)
    return p;
}

Profit ub_dantzig_skip(const Instance& ins,
        ItemIdx f, ItemIdx l, Profit p, Weight r)
{
    assert(ins.sorted());
    DBG(std::cout << "DANTZIGFROMTO... " << i << " " << l << " " << r << std::endl;)
    ItemPos i = 0;
    for (;;i++) {
        if (i == f)
            i = l+1;
        if (i == ins.item_number() || ins.item(i).w > r)
            break;
        r -= ins.item(i).w;
        p += ins.item(i).p;
    }
    if (i != ins.item_number() && r > 0)
        p += (ins.item(i).p * r) / ins.item(i).w;
    DBG(std::cout << "p " << p << std::endl;)
    DBG(std::cout << "DANTZIGFROMTO... END" << std::endl;)
    return p;
}

#undef DBG
