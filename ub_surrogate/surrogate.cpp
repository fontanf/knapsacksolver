#include "surrogate.hpp"

#include "../ub_dantzig/dantzig.hpp"

#define DBG(x)
//#define DBG(x) x

inline void swap(ItemIdx* v, ItemIdx i, ItemIdx j)
{
	ItemIdx tmp = v[j];
	v[j] = v[i];
	v[i] = tmp;
}

ItemIdx max_card(const Instance& instance)
{
	ItemIdx* index = new ItemIdx[instance.item_number()];
	std::iota(index, index+instance.item_number(), 1);

	ItemIdx kp1 = 0;
	ItemIdx f = 0;
	ItemIdx l = instance.item_number() - 1;
	Weight  w = 0;
	Weight  c = instance.capacity();
	while (f < l) {
		ItemIdx pivot = f + 1 + rand() % (l - f); // Select pivot

		//for (ItemIdx i=0; i<instance.item_number(); ++i)
			//std::cout << index[i] << " (" << instance.weight(index[i]) << ") ";
		//std::cout << std::endl;
		//std::cout << "f " << f << " l " << l << " pivot " << pivot << std::endl;

		swap(index, pivot, l);
		ItemIdx j = f;
		for (ItemIdx i=f; i<l; ++i) {
			if (instance.weight(index[i]) > instance.weight(index[l]))
				continue;
			swap(index, i, j);
			j++;
		}
		swap(index, j, l);

		Weight w_curr = w;
		for (ItemIdx i=f; i<j; ++i)
			w_curr += instance.weight(index[i]);
		//std::cout << "w " << w_curr << " c " << c << " w+pj " << w_curr + instance.weight(index[j]) << std::endl;

		if (w_curr + instance.weight(index[j]) <= c) {
			f = j;
			w = w_curr;
		} else if (w_curr > c) {
			l = j-1;
		} else {
			kp1 = j;
			break;
		}
	}
	if (kp1 == 0)
		kp1 = f;
	ItemIdx k = kp1 - 1;

	//for (ItemIdx i=0; i<instance.item_number(); ++i)
		//std::cout << index[i] << " (" << instance.weight(index[i]) << ") ";
	//std::cout << std::endl;

	DBG(std::cout << "kmax " << k+1 << std::endl;
	Weight ww = 0;
	for (ItemIdx i=0; i<=k; ++i)
		ww += instance.weight(index[i]);
	std::cout << "ww " << ww << " <= c " << c << " < ww+wk+1 " << ww + instance.weight(index[k+1]) << std::endl;
	assert(ww <= c && ww + instance.weight(index[k+1]) > c);)

	delete[] index;
	return k+1;
}

ItemIdx min_card(const Instance& instance, Profit lb)
{
	ItemIdx* index = new ItemIdx[instance.item_number()];
	std::iota(index, index+instance.item_number(), 1);

	ItemIdx f = 0;
	ItemIdx l = instance.item_number() - 1;
	ItemIdx km1 = 0;
	Profit p = 0;
	while (f < l) {
		ItemIdx pivot = f + 1 + rand() % (l - f); // Select pivot

		//for (ItemIdx i=0; i<instance.item_number(); ++i)
			//std::cout << index[i] << " (" << instance.profit(index[i]) << ") ";
		//std::cout << std::endl;
		//std::cout << "f " << f << " l " << l << " pivot " << pivot << std::endl;

		swap(index, pivot, l);
		ItemIdx j = f;
		for (ItemIdx i=f; i<l; ++i) {
			if (instance.profit(index[i]) < instance.profit(index[l]))
				continue;
			swap(index, i, j);
			j++;
		}
		swap(index, j, l);

		Profit p_curr = p;
		for (ItemIdx i=f; i<j; ++i)
			p_curr += instance.profit(index[i]);
		//std::cout << "p " << p_curr << " lb " << lb << " p+pj " << p_curr + instance.profit(index[j]) << std::endl;

		if (p_curr > lb) {
			l = j-1;
		} else if (p_curr + instance.profit(index[j]) <= lb) {
			f = j;
			p = p_curr;
		} else {
			km1 = j-1;
			break;
		}
	}
	if (km1 == 0) {
		std::cout << "f" << std::endl;
		km1 = f;
	}
	ItemIdx k = km1 + 1;

	//for (ItemIdx i=0; i<instance.item_number(); ++i)
		//std::cout << index[i] << " (" << instance.profit(index[i]) << ") ";
	//std::cout << std::endl;

	DBG(std::cout << "kmin " << k+1 << std::endl;
	Weight pp = 0;
	for (ItemIdx i=0; i<k; ++i)
		pp += instance.profit(index[i]);
	std::cout << "pp " << pp << " <= z " << lb << " < pp+pk " << pp + instance.profit(index[k]) << std::endl;
	assert(pp <= lb && pp + instance.profit(index[k]) > lb);)

	delete[] index;
	return k+1;
}

