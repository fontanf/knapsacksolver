#include "it.hpp"

#include <algorithm>
#include <iostream>

Solution sopt_bellman_it(const Instance& instance)
{
	// Initialize memory table
	// values[i][w] == values[w * (n+1) + i]
	// values[i][w] == values[i * (c+1) + w]
	ItemIdx n = instance.item_number();
	ItemIdx c = instance.capacity();
	ValIdx values_size = (n+1)*(c+1);
	Profit* values = new Profit[values_size];

	// Compute optimal value
	for (Weight w=0; w<=instance.capacity(); ++w) {
		//ValIdx x = w*(n+1);
		ValIdx x = w;
		values[x] = 0;
	}
	for (ItemIdx i=1; i<=instance.item_number(); ++i) {
		Weight wi = instance.weight(i);
		for (Weight w=0; w<=instance.capacity(); ++w) {
			//ValIdx x  = w*(n+1) + i;
			//ValIdx x0 = w*(n+1) + (i-1);
			//ValIdx x1 = (w-wi)*(n+1) + (i-1);
			ValIdx x  = i*(c+1) + w;
			ValIdx x0 = (i-1)*(c+1) + w;
			ValIdx x1 = (i-1)*(c+1) + (w-wi);
			Profit v0 = values[x0];
			Profit v1 = (w < wi)? 0: values[x1] + instance.profit(i);
			values[x] = (v1 > v0)? v1: v0;
		}
	}
	Profit opt = values[values_size-1];

	// Retrieve optimal solution
	Solution solution(instance);
	ItemIdx i = n;
	Weight  w = c;
	Profit  v = 0;
	while (v < opt) {
		Weight wi = instance.weight(i);
		Profit pi = instance.profit(i);
		//ValIdx x0 = w * (n+1) + (i-1);
		//ValIdx x1 = (w - wi) * (n+1) + (i-1);
		ValIdx x0 = (i-1) * (c+1) + w;
		ValIdx x1 = (i-1) * (c+1) + (w - wi);

		Profit v0 = values[x0];
		Profit v1 = (w < wi)? 0: values[x1] + pi;
		if (v1 > v0) {
			v += pi;
			w -= wi;
			solution.set(i, true);
		}
		i--;
	}

	delete[] values; // Free memory

	return solution;
}
