#include "greedy.hpp"

Profit lb_greedy(const Instance& instance)
{
	Weight remaining_capacity = instance.capacity();
	Profit p = 0;
	for (ItemIdx i=instance.item_number(); i>0; --i) {
		Weight wi = instance.weight(i);
		if (remaining_capacity >= wi) {
			remaining_capacity -= wi;
			p += instance.profit(i);
		}
	}
	assert(instance.optimum() == 0 || p <= instance.optimum());
	return p;
}

Solution sol_greedy(const Instance& instance)
{
	Solution solution(instance);
	for (ItemIdx i=instance.item_number(); i>0; --i)
		if (solution.remaining_capacity() >= instance.weight(i))
			solution.set(i, true);
	assert(instance.optimum() == 0 || solution.profit() <= instance.optimum());
	return solution;
}