void ub_surrogate_solve(const Instance& instance, ItemIdx k,
		Weight s_min, Weight s_max, SurrogateOut& out)
{
	Instance is = Instance::child(instance);
	ItemIdx gamma = 0;
	Weight  s = 0;
	Weight s1 = s_min;
	Weight s2 = s_max;

	while (s1 <= s2) {
		s = (s1 + s2) / 2;

		is.surrogate(instance, s, k);
		Profit p = is.solution()->profit() + is.break_profit();
		if (is.break_capacity() > 0 && is.break_item() != is.item_number() + 1)
			p += (is.profit(is.break_item()) * is.break_capacity()) / is.weight(is.break_item());
		gamma = is.break_item() - 1 + is.solution()->item_number();

		DBG(std::cout
				<< "s1 " << s1 << " s " << s << " s2 " << s2
				<< " g " << gamma
				<< " ub " << p
				<< " GAP " << p - instance.optimum()
				<< std::endl;)

		if (p < out.ub) {
			out.ub         = p;
			out.multiplier = s;
		}

		if (gamma == k && is.break_capacity() == 0)
			return;

		if ((s_min == 0 && gamma >= k) || (s_max == 0 && gamma >= k)) {
			s1 = s + 1;
		} else {
			s2 = s - 1;
		}
	}
}

SurrogateOut ub_surrogate(const Instance& instance, Profit lb,
		boost::property_tree::ptree* pt, bool verbose)
{
	assert(instance.sort_type() == "efficiency" ||
			instance.sort_type() == "partial_efficiency");

	SurrogateOut out(pt, verbose);

	// Trivial cases
	if (instance.item_number() == 0)
		return out;
	out.ub = ub_dantzig(instance);
	if (instance.break_capacity() == 0 || instance.break_item() == instance.item_number() + 1)
		return out;
	ItemIdx i_wmax = instance.max_weight_item();
	//ItemIdx i_pmax = instance.max_profit_item();
	if (i_wmax == 0)
		return out;

	// Should ideally be: maxp*maxp, but may cause overflow (sic)
	//Weight s_max = instance.weight(i_wmax) * instance.profit(i_pmax);
	Weight s_max = instance.weight(i_wmax);
	// Should ideally be: -maxp*maxw, but may cause overflow (sic)
	Weight s_min = -s_max;

	std::cout
		<< "z " << lb
		<< " GAP " << instance.optimum() - lb
		<< " b " << instance.break_item()
		<< " s_max " << s_max
		<< std::endl;

	if (max_card(instance) == instance.break_item() - 1) {
		ub_surrogate_solve(instance, instance.break_item() - 1, 0, s_max, out);
	} else if (min_card(instance, lb) == instance.break_item()) {
		ub_surrogate_solve(instance, instance.break_item(), s_min, 0, out);
		if (out.ub < lb)
			out.ub = lb;
	} else {
		SurrogateOut out2(pt, verbose);
		out2.ub = out.ub;
		ub_surrogate_solve(instance, instance.break_item()-1, 0, s_max, out);
		ub_surrogate_solve(instance, instance.break_item(), s_min, 0, out2);
		if (out2.ub > out.ub) {
			out.ub         = out2.ub;
			out.multiplier = out2.multiplier;
		}
	}

	if (verbose) {
		std::cout
			<< "UB " << out.ub
			<< " GAP " << out.ub - instance.optimum()
			<< std::endl;
	}
	if (pt != NULL) {
		pt->put("UB.Value", out.ub);
	}
	assert(instance.optimum() == 0 || out.ub >= instance.optimum());
	return out;
}

#undef DBG
