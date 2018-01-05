#include "babprimaldual.hpp"

#include "../ub_dantzig/dantzig.hpp"

#define DBG(x)
//#define DBG(x) x

void sopt_babprimaldual_rec(BabPDData& d)
{
	// If upperbound is reached, stop
	if (d.ub != 0 && d.sol_best.profit() == d.ub)
		return;

	d.nodes++;

	if (d.sol_curr.remaining_capacity() >= 0) { // Try to insert item b

		if (d.b == d.instance.item_number() + 1) { // Stop condition
			if (d.sol_curr.profit() > d.lb) {
				d.sol_best = d.sol_curr;
				d.lb = d.sol_best.profit();
				if (d.verbose)
					std::cout << "New best solution of value " << d.sol_best.profit_orig() << std::endl;
			}
			return;
		}

		Profit ub = d.sol_curr.profit() // UB test
			+ ub_dantzig_from(d.instance, d.b, d.sol_curr.remaining_capacity());
		if (ub <= d.lb)
			return;

		d.b++; // Recurisve calls
		d.sol_curr.set(d.b-1, true);
		sopt_babprimaldual_rec(d);
		d.sol_curr.set(d.b-1, false);
		sopt_babprimaldual_rec(d);
		d.b--;

	} else { // Try to remove item a

		if (d.a == 0) // Stop condition
			return;

		Profit ub = d.sol_curr.profit() // UB test
			+ ub_dantzig_rev_from(d.instance, d.a, d.sol_curr.remaining_capacity());
		if (ub <= d.lb)
			return;

		d.a--; // Recurisve calls
		d.sol_curr.set(d.a+1, false);
		sopt_babprimaldual_rec(d);
		d.sol_curr.set(d.a+1, true);
		sopt_babprimaldual_rec(d);
		d.a++;
	}
}

Profit sopt_babprimaldual(BabPDData& data)
{
	std::cout << data.sol_curr.profit() << std::endl;
	sopt_babprimaldual_rec(data);

	if (data.pt != NULL) {
		data.pt->put("Solution.OPT",   data.sol_best.profit());
		data.pt->put("Solution.Nodes", data.nodes);
	}

	if (data.verbose) {
		std::cout << "OPT: " << data.sol_best.profit() << std::endl;
		std::cout << "EXP: " << data.instance.optimum() << std::endl;
		std::cout << "Nodes: " << data.nodes << std::endl;
	}

	return data.sol_best.profit();
}

#undef DBG
