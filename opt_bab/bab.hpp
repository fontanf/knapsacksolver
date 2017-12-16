#ifndef BAB_HPP_TKVNPYD8
#define BAB_HPP_TKVNPYD8

#include "../lib/instance.hpp"
#include "../lib/solution.hpp"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

struct BabData
{
	BabData(const Instance& inst):
		instance(inst),
		sol_curr(inst), sol_best(inst),
		n(instance.item_number()), c(instance.capacity()),
		opt_instance(instance.optimum())
	{ }
	const Instance& instance;
	Solution sol_curr;
	Solution sol_best;
	ItemIdx  n;
	ItemIdx  i = 1;
	Weight   c;
	Profit   opt_instance = -1;
	Profit   ub = 0;
	Profit   lb = 0;
	size_t   nodes = 0;
	bool opt_branch = true; // debug
};

Profit opt_bab(BabData& data,
		boost::property_tree::ptree* pt = NULL, bool verbose = false);

Profit opt_bab_rec(BabData& data,
		boost::property_tree::ptree* pt = NULL, bool verbose = false);

Profit opt_bab_stack(BabData& data,
		boost::property_tree::ptree* pt = NULL, bool verbose = false);

#endif /* end of include guard: BAB_HPP_TKVNPYD8 */
