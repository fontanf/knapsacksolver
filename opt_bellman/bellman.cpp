#include "bellman.hpp"

#include "it.hpp"

Profit opt_bellman(const Instance& instance)
{
	Weight c = instance.capacity();
	Profit* values = new Profit[c+1];
	for (Weight w=c+1; w-->0;)
		values[w] = 0;
	for (ItemIdx i=1; i<=instance.item_number(); ++i) {
		Weight wi = instance.weight(i);
		Profit pi = instance.profit(i);
		for (Weight w=c+1; w-->0;)
			if (w >= wi && values[w-wi] + pi > values[w])
				values[w] = values[w-wi] + pi;
	}
	Profit opt = values[c];
	delete[] values; // Free memory
	return opt;
}

Solution sopt_bellman_1(const Instance& instance)
{
	return sopt_bellman_it(instance);
}

Solution sopt_bellman_2(const Instance& instance)
{
	ItemIdx n = instance.item_number();
	ItemIdx c = instance.capacity();

	Solution solution(instance);

	Profit* values = new Profit[c+1]; // Initialize memory table
	for (;;) {
		std::cout << "n " << n << " " << std::flush;
		std::cout << "c " << c << " " << std::flush;
		ItemIdx last_item = 0;
		for (Weight w=c+1; w-->0;)
			values[w] = 0;
		for (ItemIdx i=1; i<=n; ++i) {
			Weight wi = instance.weight(i);
			Profit pi = instance.profit(i);
			if (c >= wi && values[c-wi] + pi > values[c]) {
				values[c] = values[c-wi] + pi;
				last_item = i;
			}
			for (Weight w=c; w-->0;)
				if (w >= wi && values[w-wi] + pi > values[w])
					values[w] = values[w-wi] + pi;
		}

		std::cout << "last item " << last_item << std::endl;

		if (last_item == 0)
			break;

		solution.set(last_item, true);
		c -= instance.weight(last_item);
		n = last_item - 1;
	}

	delete[] values; // Free memory

	return solution;
}
