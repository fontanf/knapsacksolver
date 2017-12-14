#include "dantzig.hpp"

#define DBG(x)
//#define DBG(x) x

Profit ub_dantzig(const Instance& instance)
{
	DBG(std::cout << "upper_bound_danzig()..." << std::endl;)
	ItemIdx i;
	Profit p = 0;
	Weight remaining_capacity = instance.capacity();
	for (i=1; i<=instance.item_number(); i++) {
		DBG(std::cout << i << " " << std::flush;)
		Weight wi = instance.weight(i);
		if (wi > remaining_capacity)
			break;
		p += instance.profit(i);
		remaining_capacity -= wi;
	}
	DBG(std::cout << std::endl;)
	DBG(std::cout << "i: " << i << " r: " << remaining_capacity << " " << std::flush;)
	if (i != instance.item_number() + 1 && remaining_capacity > 0)
		p += (instance.profit(i) * remaining_capacity) / instance.weight(i);
	DBG(std::cout << "UB: " << p << " " << std::endl;)
	assert(instance.optimum() == 0 || p >= instance.optimum());
	DBG(std::cout << "upper_bound_danzig()... END" << std::endl;)
	return p;
}

Profit ub_dantzig_without(const Instance& instance, ItemIdx j, Weight capacity)
{
	ItemIdx i = 1;
	Profit p = 0;
	Weight remaining_capacity = capacity;
	for (i=1; i<=instance.item_number(); i++) {
		if (i == j)
			continue;
		Weight wi = instance.weight(i);
		if (wi > remaining_capacity)
			break;
		p += instance.profit(i);
		remaining_capacity -= wi;
	}
	if (i != instance.item_number() + 1 && remaining_capacity > 0)
		p += (instance.profit(i) * remaining_capacity) / instance.weight(i);
	return p;
}

Profit ub_dantzig_from(const Instance& instance, ItemIdx j, Weight capacity)
{
	ItemIdx i = 1;
	Profit p = 0;
	Weight remaining_capacity = capacity;
	for (i=j; i<=instance.item_number(); i++) {
		Weight wi = instance.weight(i);
		if (wi > remaining_capacity)
			break;
		p += instance.profit(i);
		remaining_capacity -= wi;
	}
	if (i != instance.item_number() + 1 && remaining_capacity > 0)
		p += (instance.profit(i) * remaining_capacity) / instance.weight(i);
	return p;
}

#undef DBG
