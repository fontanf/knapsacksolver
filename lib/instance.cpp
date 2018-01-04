#include "instance.hpp"
#include "solution.hpp"

#include "../ub_dantzig/dantzig.hpp"

#include <sstream>

Instance::Instance(ItemIdx n, Weight c, std::vector<Profit> p, std::vector<Weight> w):
	name_(""), format_(""),
	n_(n), c_(c), opt_(0)
{
	w_ = new Weight[n];
	p_ = new Profit[n];
	i_ = new ItemIdx[n];
	for (ItemIdx i=1; i<=n_; ++i) {
		set_weight(i, w[i-1]);
		set_profit(i, p[i-1]);
		set_index(i, i);
	}
}

Instance::Instance(boost::filesystem::path filepath)
{
	if (!boost::filesystem::exists(filepath)) {
		std::cout << filepath << ": file not found." << std::endl;
		assert(false);
	}

	boost::filesystem::path FORMAT = filepath.parent_path() / "FORMAT.txt";
	if (!boost::filesystem::exists(FORMAT)) {
		std::cout << FORMAT << ": file not found." << std::endl;
		assert(false);
	}

	boost::filesystem::fstream file(FORMAT, std::ios_base::in);
	std::getline(file, format_);
	if        (format_ == "knapsack_standard") {
		read_standard(filepath);
	} else if (format_ == "knapsack_pisinger") {
		read_pisinger(filepath);
	} else {
		std::cout << format_ << ": Unknown instance format." << std::endl;
		assert(false);
	}

	i_ = new ItemIdx[n_];
	for (ItemIdx i=1; i<=n_; ++i)
		set_index(i, i);
}

void Instance::read_standard(boost::filesystem::path filepath)
{
	name_ = filepath.stem().string();
	boost::filesystem::fstream file(filepath, std::ios_base::in);
	file >> n_;
	file >> c_;
	w_ = new Weight[n_];
	p_ = new Profit[n_];
	for (ItemIdx i=0; i<n_; ++i) {
		file >> p_[i];
		file >> w_[i];
	}

	if (!file.eof())
		file >> opt_;

	file.close();
}

void Instance::read_pisinger(boost::filesystem::path filepath)
{
	boost::filesystem::fstream file(filepath, std::ios_base::in);
	uint_fast64_t null;

	std::getline(file, name_);

	std::string line;
	std::istringstream iss;

	std::getline(file, line, ' ');
	std::getline(file, line);
	std::istringstream(line) >> n_;

	std::getline(file, line, ' ');
	std::getline(file, line);
	std::istringstream(line) >> c_;

	std::getline(file, line, ' ');
	std::getline(file, line);
	std::istringstream(line) >> null;

	std::getline(file, line);

	w_ = new Weight[n_];
	p_ = new Profit[n_];
	x_ = new bool[n_];
	opt_ = 0;

	for (ItemIdx i=0; i<n_; ++i) {
		ItemIdx idx;
		std::getline(file, line, ',');
		std::istringstream(line) >> idx;
		assert(idx == i+1);

		std::getline(file, line, ',');
		std::istringstream(line) >> p_[i];

		std::getline(file, line, ',');
		std::istringstream(line) >> w_[i];

		std::getline(file, line);
		std::istringstream(line) >> x_[i];

		if (x_[i])
			opt_ += p_[i];
	}

	file.close();
}

Instance::~Instance()
{
	delete[] w_;
	delete[] p_;
	delete[] i_;
	if (solution_ != NULL)
		delete solution_;
	if (x_ != NULL)
		delete[] x_;
}

Profit Instance::check(boost::filesystem::path cert_file)
{
	if (!boost::filesystem::exists(cert_file))
		return -1;
	boost::filesystem::ifstream file(cert_file, std::ios_base::in);
	bool x;
	Profit p = 0;
	Weight c = 0;
	for (ItemIdx i=1; i<=item_number(); ++i) {
		file >> x;
		if (x) {
			p += profit(i);
			c += weight(i);
		}
	}
	if (c > capacity())
		return -1;
	return p;
}

ItemIdx Instance::index_orig(ItemIdx i) const
{
	if (parent() == NULL)
		return i;
	return parent()->index_orig(index(i));
}

