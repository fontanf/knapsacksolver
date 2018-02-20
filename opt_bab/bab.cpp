#include "bab.hpp"

#include "../ub_dantzig/dantzig.hpp"
#include "../ub_surrogate/surrogate.hpp"

#include <iomanip>

Profit sopt_bab(BabData& data)
{
    return sopt_bab_rec(data);
}

/******************************************************************************/

#define DBG(x)
//#define DBG(x) x

void sopt_bab_rec_rec(BabData& d)
{
    // If upperbound is reached, stop
    if (d.ub != 0 && d.sol_best.profit() == d.ub)
        return;

    d.nodes++;

    // Stop condition
    if (d.i == d.instance.item_number()) {
        if (d.sol_curr.profit() > d.lb) {
            d.sol_best = d.sol_curr;
            d.lb = d.sol_best.profit();
            if (Info::verbose(d.info))
                std::cout
                    <<  "LB "   << d.sol_best.profit()
                    << " GAP "  << d.instance.optimum() - d.sol_best.profit()
                    << " NODE " << std::scientific << (double)d.nodes << std::defaultfloat
                    << " TIME " << d.info->elapsed_time()
                    << std::endl;
        }
        return;
    }

    // UB test
    Profit ub = d.sol_curr.profit() + ub_dantzig_from(d.instance, d.i, d.sol_curr.weight());
    if (ub <= d.lb)
        return;

    // Recursive calls
    d.i++;
    if (d.sol_curr.weight() + d.instance.item(d.i-1).w <= d.instance.capacity()) {
        d.sol_curr.set(d.i-1, true);
        sopt_bab_rec_rec(d);
        d.sol_curr.set(d.i-1, false);
    }
    sopt_bab_rec_rec(d);
    d.i--;
}

Profit sopt_bab_rec(BabData& data)
{
    assert(data.instance.sort_type() == "eff");

    sopt_bab_rec_rec(data);

    if (data.info != NULL) {
        data.info->pt.put("Solution.Nodes", data.nodes);
    }
    if (Info::verbose(data.info)) {
        std::cout << "NODES " << data.nodes << std::endl;
    }
    assert(data.instance.check_sopt(data.sol_best));
    return data.sol_best.profit();
}

#undef DBG

/******************************************************************************/

#define DBG(x)
//#define DBG(x) x

Profit sopt_bab_stack(BabData& data)
{
    assert(data.instance.sort_type() == "eff");

    std::stack<ItemIdx> stack;
    stack.push(0);
    size_t list_size_max = 1;
    ItemIdx i_prec = -1;
    while (!stack.empty()) {
        if (data.ub != 0 && data.sol_best.profit() == data.ub)
            break;

        data.nodes++;
        if (stack.size() > list_size_max)
            list_size_max = stack.size();

        data.i = stack.top();
        stack.pop();

        ItemIdx j_max = (i_prec >= data.instance.item_number())? data.instance.item_number()-1: i_prec;
        if (i_prec >= data.i)
            for (ItemIdx j=j_max; j>=data.i-1; --j)
                data.sol_curr.set(j, false);
        i_prec = data.i;

        if (data.i == data.instance.item_number()) {
            if (data.sol_curr.profit() > data.lb) {
                data.sol_best = data.sol_curr;
                data.lb = data.sol_best.profit();
                if (Info::verbose(data.info)) {
                    std::cout
                        <<  "LB "   << data.sol_best.profit()
                        << " GAP "  << data.instance.optimum() - data.sol_best.profit()
                        << " NODE " << std::scientific << (double)data.nodes << std::defaultfloat
                        << " TIME " << data.info->elapsed_time()
                        << std::endl;
                }
            }
            continue;
        }

        if (data.sol_curr.profit() < data.lb && data.sol_curr.profit() + ub_dantzig_from(data.instance, data.i, data.sol_curr.weight()) < data.lb)
            continue;

        stack.push(data.i+1);

        if (data.sol_curr.weight() + data.instance.item(data.i).w <= data.instance.capacity()) {
            data.sol_curr.set(data.i, true);
            stack.push(data.i+1);
        }
    }

    if (data.info != NULL) {
        data.info->pt.put("Solution.Nodes",        data.nodes);
        data.info->pt.put("Solution.MaxStackSize", list_size_max);
    }
    if (Info::verbose(data.info)) {
        std::cout << "NODES " << data.nodes << std::endl;
        std::cout << "MAXSTACKSIZE " << list_size_max << std::endl;
    }
    assert(data.instance.check_sopt(data.sol_best));
    return data.sol_best.profit();
}

#undef DBG
