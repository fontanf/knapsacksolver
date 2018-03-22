#include "surrogate.hpp"

#include "../ub_dantzig/dantzig.hpp"

#include <stdlib.h>

#define DBG(x)
//#define DBG(x) x

ItemIdx max_card(const Instance& ins)
{
    if (ins.item_number() == 1)
        return 1;

    std::vector<ItemIdx> index(ins.item_number());
    std::iota(index.begin(), index.end(), 0);
    ItemIdx kp1 = 0;
    ItemIdx f = ins.first_item();
    ItemIdx l = ins.last_item();
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

    DBG(std::cout << "KMAX " << k+1 << std::endl;
    Weight ww = 0;
    for (ItemIdx i=0; i<=k; ++i)
        ww += ins.item(index[i]).w;
    std::cout << "WW " << ww << " <= C " << c << " < WW+WK+1 " << ww + ins.item(index[k+1]).w << std::endl;
    assert(ww <= c && ww + ins.item(index[k+1]).w > c);)

    return k+1;
}

ItemIdx min_card(const Instance& ins, Profit lb)
{
    if (ins.item_number() <= 1)
        return (ins.item(1).p <= lb)? 1: 0;

    std::vector<ItemIdx> index(ins.item_number());
    std::iota(index.begin(), index.end(), 0);
    ItemIdx f = ins.first_item();
    ItemIdx l = ins.last_item();
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

    DBG(std::cout << "KMIN " << k+1 << std::endl;
    Weight pp = 0;
    for (ItemIdx i=0; i<k; ++i)
        pp += ins.item(index[i]).p;
    std::cout << "PP " << pp << " <= Z " << lb << " < PP+PK " << pp + ins.item(index[k]).p << std::endl;
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
    Weight wmax = ins.item(0).w;
    Weight wmin = ins.item(0).w;
    Profit pmax = ins.item(0).p;
    for (ItemPos i=1; i<ins.item_number(); ++i) {
        if (ins.item(i).w > wmax)
            wmax = ins.item(i).w;
        if (ins.item(i).w < wmin)
            wmin = ins.item(i).w;
        if (ins.item(i).p > pmax)
            pmax = ins.item(i).p;
    }
    Weight wabs = wmax;
    Weight wlim = INT_FAST64_MAX / pmax;

    while (s1 <= s2) {
        s = (s1 + s2) / 2;

        // Avoid INT overflow
        if (s_min == 0 && s != 0) {
            if (INT_FAST64_MAX / s < k
                    || ins.capacity() > INT_FAST64_MAX - s * k
                    || INT_FAST64_MAX / ins.total_item_number() < wmax+s
                    || wmax + s > wlim) {
                DBG(std::cout << "S2 " << s2 << " => " << s-1 << std::endl;)
                s2 = s - 1;
                continue;
            } else {
                wmax += s - s_prec;
            }
        }
        if (s_max == 0 && s != 0) {
            wabs = (wmax+s > -wmin+s)? wmax+s: wmin+s;
            if (INT_FAST64_MAX / -s < k
                    || INT_FAST64_MAX / ins.total_item_number() < wabs
                    || wabs > wlim) {
                DBG(std::cout << "S1 " << s1 << " => " << s+1 << std::endl;)
                s1 = s + 1;
                continue;
            } else {
                wmax += s - s_prec;
                wmin += s - s_prec;
            }
        }

        ins.surrogate(s-s_prec, k);
        Profit  p = ins.break_solution()->profit();
        ItemPos b = ins.break_item();
        if (ins.break_capacity() > 0 && ins.break_item() != ins.item_number())
            p += (ins.item(b).p * ins.break_capacity()) / ins.item(b).w;
        ItemPos g = ins.reduced_solution()->item_number() + b;

        DBG(std::cout
                << "S1 " << s1 << " S " << s << " S2 " << s2
                << " G "   << g
                << " B "   << ins.break_item()
                << " N "   << ins.item_number()
                << " UB "  << p
                << " GAP " << p - ins.optimum()
                << std::endl;)
        if (b != ins.item_number())
            DBG(std::cout
                    << "PBAR " << ins.break_solution()->profit()
                    << " R " << ins.break_capacity()
                    << " PB " << ins.item(b).p
                    << " WB " << ins.item(b).w
                    << " C " << ins.capacity()
                    << std::endl;)
        assert(s_min < 0 || ins.capacity() > 0);

        if (p < out.ub) {
            out.ub         = p;
            out.multiplier = s;
        }

        if (g == k && ins.break_capacity() == 0)
            break;

        if ((s_min == 0 && g >= k) || (s_max == 0 && g >= k)) {
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
    ins.sort_partially();

    SurrogateOut out(info);

    // Trivial cases
    if (ins.item_number() == 0)
        return out;
    out.ub = ub_dantzig(ins);
    if (ins.break_capacity() == 0 || ins.break_item() == ins.item_number())
        return out;

    // Compte s_min and s_max
    // s_min and s_max should ideally be (-)pmax*wmax, but this may cause
    // overflow
    Weight wmax = ins.max_weight_item().w;
    Profit pmax = ins.max_profit_item().p;
    Weight s_max = (INT_FAST64_MAX / pmax > wmax)?  pmax*wmax:  INT_FAST64_MAX;
    Weight s_min = (INT_FAST64_MAX / pmax > wmax)? -pmax*wmax: -INT_FAST64_MAX;

    DBG(std::cout
            <<  "Z "    << lb
            << " GAP "  << ins.optimum() - lb
            << " B "    << ins.break_item()
            << " SMAX " << s_max
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
            assert(ins.optimal_solution() == NULL || lb == ins.optimal_solution()->profit());
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
        DBG(std::cout << "COMPUTE UB..." << std::endl;)
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
