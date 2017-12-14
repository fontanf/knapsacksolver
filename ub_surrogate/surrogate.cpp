#include "surrogate.hpp"

#include "../ub_dantzig/dantzig.hpp"

#define DBG(x)
//#define DBG(x) x

Profit ub_surrogate_cardinality_max(const Instance& instance, ItemIdx k)
{
	DBG(std::cout << "ub_cardinality_max" << std::endl;)

	Instance instance_tmp = Instance::child(instance);
	ItemIdx i = 0;
	Weight  r = 0;
	Profit  p = 0;
	Profit ub = ub_dantzig(instance);
	ItemIdx gamma = 0;
	ProWei  s = 0;
	ProWei s1 = 0;
	ProWei s2 = instance.profit_max() * instance.weight_max() - 1;

	while (s1 <= s2) {
		s = (s1 + s2) / 2;
		DBG(std::cout << "s1: " << s1 << "; s: " << s << "; s2: " << s2 << " - ";)

		p = 0;
		instance_tmp.surrogate_plus(instance, s, k);
		r = instance_tmp.capacity();
		for (i=1; i<=instance_tmp.item_number(); ++i) {
			Weight wi = instance_tmp.weight(i);
			if (r < wi)
				break;
			r -= wi;
			p += instance_tmp.profit(i);
		}
		if (r > 0 && i != instance_tmp.item_number() + 1)
			p += (instance_tmp.profit(i) * r) / instance_tmp.weight(i);
		gamma = i - 1;
		DBG(std::cout << " " << gamma << std::flush;)

		if (p < ub)
			ub = p;
		if (gamma >= k) {
			DBG(std::cout << " >" << std::endl;)
			s1 = s + 1;
		} else if (gamma < k) {
			DBG(std::cout << " <" << std::endl;)
			s2 = s - 1;
		}
	}

	return ub;
}

//#undef DBG

//#define DBG(x)
//#define DBG(x) x

Profit ub_surrogate_cardinality_min(const Instance& instance, ItemIdx k)
{
	DBG(std::cout << "ub_cardinality_min" << std::endl;)

	Instance instance_tmp = Instance::child(instance);
	ItemIdx i = 0;
	Weight  r = 0;
	Profit  p = 0;
	Profit ub = ub_dantzig(instance);
	ItemIdx gamma = 0;
	ProWei  s = 0;
	ProWei s1 = 0;
	ProWei s2 = instance.profit_max() * instance.weight_max() - 1;

	while (s1 <= s2) {
		s = (s1 + s2) / 2;
		DBG(std::cout << "s1: " << s1 << "; s: " << s << "; s2: " << s2 << " - ";)

		instance_tmp.surrogate_minus(instance, s, k);
		p = instance_tmp.solution()->profit();
		DBG(std::cout << p << " " << std::flush;)
		r = instance_tmp.capacity();
		for (i=1; i<=instance_tmp.item_number(); ++i) {
			Weight wi = instance_tmp.weight(i);
			if (r < wi)
				break;
			r -= wi;
			p += instance_tmp.profit(i);
		}
		DBG(std::cout << p << " " << std::flush;)
		if (r > 0 && i != instance_tmp.item_number() + 1)
			p += (instance_tmp.profit(i) * r) / instance_tmp.weight(i);
		gamma = i - 1 + instance_tmp.solution()->item_number();
		DBG(std::cout << p << " " << gamma << " " << std::flush;)

		if (p < ub)
			ub = p;
		if (gamma < k) {
			DBG(std::cout << ">" << std::endl;)
			s1 = s + 1;
		} else if (gamma >= k) {
			DBG(std::cout << "<" << std::endl;)
			s2 = s - 1;
		}
	}

	DBG(std::cout << "ub_cardinality_min END" << std::endl;)
	return ub;
}

//#undef DBG

//#define DBG(x)
//#define DBG(x) x

Profit ub_surrogate(const Instance& instance, Profit lower_bound)
{
	DBG(std::cout << "ub_martello()..." << std::endl;)

	ItemIdx b = 0;
	Weight r = instance.capacity();
	Profit p = 0;
	for (ItemIdx i=1; i<=instance.item_number(); ++i) {
		Weight wi = instance.weight(i);
		if (r < wi) {
			b = i;
			break;
		}
		r -= wi;
		p += instance.profit(i);
	}
	DBG(std::cout << "b: " << b << std::endl;)

	if (r == 0) {
		DBG(std::cout << "UB: " << p << " (" << instance.optimum() << ")" << std::endl;)
		assert(instance.optimum() == 0 || p >= instance.optimum());
		DBG(std::cout << "ub_martello()... end" << std::endl;)
		return p;
	}

	Instance instance_weight = Instance::sort_by_weight(instance);
	ItemIdx kw = 0;
	r = instance_weight.capacity();
	for (ItemIdx i=1; i<=instance_weight.item_number(); ++i) {
		Weight wi = instance_weight.weight(i);
		if (r < wi) {
			kw = i - 1;
			break;
		}
		r -= wi;
	}
	DBG(std::cout << "kw: " << kw << std::endl;)
	if (kw <= b) {
		Profit ub = ub_surrogate_cardinality_max(instance, kw);
		DBG(std::cout << "UB: " << ub << " (" << instance.optimum() << ")" << std::endl;)
		assert(instance.optimum() == 0 || ub >= instance.optimum());
		DBG(std::cout << "ub_martello()... end" << std::endl;)
		return ub;
	}

	Instance instance_profit = Instance::sort_by_profit(instance);
	ItemIdx kz = 0;
	for (ItemIdx i=1; i<=instance_profit.item_number(); ++i) {
		Profit pi = instance_profit.profit(i);
		if (lower_bound < pi) {
			kz = i;
			break;
		}
		lower_bound -= pi;
	}
	DBG(std::cout << "kz: " << kz << std::endl;)
	if (kz >= b) {
		Profit ub = ub_surrogate_cardinality_min(instance, kz - 1);
		DBG(std::cout << "UB: " << ub << " (" << instance.optimum() << ")" << std::endl;)
		assert(instance.optimum() == 0 || ub >= instance.optimum());
		DBG(std::cout << "ub_martello()... end" << std::endl;)
		return ub;
	}

	Profit ub1 = ub_surrogate_cardinality_max(instance, b);
	DBG(std::cout << "ub1: " << ub1 << std::endl;)
	Profit ub2 = ub_surrogate_cardinality_min(instance, b - 1);
	DBG(std::cout << "ub2: " << ub2 << " (" << instance.optimum() << ")" << std::endl;)
	Profit ub  = (ub1 > ub2)? ub1: ub2;
	assert(instance.optimum() == 0 || ub >= instance.optimum());

	DBG(std::cout << "ub_martello()... end" << std::endl;)
	return ub;
}

#undef DBG
