#include "dantzig.hpp"

#define DBG(x)
//#define DBG(x) x

Profit ub_dantzig(const Instance& instance,
		boost::property_tree::ptree* pt, bool verbose)
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

	if (pt != NULL) {
		pt->put("UB.Value", p);
	}
	if (verbose)
		std::cout
			<< "UB " << p
			<< " GAP " << p - instance.optimum()
			<< std::endl;
	return p;
}

#undef DBG

Profit ub_dantzig_without(const Instance& instance, ItemIdx j, Weight c)
{
	ItemIdx i = 1;
	Profit p = 0;
	Weight remaining_capacity = c;
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

Profit ub_dantzig_from(const Instance& instance, ItemIdx j, Weight c)
{
	ItemIdx i = 1;
	Profit p = 0;
	Weight remaining_capacity = c;
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

Profit ub_dantzig_from_to(const Instance& instance, ItemIdx i1, ItemIdx i2, Weight c)
{
	ItemIdx i = i1;
	Profit  p = 0;
	Weight  r = c;
	for (; i<=i2; i++) {
		Weight wi = instance.weight(i);
		if (wi > r)
			break;
		p += instance.profit(i);
		r -= wi;
	}
	if (i != i2 + 1 && r > 0)
		p += (instance.profit(i) * r) / instance.weight(i);
	return p;
}

#define DBG(x)
//#define DBG(x) x

Profit ub_dantzig_except(const Instance& instance,
		ItemIdx n1, ItemIdx i1, ItemIdx i2, ItemIdx n2, Weight c)
{
	DBG(std::cout << "ub_dantzig_except " << n1 << " " << i1 << " " << i2 << " " << n2 << " " << c << std::endl;)
	ItemIdx i = n1;
	if (i == i1)
		i = i2+1;
	Profit  p = 0;
	Weight remaining_capacity = c;
	for (; i<=n2; i++) {
		DBG(std::cout << "i " << i << std::endl;)
		Weight wi = instance.weight(i);
		if (wi > remaining_capacity)
			break;
		p += instance.profit(i);
		remaining_capacity -= wi;
		if (i == i1-1)
			i = i2;
	}
	if (i != instance.item_number() + 1 && remaining_capacity > 0)
		p += (instance.profit(i) * remaining_capacity) / instance.weight(i);
	DBG(std::cout << "p " << p << std::endl;)
	return p;
}

#undef DBG