Profit Instance::profit_orig() const
{
	if (parent() == NULL)
		return 0;
	return solution()->profit() + parent()->profit_orig();
}

const Instance* Instance::instance_orig() const
{
	if (parent() == NULL)
		return this;
	return parent()->instance_orig();
}

Solution Instance::solution_orig(const Solution& solution) const
{
	assert(solution.instance() == this);
	if (parent() == NULL)
		return solution;
	Solution solution_parent(*solution_);
	for (ItemIdx i=1; i<=item_number(); ++i)
		solution_parent.set(index(i), solution.get(i));
	return parent()->solution_orig(solution_parent);
}

void Instance::init(const Instance& instance)
{
	name_     = instance.name_;
	format_   = instance.format_;
	parent_   = &instance;
	solution_ = new Solution(instance);
}

Instance::Instance(const Instance& instance):
	name_(instance.name() + "_copy"), format_(instance.format()),
	n_(instance.item_number()), c_(instance.capacity()), opt_(instance.optimum()),
	parent_(instance.parent()), solution_(new Solution(*instance.solution()))
{
	w_ = new Weight[instance_orig()->item_number()];
	p_ = new Profit[instance_orig()->item_number()];
	i_ = new ItemIdx[instance_orig()->item_number()];
	for (ItemIdx i=1; i<=n_; ++i) {
		set_weight(i, instance.weight(i));
		set_profit(i, instance.profit(i));
		set_index(i, instance.index(i));
	}
}

Instance Instance::child(const Instance& instance)
{
	Instance instance_new;
	instance_new.init(instance);
	instance_new.name_ += "_child";
	instance_new.n_   = instance.item_number();
	instance_new.c_   = instance.capacity();
	instance_new.opt_ = instance.optimum();
	instance_new.i_   = new ItemIdx[instance_new.n_];
	for (ItemIdx i=1; i<=instance_new.n_; ++i)
		instance_new.set_index(i, i);
	instance_new.set_profits_and_weights_from_parent();
	return instance_new;
}

void Instance::set_profits_and_weights_from_parent()
{
	w_ = new Weight[instance_orig()->item_number()];
	p_ = new Profit[instance_orig()->item_number()];
	for (ItemIdx i=1; i<=n_; ++i) {
		set_weight(i, weight_parent(i));
		set_profit(i, profit_parent(i));
	}
}

#define DBG(x)
//#define DBG(x) x

inline void swap(ItemIdx* v, ItemIdx i, ItemIdx j)
{
	ItemIdx tmp = v[j];
	v[j] = v[i];
	v[i] = tmp;
}

template<typename Func1, typename Func2, typename Q>
void partial_sort(ItemIdx* v, ItemIdx n, Func1 compare, Q capacity, Func2 weight)
{
	ItemIdx f = 0;
	ItemIdx l = (n > 0)? n - 1: 0;
	Q w = 0; // Sum of the weights of the items after l
	DBG(std::cout << "f " << f << " l " << l << std::flush;)
	while (f < l) {
		ItemIdx pivot = f + 1 + rand() % (l - f); // Select pivot
		DBG(std::cout << " pivot " << pivot << std::flush;)
		swap(v, pivot, l);

		ItemIdx j = f;
		for (ItemIdx i=f; i<l; ++i) {
			if (!compare(v[i], v[l]))
				continue;
			swap(v, i, j);
			j++;
		}

		Q w_curr = w;
		for (ItemIdx i=f; i<j; ++i)
			w_curr += weight(v[i]);

		swap(v, j, l);
		DBG(std::cout << " j " << j << std::flush;)
		DBG(std::cout << " w_curr " << w_curr << std::flush;)

		if (w_curr + weight(v[j]) <= capacity) {
			f = j+1;
			w = w_curr + weight(v[j]);
			DBG(std::cout << " w " << w << std::flush;)
		} else if (w_curr > capacity) {
			l = j-1;
		} else {
			break;
		}

		DBG(std::cout << std::endl;
		for (ItemIdx i=0; i<v.size(); ++i)
			std::cout << v[i] << " " << std::flush;)
		DBG(std::cout << std::endl;)
		DBG(std::cout << "f " << f << " l " << l << std::flush;)
	}
}

#undef DBG

