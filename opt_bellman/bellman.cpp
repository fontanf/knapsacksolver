#include "bellman.hpp"

#include "it.hpp"

Solution sopt_bellman(const Instance& instance)
{
	return sopt_bellman_it(instance);
}

Profit opt_bellman(const Instance& instance)
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
