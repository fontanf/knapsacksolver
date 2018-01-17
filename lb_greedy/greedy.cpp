#include "greedy.hpp"

Profit lb_extgreedy(const Instance& instance)
{
	assert(instance.sort_type() == "efficiency" ||
			instance.sort_type() == "partial_efficiency");

	Weight remaining_capacity = instance.capacity();
	Profit p1 = 0;
	for (ItemIdx i=1; i<=instance.item_number(); ++i) {
		Weight wi = instance.weight(i);
		if (remaining_capacity >= wi) {
			remaining_capacity -= wi;
			p1 += instance.profit(i);
		}
	}

	ItemIdx j = instance.max_profit_item();
	Profit p2 = 0;
	if (j != 0) {
		remaining_capacity = instance.capacity() - instance.weight(j);
		p2 = instance.profit(j);
		for (ItemIdx i=1; i<=instance.item_number(); ++i) {
			if (i == j)
				continue;
			Weight wi = instance.weight(i);
			if (remaining_capacity >= wi) {
				remaining_capacity -= wi;
				p2 += instance.profit(i);
			}
		}
	}

	assert(instance.optimum() == 0 || p1 <= instance.optimum());
	assert(instance.optimum() == 0 || p2 <= instance.optimum());
	return (p1 > p2)? p1: p2;
}

Solution sol_extgreedy(const Instance& instance,
		boost::property_tree::ptree* pt, bool verbose)
{
	assert(instance.sort_type() == "efficiency" ||
			instance.sort_type() == "partial_efficiency");

	Solution solution1(instance);
	for (ItemIdx i=1; i<=instance.item_number(); ++i)
		if (solution1.remaining_capacity() >= instance.weight(i))
			solution1.set(i, true);

	Solution solution2(instance);
	ItemIdx j = instance.max_profit_item();
	if (j != 0) {
		solution2.set(j, true);
		for (ItemIdx i=1; i<=instance.item_number(); ++i) {
			if (i == j)
				continue;
			if (solution2.remaining_capacity() >= instance.weight(i))
				solution2.set(i, true);
		}
	}

	assert(instance.optimum() == 0 || solution1.profit() <= instance.optimum());
	assert(instance.optimum() == 0 || solution2.profit() <= instance.optimum());

	Solution solution = (solution1.profit() > solution2.profit())? solution1: solution2;
	if (pt != NULL) {
		pt->put("Solution.Value", solution.profit());
	}
	if (verbose)
		std::cout
			<< "LB " << solution.profit()
			<< " GAP " << instance.optimum() - solution.profit()
			<< std::endl;
	return solution;
}

/******************************************************************************/

Profit lb_greedy_from_to(const Instance& instance, ItemIdx n1, ItemIdx n2, Weight c)
{
	Weight r = c;
	Profit p = 0;
	for (ItemIdx i=n1; i<=n2; ++i) {
		Profit pi = instance.profit(i);
		Weight wi = instance.weight(i);
		if (wi > r)
			continue;
		r -= wi;
		p += pi;
	}
	return p;
}

Profit lb_greedy_except(const Instance& instance,
		ItemIdx first, ItemIdx i1, ItemIdx i2, ItemIdx last, Weight c)
{
	ItemIdx i = first;
	if (i == i1)
		i = i2+1;
	Profit p = 0;
	Weight r = c;
	for (; i<=last; i++) {
		Weight wi = instance.weight(i);
		if (wi <= r) {
			p += instance.profit(i);
			r -= wi;
		}
		if (i == i1-1)
			i = i2;
	}
	return p;
}
