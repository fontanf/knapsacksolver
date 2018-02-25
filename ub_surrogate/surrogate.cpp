#include "surrogate.hpp"

#include "../ub_dantzig/dantzig.hpp"

#define DBG(x)
//#define DBG(x) x

ItemIdx max_card(const Instance& ins)
{
    if (ins.item_number() == 1)
        return 1;

    std::vector<ItemIdx> index(ins.item_number());
    std::iota(index.begin(), index.end(), 0);
    ItemIdx kp1 = 0;
    ItemIdx f = 0;
    ItemIdx l = ins.item_number() - 1;
    Weight  w = 0;
    Weight  c = ins.capacity();
    while (f < l) {
        ItemIdx pivot = f + 1 + rand() % (l - f); // Select pivot

        iter_swap(index.begin() + pivot, index.begin() + l);
        ItemIdx j = f;
        for (ItemIdx i=f; i<l; ++i) {
            if (ins.item(index[i]).w > ins.item(index[l]).w)
                continue;
            iter_swap(index.begin() + i, index.begin() + j);
            j++;
        }
        iter_swap(index.begin() + j, index.begin() + l);

        Weight w_curr = w;
        for (ItemIdx i=f; i<j; ++i)
            w_curr += ins.item(index[i]).w;

        if (w_curr + ins.item(index[j]).w <= c) {
            f = j;
            w = w_curr;
        } else if (w_curr > c) {
            l = j-1;
        } else {
            f = j;
            break;
        }
    }
    kp1 = f;
    ItemIdx k = kp1 - 1;

    DBG(std::cout << "kmax " << k+1 << std::endl;
    Weight ww = 0;
    for (ItemIdx i=0; i<=k; ++i)
        ww += ins.item(index[i]).w;
    std::cout << "ww " << ww << " <= c " << c << " < ww+wk+1 " << ww + ins.item(index[k+1]).w << std::endl;
    assert(ww <= c && ww + ins.item(index[k+1]).w > c);)

    return k+1;
}

ItemIdx min_card(const Instance& ins, Profit lb)
{
    if (ins.item_number() <= 1)
        return (ins.item(1).p <= lb)? 1: 0;

    std::vector<ItemIdx> index(ins.item_number());
    std::iota(index.begin(), index.end(), 0);
    ItemIdx f = 0;
    ItemIdx l = ins.item_number() - 1;
    ItemIdx km1 = 0;
    Profit p = 0;
    while (f < l) {
        ItemIdx pivot = f + 1 + rand() % (l - f); // Select pivot

        iter_swap(index.begin() + pivot, index.begin() + l);
        ItemIdx j = f;
        for (ItemIdx i=f; i<l; ++i) {
            if (ins.item(index[i]).p < ins.item(index[l]).p)
                continue;
            iter_swap(index.begin() + i, index.begin() + j);
            j++;
        }
        iter_swap(index.begin() + j, index.begin() + l);

        Profit p_curr = p;
        for (ItemIdx i=f; i<j; ++i)
            p_curr += ins.item(index[i]).p;

        if (p_curr > lb) {
            l = j-1;
        } else if (p_curr + ins.item(index[j]).p <= lb) {
            f = j;
            p = p_curr;
        } else {
            km1 = j-1;
            break;
        }
    }
    if (km1 == 0)
        km1 = f;
    ItemIdx k = km1 + 1;

    DBG(std::cout << "kmin " << k+1 << std::endl;
    Weight pp = 0;
    for (ItemIdx i=0; i<k; ++i)
        pp += ins.item(index[i]).p;
    std::cout << "pp " << pp << " <= z " << lb << " < pp+pk " << pp + ins.item(index[k]).p << std::endl;
    assert(pp <= lb && pp + ins.item(index[k]).p > lb);)

    return k+1;
}

