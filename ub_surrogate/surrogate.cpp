#include "knapsack/ub_surrogate/surrogate.hpp"

#include "knapsack/ub_dantzig/dantzig.hpp"

#include <stdlib.h>

using namespace knapsack;

//#define DBG(x)
#define DBG(x) x

ItemIdx max_card(const Instance& ins)
{
    DBG(std::cout << "MAXCARD..." << std::endl;)
    if (ins.item_number() == 1) {
        DBG(std::cout << "MAXCARD... END - 1 ITEM" << std::endl;)
        return 1;
    }

    std::vector<ItemIdx> index(ins.total_item_number());
    std::iota(index.begin(), index.end(), 0);
    ItemIdx f = ins.first_item();
    ItemIdx l = ins.last_item();
    Weight  w = 0;
    Weight  c = ins.capacity();
    while (f < l) {
        if (l - f < 128) {
            std::sort(index.begin()+f, index.begin()+l+1,
                    [&ins](ItemPos j1, ItemPos j2) {
                    return ins.item(j1).w < ins.item(j2).w;});
            break;
        }

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
            f = j+1;
            w = w_curr + ins.item(index[j]).w;
        } else if (w_curr > c) {
            l = j-1;
        } else {
            break;
        }
    }

    ItemPos k = ins.reduced_solution()->item_number();
    Weight r = ins.capacity();
    for (ItemPos j=ins.first_item(); j<=ins.last_item(); ++j) {
        if (r < ins.item(index[j]).w) {
            k = j;
            break;
        }
        r -= ins.item(index[j]).w;
    }

    DBG(
    for (ItemPos j=ins.first_item(); j<k; ++j) {
        if (ins.item(index[j]).w <= ins.item(index[k]).w)
            continue;
        std::cout << "J " << j << " " << index[j] << " " << ins.item(index[j]) << std::endl;
        std::cout << "K " << k << " " << index[k] << " " << ins.item(index[k]) << std::endl;
        assert(false);
    }
    for (ItemPos j=k+1; j<=ins.last_item(); ++j)
        assert(ins.item(index[j]).w >= ins.item(index[k]).w);
    )

    DBG(std::cout << "K " << k << std::endl;)
    DBG(std::cout << "MAXCARD... END" << std::endl;)
    return k;
}

ItemIdx min_card(const Instance& ins, Profit lb)
{
    DBG(std::cout << "MINCARD..." << std::endl;)
    lb -= ins.reduced_solution()->profit();
    if (ins.item_number() <= 1)
        return (ins.item(1).p <= lb)? 1: 0;

    std::vector<ItemIdx> index(ins.total_item_number());
    std::iota(index.begin(), index.end(), 0);
    ItemIdx f = ins.first_item();
    ItemIdx l = ins.last_item();
    Profit p = 0;
    while (f < l) {
        if (l - f < 128) {
            std::sort(index.begin()+f, index.begin()+l+1,
                    [&ins](ItemPos j1, ItemPos j2) {
                    return ins.item(j1).p > ins.item(j2).p;});
            break;
        }

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
            f = j+1;
            p = p_curr + ins.item(index[j]).p;
        } else {
            break;
        }
    }

    ItemPos k = -1;
    Profit z = 0;
    for (ItemPos j=ins.first_item(); j<=ins.last_item(); ++j) {
        if (z + ins.item(index[j]).p > lb) {
            k = j+1;
            break;
        }
        z += ins.item(index[j]).p;
    }

    DBG(
    for (ItemPos j=ins.first_item(); j<k-1; ++j)
        assert(ins.item(index[j]).p >= ins.item(index[k]).p);
    for (ItemPos j=k; j<=ins.last_item(); ++j)
        assert(ins.item(index[j]).p <= ins.item(index[k]).p);
    )

    DBG(std::cout << "K " << k << std::endl;)
    DBG(std::cout << "MINCARD... END" << std::endl;)
    return k;
}