Instance Instance::sort_by_profit(const Instance& instance)
{
	Instance instance_new;
	instance_new.init(instance);
	instance_new.name_ += "_profit";
	instance_new.n_   = instance.item_number();
	instance_new.c_   = instance.capacity();
	instance_new.opt_ = instance.optimum();
	instance_new.i_   = new ItemIdx[instance_new.n_];
	for (ItemIdx i=1; i<=instance_new.n_; ++i)
		instance_new.set_index(i, i);
	std::sort(instance_new.i_, instance_new.i_ + instance_new.n_,
			[&instance](ItemIdx i1, ItemIdx i2) {
			return instance.profit(i1) > instance.profit(i2);});
	instance_new.set_profits_and_weights_from_parent();
	return instance_new;
}

Instance Instance::sort_by_weight(const Instance& instance)
{
	Instance instance_new;
	instance_new.init(instance);
	instance_new.name_ += "_weight";
	instance_new.n_   = instance.item_number();
	instance_new.c_   = instance.capacity();
	instance_new.opt_ = instance.optimum();
	instance_new.i_   = new ItemIdx[instance_new.n_];
	for (ItemIdx i=1; i<=instance_new.n_; ++i)
		instance_new.set_index(i, i);
	std::sort(instance_new.i_, instance_new.i_ + instance_new.n_,
			[&instance](ItemIdx i1, ItemIdx i2) {
			return instance.weight(i1) < instance.weight(i2);});
	instance_new.set_profits_and_weights_from_parent();
	return instance_new;
}

Instance Instance::sort_by_efficiency(const Instance& instance)
{
	Instance instance_new;
	instance_new.init(instance);
	instance_new.name_ += "_efficiency";
	instance_new.n_   = instance.item_number();
	instance_new.c_   = instance.capacity();
	instance_new.opt_ = instance.optimum();
	instance_new.i_   = new ItemIdx[instance_new.n_];
	for (ItemIdx i=1; i<=instance_new.n_; ++i)
		instance_new.set_index(i, i);
	std::sort(instance_new.i_, instance_new.i_ + instance_new.n_,
			[&instance](ItemIdx i1, ItemIdx i2) {
			return instance.profit(i1) * instance.weight(i2)
			     > instance.profit(i2) * instance.weight(i1);});
	instance_new.set_profits_and_weights_from_parent();
	return instance_new;
}

Instance Instance::sort_partially_by_profit(const Instance& instance, Profit lb)
{
	Instance instance_new;
	instance_new.init(instance);
	instance_new.name_ += "_partprofit";
	instance_new.n_   = instance.item_number();
	instance_new.c_   = instance.capacity();
	instance_new.opt_ = instance.optimum();
	instance_new.i_   = new ItemIdx[instance_new.n_];
	for (ItemIdx i=1; i<=instance_new.n_; ++i)
		instance_new.set_index(i, i);

	partial_sort(
			instance_new.i_, instance_new.n_,
			[&instance](ItemIdx i1, ItemIdx i2) {
			return instance.weight(i1) > instance.weight(i2);},
			lb,
			[&instance](ItemIdx i) { return instance.profit(i); });

	instance_new.set_profits_and_weights_from_parent();
	return instance_new;
}

Instance Instance::sort_partially_by_weight(const Instance& instance)
{
	Instance instance_new;
	instance_new.init(instance);
	instance_new.name_ += "_partweight";
	instance_new.n_   = instance.item_number();
	instance_new.c_   = instance.capacity();
	instance_new.opt_ = instance.optimum();
	instance_new.i_   = new ItemIdx[instance_new.n_];
	for (ItemIdx i=1; i<=instance_new.n_; ++i)
		instance_new.set_index(i, i);

	partial_sort(
			instance_new.i_, instance_new.n_,
			[&instance](ItemIdx i1, ItemIdx i2) {
			return instance.weight(i1) < instance.weight(i2);},
			instance_new.capacity(),
			[&instance](ItemIdx i) { return instance.weight(i); });

	instance_new.set_profits_and_weights_from_parent();
	return instance_new;
}