void ub_surrogate_solve(Instance& ins, ItemIdx k,
        Weight s_min, Weight s_max, SurrogateOut& out)
{
    out.bound = k;
    Weight  s_prec = 0;
    Weight  s = 0;
    Weight s1 = s_min;
    Weight s2 = s_max;

    while (s1 <= s2) {
        s = (s1 + s2) / 2;
        ins.surrogate(s-s_prec, k);
        Profit  p = ins.break_profit();
        ItemPos b = ins.break_item();
        if (ins.break_capacity() > 0 && ins.break_item() != ins.item_number())
            p += (ins.item(b).p * ins.break_capacity()) / ins.item(b).w;

        DBG(std::cout
                << "s1 " << s1 << " s " << s << " s2 " << s2
                << " g "   << b
                << " ub "  << p
                << " GAP " << p - ins.optimum()
                << std::endl;)
        //DBG(std::cout
                //<< "pbar " << ins.break_profit()
                //<< " r " << ins.break_capacity()
                //<< " c " << ins.capacity()
                //<< std::endl;)
        assert(s_min < 0 || ins.capacity() > 0);

        if (p < out.ub) {
            out.ub         = p;
            out.multiplier = s;
        }

        if (b == k && ins.break_capacity() == 0)
            break;

        if ((s_min == 0 && b >= k) || (s_max == 0 && b >= k)) {
            s1 = s + 1;
        } else {
            s2 = s - 1;
        }
        s_prec = s;
    }
    ins.surrogate(-s, k);
}

SurrogateOut ub_surrogate(const Instance& instance, Profit lb, Info* info)
{
    DBG(std::cout << "SURROGATERELAX..." << std::endl;)
    Instance ins(instance);
    ins.reset();
    ins.sort_partially();

    SurrogateOut out(info);

    // Trivial cases
    if (ins.item_number() == 0)
        return out;
    out.ub = ub_dantzig(ins);
    if (ins.break_capacity() == 0 || ins.break_item() == ins.item_number())
        return out;

    Weight s_max = INT_FAST64_MAX / ins.item_number() / ins.capacity(); // Should ideally be:  maxp*maxp, but may cause overflow (sic)
    Weight s_min = INT_FAST64_MIN / 2; // Should ideally be: -maxp*maxw, but may cause overflow (sic)

    DBG(std::cout
            <<  "z "     << lb
            << " GAP "   << ins.optimum() - lb
            << " b "     << ins.break_item()
            << " s_max " << s_max
            << std::endl;)

    if (max_card(ins) == ins.break_item()) {
        ub_surrogate_solve(ins, ins.break_item(), 0, s_max, out);
        if (info != NULL)
            info->pt.put("UB.Type", "max");
        if (Info::verbose(info))
            std::cout << "MAXCARD" << std::endl;
    } else if (min_card(ins, lb) == ins.break_item() + 1) {
        ub_surrogate_solve(ins, ins.break_item() + 1, s_min, 0, out);
        if (out.ub < lb) {
            assert(ins.optimal_solution()->profit() == 0
                    || lb == ins.optimal_solution()->profit());
            out.ub = lb;
        }
        if (info != NULL)
            info->pt.put("UB.Type", "min");
        if (Info::verbose(info))
            std::cout << "MINCARD" << std::endl;
    } else {
        SurrogateOut out2(info);
        out2.ub = out.ub;
        ub_surrogate_solve(ins, ins.break_item(), 0, s_max, out);
        ub_surrogate_solve(ins, ins.break_item() + 1, s_min, 0, out2);
        DBG(std::cout << "Compute UB..." << std::endl;)
        if (out2.ub > out.ub) {
            out.ub         = out2.ub;
            out.multiplier = out2.multiplier;
            out.bound      = out2.bound;
        }
        if (info != NULL)
            info->pt.put("UB.Type", "maxmin");
        if (Info::verbose(info))
            std::cout << "MAXCARD MINCARD" << std::endl;
    }

    assert(ins.check_ub(out.ub));
    DBG(std::cout << "SURROGATERELAX... END" << std::endl;)
    return out;
}

#undef DBG
