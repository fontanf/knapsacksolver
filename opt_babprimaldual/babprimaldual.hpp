#ifndef BAB_HPP_TKVNPYD8
#define BAB_HPP_TKVNPYD8

#include "../lib/instance.hpp"
#include "../lib/solution.hpp"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

struct BabPDData
{
	BabPDData(const Instance& inst,
			boost::property_tree::ptree* pt = NULL, bool verbose = false):
			instance(inst), sol_curr(inst), sol_best(inst),
			pt(pt), verbose(verbose)
	{
		for (b=1; b<=inst.item_number(); ++b) {
			if (instance.weight(b) > sol_curr.remaining_capacity())
				break;
			sol_curr.set(b, true);
			sol_best.set(b, true);
		}
		a = b-1;
		lb = sol_curr.profit();
	}
	const Instance& instance;
	Solution sol_curr;
	Solution sol_best;
	ItemIdx  a;
	ItemIdx  b  = 0;
	Profit   ub = 0;
	Profit   lb;
	size_t   nodes = 0;
	boost::property_tree::ptree* pt;
	bool verbose;
};

Profit sopt_babprimaldual(BabPDData& data);

#endif /* end of include guard: BAB_HPP_TKVNPYD8 */
