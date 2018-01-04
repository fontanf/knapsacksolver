#ifndef BAB_HPP_TKVNPYD8
#define BAB_HPP_TKVNPYD8

#include "../lib/instance.hpp"
#include "../lib/solution.hpp"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

struct BabData
{
	BabData(const Instance& inst,
			boost::property_tree::ptree* pt = NULL, bool verbose = false):
		instance(inst), sol_curr(inst), sol_best(inst),
		pt(pt), verbose(verbose)
	{ }
	const Instance& instance;
	Solution sol_curr;
	Solution sol_best;
	ItemIdx i = 1;
	Profit ub = 0;
	Profit lb = 0;
	size_t nodes = 0;
	boost::property_tree::ptree* pt;
	bool verbose;
};

Profit sopt_bab(BabData& data);

Profit sopt_bab_rec(BabData& data);

Profit sopt_bab_stack(BabData& data);

#endif /* end of include guard: BAB_HPP_TKVNPYD8 */