Instance Instance::sort_partially_by_efficiency(const Instance& instance)
{
	Instance instance_new;
	instance_new.init(instance);
	instance_new.name_ += "_partefficiency";
	instance_new.n_   = instance.item_number();
	instance_new.c_   = instance.capacity();
	instance_new.opt_ = instance.optimum();
	instance_new.i_   = new ItemIdx[instance_new.n_];
	for (ItemIdx i=1; i<=instance_new.n_; ++i)
		instance_new.set_index(i, i);

	partial_sort(
			instance_new.i_, instance_new.n_,
			[&instance](ItemIdx i1, ItemIdx i2) {
			return instance.profit(i1) * instance.weight(i2)
			     > instance.profit(i2) * instance.weight(i1);},
			instance_new.capacity(),
			[&instance](ItemIdx i) { return instance.weight(i); });

	instance_new.set_profits_and_weights_from_parent();
	return instance_new;
}

#define DBG(x)
//#define DBG(x) x

void Instance::surrogate_plus(const Instance& instance, Weight multiplier, ItemIdx bound)
{
	DBG(std::cout << "Instance::surrogate_plus()..." << std::endl;)
	parent_ = &instance;
	opt_ = 0;
	n_ = instance.item_number();
	c_ = instance.capacity() + multiplier * bound;
	partial_sort(
			i_, n_,
			[&instance, &multiplier](ItemIdx i1, ItemIdx i2) {
			Profit pi1 = instance.profit(i1);
			Profit pi2 = instance.profit(i2);
			Weight wi1 = instance.weight(i1) + multiplier;
			Weight wi2 = instance.weight(i2) + multiplier;
			if (pi1 * wi2 > pi2 * wi1) {
				return true;
			} else if (pi1 * wi2 < pi2 * wi1) {
				return false;
			} else {
				return instance.weight(i1) > instance.weight(i2);
			}},
			capacity(),
			[&instance, &multiplier](ItemIdx i) { return instance.weight(i) + multiplier; });

	for (ItemIdx i=1; i<=n_; ++i) {
		set_weight(i, weight_parent(i) + multiplier);
		set_profit(i, profit_parent(i));
	}
	DBG(std::cout << "Instance::surrogate_plus()... END" << std::endl;)
}

#undef DBG

#define DBG(x)
//#define DBG(x) x

void Instance::surrogate_minus(const Instance& instance, Weight multiplier, ItemIdx bound)
{
	DBG(std::cout << "Instance::surrogate_minus()..." << std::endl;)

	name_ = instance.name() + "_minus";
	c_ = instance.capacity();
	parent_ = &instance;
	opt_ = 0;
	n_ = 0;
	for (ItemIdx i=1; i<=instance.item_number(); ++i) {
		if (instance.weight(i) > multiplier) {
			n_++;
			set_index(n_, i);
			solution_->set(i, false);
		} else {
			c_ += multiplier - instance.weight(i);
			solution_->set(i, true);
		}
	}
	c_ -= c_ - multiplier * bound;

	partial_sort(
			i_, n_,
			[&instance, &multiplier](ItemIdx i1, ItemIdx i2) {
			Profit pi1 = instance.profit(i1);
			Profit pi2 = instance.profit(i2);
			Weight wi1 = instance.weight(i1) - multiplier;
			Weight wi2 = instance.weight(i2) - multiplier;
			if (pi1 * wi2 > pi2 * wi1) {
				return true;
			} else if (pi1 * wi2 < pi2 * wi1) {
				return false;
			} else {
				return instance.weight(i1) > instance.weight(i2);
			}},
			capacity(),
			[&instance, &multiplier](ItemIdx i) { return instance.weight(i) - multiplier; });

	for (ItemIdx i=1; i<=n_; ++i) {
		assert(weight_parent(i) >= multiplier);
		set_weight(i, weight_parent(i) - multiplier);
		set_profit(i, profit_parent(i));
	}
	DBG(std::cout << "Instance::surrogate_minus()... END" << std::endl;)
}

#undef DBG

#define DBG(x)
//#define DBG(x) x

