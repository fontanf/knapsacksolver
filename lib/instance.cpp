#include "instance.hpp"
#include "solution.hpp"

#include "../ub_dantzig/dantzig.hpp"

#include <sstream>

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

	i_ = std::vector<ItemIdx>(n_);
	iota(i_.begin(), i_.end(), 1);
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
	for (ItemIdx i=1; i<=n_; ++i) {
		set_weight(i, instance.weight(i));
		set_profit(i, instance.profit(i));
		i_.push_back(instance.index(i));
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
	instance_new.i_   = std::vector<ItemIdx>(instance_new.n_);
	iota(instance_new.i_.begin(), instance_new.i_.end(), 1);
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

Instance Instance::sort_by_profit(const Instance& instance)
{
	Instance instance_new;
	instance_new.init(instance);
	instance_new.name_ += "_profit";
	instance_new.n_   = instance.item_number();
	instance_new.c_   = instance.capacity();
	instance_new.opt_ = instance.optimum();
	instance_new.i_   = std::vector<ItemIdx>(instance_new.n_);
	iota(instance_new.i_.begin(), instance_new.i_.end(), 1);
	std::sort(instance_new.i_.begin(), instance_new.i_.end(),
			[&instance](ItemIdx i1, ItemIdx i2) {
			return instance.profit(i1) < instance.profit(i2);});
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
	instance_new.i_   = std::vector<ItemIdx>(instance_new.n_);
	iota(instance_new.i_.begin(), instance_new.i_.end(), 1);
	std::sort(instance_new.i_.begin(), instance_new.i_.end(),
			[&instance](ItemIdx i1, ItemIdx i2) {
			return instance.weight(i1) < instance.weight(i2);});
	instance_new.set_profits_and_weights_from_parent();
	return instance_new;
}

Instance Instance::sort_by_density(const Instance& instance)
{
	Instance instance_new;
	instance_new.init(instance);
	instance_new.name_ += "_density";
	instance_new.n_   = instance.item_number();
	instance_new.c_   = instance.capacity();
	instance_new.opt_ = instance.optimum();
	instance_new.i_   = std::vector<ItemIdx>(instance_new.n_);
	iota(instance_new.i_.begin(), instance_new.i_.end(), 1);
	std::sort(instance_new.i_.begin(), instance_new.i_.end(),
			[&instance](ItemIdx i1, ItemIdx i2) {
			return instance.profit(i1) * instance.weight(i2)
			     < instance.profit(i2) * instance.weight(i1);});
	instance_new.set_profits_and_weights_from_parent();
	return instance_new;
}

Instance Instance::remove_first_item(const Instance& instance)
{
	Instance instance_new;
	instance_new.init(instance);
	assert(instance.item_number() > 0);
	instance_new.name_  += "_dltfirst";
	instance_new.n_      = instance.item_number() - 1;
	instance_new.c_      = instance.capacity();
	instance_new.opt_    = 0;
	instance_new.parent_ = &instance;
	instance_new.i_      = std::vector<ItemIdx>(instance_new.n_);
	iota(instance_new.i_.begin(), instance_new.i_.end(), 2);
	instance_new.w_ = new Weight[instance.instance_orig()->item_number()];
	instance_new.p_ = new Profit[instance.instance_orig()->item_number()];
	for (ItemIdx i=1; i<=instance_new.item_number(); ++i) {
		instance_new.set_weight(i, instance.weight(i+1));
		instance_new.set_profit(i, instance.profit(i+1));
	}
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
	std::sort(i_.begin(), i_.end(),
			[&instance, &multiplier](ItemIdx i1, ItemIdx i2) {
			Profit pi1 = instance.profit(i1);
			Profit pi2 = instance.profit(i2);
			Weight wi1 = instance.weight(i1) + multiplier;
			Weight wi2 = instance.weight(i2) + multiplier;
			if (pi1 * wi2 < pi2 * wi1) {
				return true;
			} else if (pi1 * wi2 > pi2 * wi1) {
				return false;
			} else {
				return instance.weight(i1) < instance.weight(i2);
			}});
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
	i_.clear();
	for (ItemIdx i=1; i<=instance.item_number(); ++i) {
		if (instance.weight(i) > multiplier) {
			i_.push_back(i);
			solution_->set(i, false);
		} else {
			c_ += multiplier - instance.weight(i);
			solution_->set(i, true);
		}
		n_ = i_.size();
	}
	c_ = (c_ > multiplier * bound)?
		c_ - multiplier * bound: 0;
	std::sort(i_.begin(), i_.end(),
			[&instance, &multiplier](ItemIdx i1, ItemIdx i2) {
			Profit pi1 = instance.profit(i1);
			Profit pi2 = instance.profit(i2);
			Weight wi1 = instance.weight(i1) - multiplier;
			Weight wi2 = instance.weight(i2) - multiplier;
			if (pi1 * wi2 < pi2 * wi1) {
				return true;
			} else if (pi1 * wi2 > pi2 * wi1) {
				return false;
			} else {
				return instance.weight(i1) < instance.weight(i2);
			}});
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
	Weight c = instance.capacity();

	ItemIdx i=1;
	Weight wi;
	Profit pi;
	Instance instance_tmp = Instance::remove_first_item(instance);
	for (;;i++) {
		DBG(std::cout << "i: " << i << " " << std::flush;)

		wi = instance.weight(i);
		pi = instance.profit(i);
		DBG(std::cout << "pi: " << pi << " wi: " << wi << std::flush;)

		if (ub_dantzig(instance_tmp.set_capacity(c)) < lower_bound) {
			// If item i is in any optimal solution
			DBG(std::cout << "1 " << std::flush;)
			instance_new.solution_->set(i, true);
			instance_new.c_   -= wi;
			instance_new.opt_ -= pi;
			assert(instance.instance_orig()->optimum(instance.index_orig(i)));
		} else if (pi + ub_dantzig(instance_tmp.set_capacity(c-wi)) < lower_bound) {
			// If item i is not in any optimal solution
			DBG(std::cout << "0 " << std::flush;)
			assert(!instance.instance_orig()->optimum(instance.index_orig(i)));
		} else {
			// Item i may be in an optimal solution
			DBG(std::cout << "? " << std::flush;)
			instance_new.i_.push_back(i);
		}

		if (i == instance.item_number())
			break;

		instance_tmp.set_weight(i, wi);
		instance_tmp.set_profit(i, pi);
		instance_tmp.set_index(i, i);
		DBG(std::cout << std::endl;)
	}

	instance_new.n_ = instance_new.i_.size();
	instance_new.set_profits_and_weights_from_parent();

	DBG(std::cout << "reduce()... END" << std::endl;)
	return instance_new;
}

#undef DBG

Instance Instance::divide_floor(const Instance& instance, Weight divisor)
{
	Instance instance_new;
	instance_new.init(instance);
	instance_new.n_   = instance.item_number();
	instance_new.c_   = instance.capacity() / divisor;
	instance_new.opt_ = instance.optimum();
	instance_new.i_   = std::vector<ItemIdx>(instance_new.n_);
	iota(instance_new.i_.begin(), instance_new.i_.end(), 1);
	instance_new.w_ = new Weight[instance_new.instance_orig()->item_number()];
	instance_new.p_ = new Profit[instance_new.instance_orig()->item_number()];
	for (ItemIdx i=1; i<=instance_new.n_; ++i) {
		instance_new.set_weight(i, instance.weight(i) / divisor);
		instance_new.set_profit(i, instance.profit(i));
	}
	return instance_new;
}

Instance Instance::divide_ceil(const Instance& instance, Weight divisor)
{
	Instance instance_new;
	instance_new.init(instance);
	instance_new.n_   = instance.item_number();
	instance_new.c_   = (instance.capacity() != 0)?
		1 + ((instance.capacity() - 1) / divisor): 0;
	instance_new.opt_ = instance.optimum();
	instance_new.i_   = std::vector<ItemIdx>(instance_new.n_);
	iota(instance_new.i_.begin(), instance_new.i_.end(), 1);
	instance_new.w_ = new Weight[instance_new.instance_orig()->item_number()];
	instance_new.p_ = new Profit[instance_new.instance_orig()->item_number()];
	for (ItemIdx i=1; i<=instance_new.n_; ++i) {
		instance_new.set_weight(i, (instance.weight(i) != 0)?
				1 + ((instance.weight(i) - 1) / divisor): 0);
		instance_new.set_profit(i, instance.profit(i));
	}
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

Weight Instance::weight_max() const
{
	Weight w_max = 0;
	for (ItemIdx i=1; i<=item_number(); ++i)
		if (weight(i) > w_max)
			w_max = weight(i);
	return w_max;
}

Profit Instance::profit_max() const
{
	Profit p_max = 0;
	for (ItemIdx i=1; i<=item_number(); ++i)
		if (profit(i) > p_max)
			p_max = profit(i);
	return p_max;
}
