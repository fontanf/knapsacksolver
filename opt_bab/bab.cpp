#include "bab.hpp"

#include "../ub_dantzig/dantzig.hpp"

Profit opt_bab(BabData& data,
		boost::property_tree::ptree* pt, bool verbose)
{
	return opt_bab_rec(data, pt, verbose);
}

/******************************************************************************/

#define DBG(x)
//#define DBG(x) x

void opt_bab_rec_rec(BabData& d)
{
	DBG(if (d.opt_branch && d.i <= d.instance.item_number()) {
		std::cout << std::endl
			<< "* " << d.i
			<< " pi " << d.instance.profit(d.i)
			<< " wi " << d.instance.weight(d.i)
			<< " xi " << d.instance.instance_orig()->optimum(d.instance.index_orig(d.i))
			<< " r " << d.sol_curr.remaining_capacity() << " " << std::flush;
	} else {
		std::cout << d.i << " " << std::flush;
	})
	DBG(bool opt_branch_mem = d.opt_branch;)

	// If upperbound is reached, stop
	if (d.ub != 0 && d.sol_best.profit() == d.ub)
		return;

	d.nodes++;

	if (d.i == d.instance.item_number() + 1) {
		if (d.sol_curr.profit() > d.lb) {
			d.sol_best = d.sol_curr;
			d.lb = d.sol_best.profit();
			DBG(std::cout << std::endl << "New best solution of value " << d.sol_best.profit_orig() << std::endl;)
		}
		return;
	}

	if (d.sol_curr.profit() < d.lb && d.sol_curr.profit() + ub_dantzig_from(d.instance, d.i, d.sol_curr.remaining_capacity()) < d.lb)
		return;

	if (d.instance.weight(d.i) <= d.sol_curr.remaining_capacity()) {
		DBG(if (opt_branch_mem) {
			d.opt_branch = d.instance.instance_orig()->optimum(d.instance.index_orig(d.i));
			std::cout << "1 " << std::flush;
		})
		d.sol_curr.set(d.i, true);
		d.i++;
		opt_bab_rec_rec(d);
		d.i--;
		d.sol_curr.set(d.i, false);
		DBG(d.opt_branch = opt_branch_mem;)
	}

	DBG(if (opt_branch_mem) {
		d.opt_branch = !(d.instance.instance_orig()->optimum(d.instance.index_orig(d.i)));
		std::cout << "0 " << std::flush;
	})
	d.i++;
	opt_bab_rec_rec(d);
	d.i--;
	DBG(d.opt_branch = opt_branch_mem;)
}

Profit opt_bab_rec(BabData& data,
		boost::property_tree::ptree* pt, bool verbose)
{
	DBG(std::cout << std::endl;)
	DBG(std::cout << "LB: " << data.lb << std::endl;)
	DBG(std::cout << "UB: " << data.ub << std::endl;)
	opt_bab_rec_rec(data);
	DBG(std::cout << std::endl;)
	DBG(std::cout << "OPT: " << data.sol_best.profit() << std::endl;)
	DBG(std::cout << "OPT: " << data.instance.optimum() << std::endl;)
	DBG(std::cout << "OPT Orig: " << data.sol_best.profit_orig() << std::endl;;)
	assert(data.lb == data.instance.optimum());

	if (pt != NULL) {
		pt->put("Solution.OPT",   data.sol_best.profit());
		pt->put("Solution.Nodes", data.nodes);
	}

	if (verbose) {
		std::cout << "OPT: " << data.sol_best.profit() << std::endl;
		std::cout << "Nodes: " << data.nodes << std::endl;
	}

	return data.sol_best.profit();
}

#undef DBG

/******************************************************************************/

#define DBG(x)
//#define DBG(x) x

Profit opt_bab_stack(BabData& data,
		boost::property_tree::ptree* pt, bool verbose)
{
	std::stack<ItemIdx> stack;
	stack.push(1);
	size_t list_size_max = 1;
	ItemIdx i_prec = 0;
	while (!stack.empty()) {
		if (data.ub != 0 && data.sol_best.profit() == data.ub)
			break;

		data.nodes++;
		if (stack.size() > list_size_max)
			list_size_max = stack.size();

		data.i = stack.top();
		stack.pop();

		ItemIdx j_max = (i_prec > data.instance.item_number())? data.instance.item_number(): i_prec;
		if (i_prec >= data.i)
			for (ItemIdx j=j_max; j>=data.i-1; --j)
				data.sol_curr.set(j, false);
		i_prec = data.i;

		if (data.i == data.instance.item_number() + 1) {
			if (data.sol_curr.profit() > data.lb) {
				data.sol_best = data.sol_curr;
				data.lb = data.sol_best.profit();
				if (verbose)
					std::cout << "New best solution of value " << data.sol_best.profit() << " (node " << data.nodes << ")" << std::endl;
			}
			continue;
		}

		if (data.sol_curr.profit() < data.lb && data.sol_curr.profit() + ub_dantzig_from(data.instance, data.i, data.sol_curr.remaining_capacity()) < data.lb)
			continue;

		stack.push(data.i+1);

		if (data.instance.weight(data.i) <= data.sol_curr.remaining_capacity()) {
			data.sol_curr.set(data.i, true);
			stack.push(data.i+1);
		}
	}

	if (pt != NULL) {
		pt->put("Solution.OPT",          data.sol_best.profit());
		pt->put("Solution.Nodes",        data.nodes);
		pt->put("Solution.StackSizeMax", list_size_max);
	}

	if (verbose) {
		std::cout << "OPT: " << data.sol_best.profit() << std::endl;
		std::cout << "Nodes: " << data.nodes << std::endl;
		std::cout << "StackSizeMax: " << list_size_max << std::endl;
	}

	return data.sol_best.profit();
}

#undef DBG