Instance Instance::reduce(const Instance& instance, Profit lower_bound)
{
	DBG(std::cout << "reduce()..." << std::endl;)

	Instance instance_new;
	instance_new.init(instance);
	instance_new.name_ += "_reduced";
	instance_new.c_   = instance.capacity();
	instance_new.opt_ = instance.optimum();
	instance_new.n_   = 0;
	instance_new.i_   = new ItemIdx[instance.item_number()];
	Weight c = instance.capacity();

	Weight wi;
	Profit pi;
	for (ItemIdx i=1; i<=instance.item_number(); ++i) {
		DBG(std::cout << "i: " << i << " " << std::flush;)

		wi = instance.weight(i);
		pi = instance.profit(i);
		DBG(std::cout << "pi: " << pi << " wi: " << wi << std::flush;)

		if (ub_dantzig_without(instance, i, c) < lower_bound) {
			// If item i is in any optimal solution
			DBG(std::cout << "1 " << std::flush;)
			instance_new.solution_->set(i, true);
			instance_new.c_   -= wi;
			instance_new.opt_ -= pi;
			assert(instance.instance_orig()->optimum(instance.index_orig(i)));
		} else if (pi + ub_dantzig_without(instance, i, c-wi) < lower_bound) {
			// If item i is not in any optimal solution
			DBG(std::cout << "0 " << std::flush;)
			assert(!instance.instance_orig()->optimum(instance.index_orig(i)));
		} else {
			// Item i may be in an optimal solution
			DBG(std::cout << "? " << std::flush;)
			instance_new.n_++;
			instance_new.set_index(instance_new.n_, i);
		}
		DBG(std::cout << std::endl;)
	}

	instance_new.set_profits_and_weights_from_parent();

	DBG(std::cout << "reduce()... END" << std::endl;)
	return instance_new;
}

#undef DBG

Instance Instance::divide_weights_floor(const Instance& instance, Weight divisor)
{
	Instance instance_new;
	instance_new.init(instance);
	instance_new.n_   = instance.item_number();
	instance_new.c_   = instance.capacity() / divisor;
	instance_new.opt_ = instance.optimum();
	instance_new.w_ = new Weight[instance_new.instance_orig()->item_number()];
	instance_new.p_ = new Profit[instance_new.instance_orig()->item_number()];
	instance_new.i_ = new ItemIdx[instance_new.instance_orig()->item_number()];
	for (ItemIdx i=1; i<=instance_new.n_; ++i) {
		instance_new.set_index(i, i);
		instance_new.set_weight(i, instance.weight(i) / divisor);
		instance_new.set_profit(i, instance.profit(i));
	}
	return instance_new;
}

Instance Instance::divide_weights_ceil(const Instance& instance, Weight divisor)
{
	Instance instance_new;
	instance_new.init(instance);
	instance_new.n_   = instance.item_number();
	instance_new.c_   = (instance.capacity() != 0)?
		1 + ((instance.capacity() - 1) / divisor): 0;
	instance_new.opt_ = instance.optimum();
	instance_new.w_ = new Weight[instance_new.instance_orig()->item_number()];
	instance_new.p_ = new Profit[instance_new.instance_orig()->item_number()];
	instance_new.i_ = new ItemIdx[instance_new.instance_orig()->item_number()];
	for (ItemIdx i=1; i<=instance_new.n_; ++i) {
		instance_new.set_index(i, i);
		instance_new.set_weight(i, (instance.weight(i) != 0)?
				1 + ((instance.weight(i) - 1) / divisor): 0);
		instance_new.set_profit(i, instance.profit(i));
	}
	return instance_new;
}

Instance Instance::divide_profits_floor(const Instance& instance, Profit divisor)
{
	Instance instance_new;
	instance_new.init(instance);
	instance_new.n_   = instance.item_number();
	instance_new.c_   = instance.capacity();
	instance_new.opt_ = 0;
	instance_new.w_ = new Weight[instance_new.instance_orig()->item_number()];
	instance_new.p_ = new Profit[instance_new.instance_orig()->item_number()];
	instance_new.i_ = new ItemIdx[instance_new.instance_orig()->item_number()];
	for (ItemIdx i=1; i<=instance_new.n_; ++i) {
		instance_new.set_index(i, i);
		instance_new.set_weight(i, instance.weight(i));
		instance_new.set_profit(i, instance.profit(i) / divisor);
	}
	return instance_new;
}

