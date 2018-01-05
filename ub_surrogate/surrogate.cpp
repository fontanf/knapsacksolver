#include "surrogate.hpp"

#include "../ub_dantzig/dantzig.hpp"

#define DBG(x)
//#define DBG(x) x

class SurrogateInfos
{

public:

	SurrogateInfos(boost::property_tree::ptree* pt, bool verbose, const Instance& instance):
		pt_(pt), verbose_(verbose), instance_(instance) {  }

	~SurrogateInfos() {  }

	Profit write(Profit ub, std::string str, Weight s)
	{
		if (pt_ != NULL) {
			pt_->put("UB" + std::to_string(k_) + ".Value", ub);
			pt_->put("UB" + std::to_string(k_) + ".Type", str);
			pt_->put("UB" + std::to_string(k_) + ".Multiplier", s);
		}
		if (verbose_)
			std::cout
				<< "UB " << ub
				<< " GAP " << ub - instance_.optimum()
				<< " (" << str << ", " << s << ")"
				<< std::endl;
		k_++;
		return ub;
	}

private:

	boost::property_tree::ptree* pt_;
	bool verbose_;
	const Instance& instance_;
	size_t k_ = 1;

};

Profit ub_surrogate_cardinality_max(const Instance& instance, ItemIdx k,
		SurrogateInfos& infos)
{
	DBG(std::cout << "ub_cardinality_max " << k << std::endl;)

	Instance instance_tmp = Instance::child(instance);
	ItemIdx i = 0;
	Weight  r = 0;
	Profit  p = 0;
	Profit ub = ub_dantzig(instance);
	infos.write(ub, "W", -1);
	ItemIdx gamma = 0;
	Profit p_max = instance.profit(instance.max_profit_item());
	Weight w_max = instance.weight(instance.max_weight_item());
	Weight  s = 0;
	Weight s1 = 0;
	Weight s2 = p_max * w_max - 1;

	while (s1 <= s2) {
		s = (s1 + s2) / 2;
		DBG(std::cout << "s1: " << s1 << "; s: " << s << "; s2: " << s2 << " - ";)

		instance_tmp.surrogate_plus(instance, s, k);
		p = 0;
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
		DBG(std::cout << " " << gamma << " " << p << std::flush;)

		if (p < ub) {
			ub = p;
			infos.write(ub, "W", s);
		}
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

Profit ub_surrogate_cardinality_min(const Instance& instance, ItemIdx k,
		SurrogateInfos& infos)
{
	DBG(std::cout << "ub_cardinality_min" << std::endl;)

	Instance instance_tmp = Instance::child(instance);
	ItemIdx i = 0;
	Weight  r = 0;
	Profit  p = 0;
	Profit ub = ub_dantzig(instance);
	infos.write(ub, "Z", -1);
	ItemIdx gamma = 0;
	Profit p_max = instance.profit(instance.max_profit_item());
	Weight w_max = instance.weight(instance.max_weight_item());
	Weight  s = 0;
	Weight s1 = 0;
	Weight s2 = p_max * w_max - 1;

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

		if (p < ub) {
			ub = p;
			infos.write(ub, "Z", s);
		}
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

Profit ub_surrogate(const Instance& instance, Profit lower_bound,
		boost::property_tree::ptree* pt, bool verbose)
{
	DBG(std::cout << "ub_surrogate()..." << std::endl;)

	if (instance.item_number() == 0)
		return 0;

	SurrogateInfos infos(pt, verbose, instance);

	ItemIdx b = 1;
	Weight  r = instance.capacity();
	Profit  p = 0;
	for (b=1; b<=instance.item_number(); ++b) {
		Weight wi = instance.weight(b);
		if (r < wi)
			break;
		r -= wi;
		p += instance.profit(b);
	}
	DBG(std::cout << "b: " << b << std::endl;)

	if (r == 0 || b == instance.item_number() + 1) {
		DBG(std::cout << "UB: " << p << " (" << instance.optimum() << ")" << std::endl;)
		assert(instance.optimum() == 0 || p >= instance.optimum());
		DBG(std::cout << "ub_surrogate()... end" << std::endl;)
		return infos.write(p, "D", -1);
	}

	Profit ub_kw = -1;
	Profit ub_kz = -1;

	// Compute kw
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
	if (kw == 0)
		return 0;
	if (kw == b - 1) {
		ub_kw = ub_surrogate_cardinality_max(instance, kw, infos);
		DBG(std::cout << "UB: " << ub_kw << " (" << instance.optimum() << ")" << std::endl;)
		assert(instance.optimum() == 0 || ub_kw >= instance.optimum());
	}

	// Compute kz
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
	if (kz == b) {
		ub_kz = ub_surrogate_cardinality_min(instance, kz, infos);
		DBG(std::cout << "UB: " << ub_kz << " (" << instance.optimum() << ")" << std::endl;)
		assert(instance.optimum() == 0 || ub_kz >= instance.optimum());
	}

	if (ub_kw != -1 && ub_kz != -1) {
		return (ub_kw > ub_kz)? ub_kz: ub_kw;
	} else if (ub_kw != -1) {
		return ub_kw;
	} else if (ub_kz != -1) {
		return ub_kz;
	} else {
		ub_kw = ub_surrogate_cardinality_max(instance, b-1, infos);
		DBG(std::cout << "ub_kw: " << ub_kw << std::endl;)
		ub_kz = ub_surrogate_cardinality_min(instance, b, infos);
		DBG(std::cout << "ub_kz: " << ub_kz << " (" << instance.optimum() << ")" << std::endl;)
		Profit ub = (ub_kw > ub_kz)? ub_kw: ub_kz;
		assert(instance.optimum() == 0 || ub >= instance.optimum());
		infos.write(ub, "WZ", -1);
		return ub;
	}
}

#undef DBG
