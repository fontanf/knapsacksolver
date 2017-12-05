#include "Instance.hpp"
#include "Solution.hpp"
#include "Tools.hpp"

Profit lower_bound_greedy(const Instance& instance)
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
	return p;
}

Solution solution_greedy(const Instance& instance)
{
	Solution solution(instance);
	for (ItemIdx i=instance.item_number(); i>0; --i)
		if (solution.remaining_capacity() >= instance.weight(i))
			solution.set(i, true);
	return solution;
}

Profit opt_bellman_dp(const Instance& instance)
{
	Weight c = instance.capacity();
	Profit* values = new Profit[c+1];
	for (Weight w=c+1; w-->0;)
		values[w] = 0;
	for (ItemIdx i=1; i<=instance.item_number(); ++i) {
		Weight wi = instance.weight(i);
		for (Weight w=c+1; w-->0;)
			if (w >= wi)
				if (values[w-wi] + instance.profit(i) > values[w])
					values[w] = values[w-wi] + instance.profit(i);
	}
	Profit opt = values[c];
	delete[] values; // Free memory
	return opt;
}

Profit local_search(Solution& solution)
{
	const Instance& instance = solution.instance();
	bool b = true;
	ItemIdx n = instance.item_number();
	
	while (b) {
		b = false;
		for (ItemIdx j=1; j<=n; ++j) {
			if (!solution.get(j) &&
					solution.remaining_capacity() >= instance.weight(j)) {
				solution.set(j, true);
				b = true;
				break;
			}
		}
		for (ItemIdx i=1; i<=n; ++i) {
			Profit pi = instance.profit(i);
			Weight wi = instance.weight(i);
			if (!solution.get(i))
				continue;
			for (ItemIdx j=1; j<=n; ++j) {
				if (solution.get(j))
					continue;
				Profit pj = instance.profit(j);
				Weight wj = instance.weight(j);
				if ((solution.remaining_capacity() + wi >= wj && pj > pi)
						|| (pj >= pi && wj < wi)) {
					solution.set(i, false);
					solution.set(j, true);
					b = true;
					break;
				}
			}
			if (b)
				break;
		}
	}
	return solution.profit();
}

bool upper_bound(
		const Instance& instance,
		ItemIdx j, Weight capacity, Profit lower_bound)
{
	ItemIdx i;
	Profit p = 0;
	Weight w = capacity;
	for (i=j; i>0; i--) {
		Weight wi = instance.weight(i);
		if (wi > w)
			break;
		p += instance.profit(i);
		w -= wi;
	}
	ItemIdx b = i; // break item
	Weight  r = w; // residual capacity
	Profit pb = instance.profit(b);
	Weight wb = instance.weight(b);
	// p + pb/wb*r >= lb
	return (p * wb + pb * r >= wb * lower_bound);
}

bool upper_bound_without(
		const Instance& instance,
		ItemIdx j, Weight capacity, Profit lower_bound)
{
	ItemIdx i;
	Profit  p = 0;
	Weight  w = capacity;
	for (i=instance.item_number(); i>0; i--) {
		Weight wi = instance.weight(i);
		if (i != j) {
			if (wi > w)
				break;
			w -= wi;
			p += instance.profit(i);
		}
	}
	ItemIdx b = i;
	Weight  r = w;
	Profit pb = instance.profit(b);
	Weight wb = instance.weight(b);
	// p + pb/wb*r >= lb
	return (p * wb + pb * r >= wb * lower_bound);
}