Instance Instance::divide_profits_ceil(const Instance& instance, Profit divisor)
{
	Instance instance_new;
	instance_new.init(instance);
	instance_new.n_   = instance.item_number();
	instance_new.c_   = instance.capacity();
	instance_new.opt_ = 0;
	instance_new.w_ = new Weight[instance_new.instance_orig()->item_number()];
	instance_new.p_ = new Profit[instance_new.instance_orig()->item_number()];
	instance_new.i_ = new ItemIdx[instance_new.instance_orig()->item_number()];
	for (ItemIdx i=1; i<=instance_new.n_; ++i) {
		instance_new.set_index(i, i);
		instance_new.set_profit(i, (instance.profit(i) != 0)?
				1 + ((instance.profit(i) - 1) / divisor): 0);
		instance_new.set_weight(i, instance.weight(i));
	}
	return instance_new;
}

Instance Instance::from_to(const Instance& instance, ItemIdx i1, ItemIdx i2, Weight c)
{
	Instance instance_new;
	instance_new.init(instance);
	instance_new.n_   = i2 - i1 + 1;
	instance_new.c_   = c;
	instance_new.opt_ = 0;
	instance_new.w_ = new Weight[instance_new.instance_orig()->item_number()];
	instance_new.p_ = new Profit[instance_new.instance_orig()->item_number()];
	instance_new.i_ = new ItemIdx[instance_new.instance_orig()->item_number()];
	for (ItemIdx i=1; i<=instance_new.n_; ++i)
		instance_new.set_index(i, i+i1-1);
	instance_new.set_profits_and_weights_from_parent();
	return instance_new;
}

Instance Instance::fix(const Instance& instance, ItemIdx j, Weight w)
{
	Instance instance_new;
	instance_new.init(instance);
	instance_new.n_   = instance.item_number() - 1;
	instance_new.c_   = instance.capacity() - w;
	instance_new.opt_ = 0;
	instance_new.w_ = new Weight[instance_new.instance_orig()->item_number()];
	instance_new.p_ = new Profit[instance_new.instance_orig()->item_number()];
	instance_new.i_ = new ItemIdx[instance_new.instance_orig()->item_number()];
	for (ItemIdx i=1; i<=j-1; ++i)
		instance_new.set_index(i, i);
	for (ItemIdx i=j+1; i<=instance_new.n_; ++i)
		instance_new.set_index(i, i+1);
	instance_new.set_profits_and_weights_from_parent();
	return instance_new;
}

void Instance::info(std::ostream& os)
{
	os
		<< n_ << " / " << instance_orig()->item_number() << " (" << (double)n_ / (double)instance_orig()->item_number() << "); "
		<< c_ << " / " << instance_orig()->capacity()    << " (" << (double)c_ / (double)instance_orig()->capacity()    << "); ";
	if (solution_ != NULL)
		os << solution_->profit();
	os << std::endl;
}

std::ostream& operator<<(std::ostream& os, const Instance& instance)
{
	os << instance.item_number() << " " << instance.capacity() << std::endl;
	for (ItemIdx i=1; i<=instance.item_number(); ++i)
		os << instance.index(i) << " " << instance.profit(i) << " " << instance.weight(i) << " " << (double)instance.profit(i)/(double)instance.weight(i) << std::endl;
	return os;
}

Weight Instance::gcd() const
{
	Weight gcd = capacity();
	for (ItemIdx i=1; i<=item_number(); ++i) {
		if (gcd == 1)
			return gcd;
		Weight wi = weight(i);
		while (wi != 0) {
			Weight tmp = gcd % wi;
			gcd = wi;
			wi = tmp;
		}
	}
	return gcd;
}

ItemIdx Instance::max_weight_item() const
{
	ItemIdx j = 0;
	Profit p  = 0;
	Weight w_max = 0;
	for (ItemIdx i=1; i<=item_number(); ++i) {
		if (weight(i) > capacity())
			continue;
		if (weight(i) > w_max || (weight(i) == w_max && profit(i) > p)) {
			j     = i;
			w_max = weight(i);
			p     = profit(i);
		}
	}
	return j;
}

ItemIdx Instance::max_profit_item() const
{
	ItemIdx    j = 0;
	Profit p_max = 0;
	Weight w     = capacity();
	for (ItemIdx i=1; i<=item_number(); ++i) {
		if (weight(i) > capacity())
			continue;
		if (profit(i) > p_max || (profit(i) == p_max && weight(i) < w)) {
			j     = i;
			p_max = profit(i);
			w     = weight(i);
		}
	}
	return j;
}
