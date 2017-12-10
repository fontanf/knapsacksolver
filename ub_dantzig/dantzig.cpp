#include "dantzig.hpp"

#define DBG(x)
//#define DBG(x) x

Profit ub_dantzig(const Instance& instance)
{
	DBG(std::cout << "upper_bound_danzig()..." << std::endl;)
	ItemIdx i;
	Profit p = 0;
	Weight remaining_capacity = instance.capacity();
	for (i=instance.item_number(); i>0; i--) {
		DBG(std::cout << i << " " << std::flush;)
		Weight wi = instance.weight(i);
		if (wi > remaining_capacity)
			break;
		p += instance.profit(i);
		remaining_capacity -= wi;
	}
	DBG(std::cout << std::endl;)
	DBG(std::cout << "i: " << i << " r: " << remaining_capacity << " " << std::flush;)
	if (i != 0 && remaining_capacity > 0)
		p += (instance.profit(i) * remaining_capacity) / instance.weight(i);
	DBG(std::cout << "UB: " << p << " " << std::endl;)
	assert(instance.optimum() == 0 || p >= instance.optimum());
	DBG(std::cout << "upper_bound_danzig()... END" << std::endl;)
	return p;
}

#undef DBG
