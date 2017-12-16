#include "greedy.hpp"

Profit lb_greedy(const Instance& instance)
{
	Weight remaining_capacity = instance.capacity();
	Profit p1 = 0;
	for (ItemIdx i=1; i<=instance.item_number(); ++i) {
		Weight wi = instance.weight(i);
		if (remaining_capacity >= wi) {
			remaining_capacity -= wi;
			p1 += instance.profit(i);
		}
	}

	ItemIdx j = instance.profit_max();
	remaining_capacity = instance.capacity() - instance.weight(j);
	Profit p2 = instance.profit(j);
	for (ItemIdx i=1; i<=instance.item_number(); ++i) {
		if (i == j)
			continue;
		Weight wi = instance.weight(i);
		if (remaining_capacity >= wi) {
			remaining_capacity -= wi;
			p2 += instance.profit(i);
		}
	}

	assert(instance.optimum() == 0 || p1 <= instance.optimum());
	assert(instance.optimum() == 0 || p2 <= instance.optimum());
	return (p1 > p2)? p1: p2;
}

Solution sol_greedy(const Instance& instance)
{
	Solution solution1(instance);
	for (ItemIdx i=1; i<=instance.item_number(); ++i)
		if (solution1.remaining_capacity() >= instance.weight(i))
			solution1.set(i, true);

	Solution solution2(instance);
	ItemIdx j = instance.profit_max();
	solution2.set(j, true);
	for (ItemIdx i=1; i<=instance.item_number(); ++i) {
		if (i == j)
			continue;
		if (solution2.remaining_capacity() >= instance.weight(i))
			solution2.set(i, true);
	}

	assert(instance.optimum() == 0 || solution1.profit() <= instance.optimum());
	assert(instance.optimum() == 0 || solution2.profit() <= instance.optimum());
	return (solution1.profit() > solution2.profit())? solution1: solution2;
}