void ub_surrogate_solve(Instance& ins, ItemIdx k,
        Weight s_min, Weight s_max, SurrogateOut& out)
{
    out.bound = k;
    ItemPos first = ins.first_item();
    Weight  s_prec = 0;
    Weight  s = 0;
    Weight s1 = s_min;
    Weight s2 = s_max;
    Weight wmax = ins.item(ins.first_item()).w;
    Weight wmin = ins.item(ins.first_item()).w;
    Profit pmax = ins.item(ins.first_item()).p;
    for (ItemPos j=ins.first_item()+1; j<=ins.last_item(); ++j) {
        if (ins.item(j).w > wmax)
            wmax = ins.item(j).w;
        if (ins.item(j).w < wmin)
            wmin = ins.item(j).w;
        if (ins.item(j).p > pmax)
            pmax = ins.item(j).p;
    }
    Weight wabs = wmax;
    Weight wlim = INT_FAST64_MAX / pmax;

    while (s1 <= s2) {
        s = (s1 + s2) / 2;

        // Avoid INT overflow
        if (s_min == 0 && s != 0) {
            if (INT_FAST64_MAX / s < k
                    || ins.total_capacity() > INT_FAST64_MAX - s * k
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

        ins.surrogate(s-s_prec, k, first);
        Profit p = ub_dantzig(ins);
        ItemPos b = ins.break_item();

        DBG(std::cout
                << "S1 " << s1 << " S " << s << " S2 " << s2
                << " B "   << b
                << " N "   << ins.item_number()
                << " UB "  << p
                << " GAP " << p - ins.optimum()
                << std::endl;)
            std::cout << ins << std::endl;
        if (b <= ins.last_item())
            DBG(std::cout
                    << "F " << ins.first_item()
                    << " K " << ins.break_solution()->item_number()
                    << " PBAR " << ins.break_solution()->profit()
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

        if (b == k && ins.break_capacity() == 0)
            break;

        if ((s_min == 0 && b >= k) || (s_max == 0 && b >= k)) {
            s1 = s + 1;
        } else {
            s2 = s - 1;
        }
        s_prec = s;
    }
    ins.surrogate(-s, k, first);
    assert(ins.first_item() == first);
}

SurrogateOut knapsack::ub_surrogate(const Instance& instance, Profit lb, Info* info)
{
    DBG(std::cout << "SURROGATERELAX..." << std::endl;)
    Instance ins(instance);
    ins.sort_partially();
    ItemPos b = ins.break_item();

    SurrogateOut out(info);

    // Trivial cases
    if (ins.item_number() == 0) {
        DBG(std::cout << "SURROGATERELAX... END - NO ITEM" << std::endl;)
        return out;
    }
    out.ub = ub_dantzig(ins);
    if (ins.break_capacity() == 0 || b == ins.last_item() + 1) {
        DBG(std::cout << "SURROGATERELAX... END - NO CAPACITY OR BREAK ITEM" << std::endl;)
        return out;
    }

    // Compte s_min and s_max
    // s_min and s_max should ideally be (-)pmax*wmax, but this may cause
    // overflow
    Weight wmax = ins.max_weight_item().w;
    Profit pmax = ins.max_profit_item().p;
    Weight s_max = (INT_FAST64_MAX / pmax > wmax)?  pmax*wmax:  INT_FAST64_MAX;
    Weight s_min = (INT_FAST64_MAX / pmax > wmax)? -pmax*wmax: -INT_FAST64_MAX;

    DBG(std::cout
            << ins.print_lb(lb)
            << " B "    << b
            << " SMAX " << s_max
            << std::endl;)

    if (max_card(ins) == b) {
        ub_surrogate_solve(ins, b, 0, s_max, out);
        if (info != NULL)
            info->pt.put("UB.Type", "max");
        if (Info::verbose(info))
            std::cout << "MAXCARD" << std::endl;
    } else if (min_card(ins, lb) == b + 1) {
        ub_surrogate_solve(ins, b + 1, s_min, 0, out);
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
        ub_surrogate_solve(ins, b,     0,     s_max, out);
        ub_surrogate_solve(ins, b + 1, s_min, 0,     out2);
        if (out2.ub < lb) {
            out2.ub = lb;
        }
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
