#include "babprimaldual.hpp"

#include "../ub_dantzig/dantzig.hpp"

#define DBG(x)
//#define DBG(x) x

void sopt_babprimaldual_rec(BabPDData& d)
{
    DBG(std::cout << "BABPRIMALDUALREC... a " << d.a << " b " << d.b << " r " << d.sol_curr.remaining_capacity() << std::endl;)

    // If upperbound is reached, stop
    if (d.ub != 0 && d.sol_best.profit() == d.ub)
        return;

    d.nodes++;

    if (d.sol_curr.remaining_capacity() >= 0) { // Try to insert item b

        if (d.b == d.instance.item_number()) { // Stop condition
            d.update_best_solution();
            return;
        }

        // UB test
        Profit ub = 0;
        if (d.ub_type == "trivial") {
            ub = ub_trivial_from(d.instance, d.b, d.sol_curr);
        } else if (d.ub_type == "dantzig") {
            ub = ub_dantzig_from(d.instance, d.b, d.sol_curr);
        }
        if (ub <= d.lb)
            return;

        d.b++; // Recurisve calls
        d.sol_curr.set(d.b-1, true);
        sopt_babprimaldual_rec(d);
        d.sol_curr.set(d.b-1, false);
        sopt_babprimaldual_rec(d);
        d.b--;

    } else { // Try to remove item a

        if (d.a == -1) // Stop condition
            return;

        // UB test
        Profit ub = 0;
        if (d.ub_type == "trivial") {
            ub = ub_trivial_from_rev(d.instance, d.a, d.sol_curr);
        } else if (d.ub_type == "dantzig") {
            ub = ub_dantzig_from_rev(d.instance, d.a, d.sol_curr);
        }
        if (ub <= d.lb)
            return;

        d.a--; // Recurisve calls
        d.sol_curr.set(d.a+1, false);
        sopt_babprimaldual_rec(d);
        d.sol_curr.set(d.a+1, true);
        sopt_babprimaldual_rec(d);
        d.a++;
    }

    DBG(std::cout << "BABPRIMALDUALREC... END a " << d.a << " b " << d.b << " r " << d.sol_curr.remaining_capacity() << std::endl;)
}

Profit sopt_babprimaldual(BabPDData& data)
{
    assert(data.instance.sort_type() == "eff" ||
            data.instance.sort_type() == "peff");

    if (data.break_item == data.instance.item_number()) {
        data.update_best_solution();
    } else {
        sopt_babprimaldual_rec(data);
    }

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
