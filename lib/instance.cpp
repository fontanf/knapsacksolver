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

	assert(check());
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

bool Instance::check()
{
	for (ItemIdx i=1; i<=item_number(); ++i)
		if (weight(i) <= 0 || weight(i) > capacity())
			return false;
	return true;
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
	sort_type_ = instance.sort_type();
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

Instance Instance::sort_by_efficiency(const Instance& instance)
{
	Instance ins;
	ins.init(instance);
	ins.name_ += "_eff";
	ins.n_   = instance.item_number();
	ins.c_   = instance.capacity();
	ins.opt_ = instance.optimum();
	ins.i_   = new ItemIdx[ins.n_];
	ins.sort_type_ = "efficiency";
	for (ItemIdx i=1; i<=ins.n_; ++i)
		ins.set_index(i, i);
	std::sort(ins.i_, ins.i_ + ins.n_,
			[&instance](ItemIdx i1, ItemIdx i2) {
			return instance.profit(i1) * instance.weight(i2)
			     > instance.profit(i2) * instance.weight(i1);});
	ins.set_profits_and_weights_from_parent();

	ins.r_    = ins.capacity();
	ins.psum_ = 0;
	ins.b_    = ins.item_number() + 1;
	for (ItemIdx i=1; i<=ins.item_number(); ++i) {
		if (ins.r_ < ins.weight(i)) {
			ins.b_ = i;
			break;
		}
		ins.r_    -= ins.weight(i);
		ins.psum_ += ins.profit(i);
	}
	ins.wsum_ = ins.capacity() - ins.r_;
	return ins;
}

#define DBG(x)
//#define DBG(x) x

inline void swap(ItemIdx* v, ItemIdx i, ItemIdx j)
{
	ItemIdx tmp = v[j];
	v[j] = v[i];
	v[i] = tmp;
}

void Instance::partial_sort(Weight s)
{
	DBG(std::cout << "Partial sort s " << s << std::endl;)
	if (item_number() == 0) {
		b_    = 1;
		psum_ = 0;
		wsum_ = 0;
		r_    = capacity();
		return;
	} else if (item_number() == 1) {
		b_    = 2;
		psum_ = parent()->profit(1);
		wsum_ = parent()->weight(1);
		r_    = capacity() - wsum_;
		return;
	}

	ItemIdx f = 0;
	ItemIdx l = (n_ > 0)? n_ - 1: 0;
	wsum_ = 0;
	psum_ = 0;
	while (f < l) {
		ItemIdx pivot = f + 1 + rand() % (l - f); // Select pivot

		//for (ItemIdx i=0; i<item_number(); ++i)
			//std::cout << i_[i] << " ";
		//std::cout << std::endl;
		//std::cout << "f " << f << " l " << l << " pivot " << pivot << std::endl;

		swap(i_, pivot, l);
		ItemIdx j = f;
		for (ItemIdx i=f; i<l; ++i) {
			if (parent()->profit(i_[i])*(parent()->weight(i_[l])+s)
					<= parent()->profit(i_[l])*(parent()->weight(i_[i])+s))
				continue;
			swap(i_, i, j);
			j++;
		}
		swap(i_, j, l);

		Weight w_curr = wsum_;
		for (ItemIdx i=f; i<j; ++i)
			w_curr += parent()->weight(i_[i]) + s;
		//std::cout << "j " << j << " w " << w_curr << " c " << c_ << " w+wj " << w_curr + parent()->weight(i_[j]) + s << std::endl;

		if (w_curr + parent()->weight(i_[j]) + s <= c_) {
			for (ItemIdx i=f; i<=j; ++i)
				psum_ += parent()->profit(i_[i]);
			wsum_ = w_curr + parent()->weight(i_[j]) + s;
			f = j+1;
		} else if (w_curr > c_) {
			l = j-1;
		} else {
			for (ItemIdx i=f; i<j; ++i)
				psum_ += parent()->profit(i_[i]);
			wsum_ = w_curr;
			f = j;
			break;
		}
	}
	if (f == item_number() - 1 && wsum_ + parent()->weight(i_[f]) + s <= c_) {
		b_ = item_number() + 1;
		wsum_ += parent()->weight(i_[f]) + s;
		psum_ += parent()->profit(i_[f]);
	} else {
		b_ = f+1;
	}
	r_ = c_ - wsum_;

	//for (ItemIdx i=0; i<item_number(); ++i)
		//std::cout << i_[i] << " ";
	//std::cout << std::endl;

	//std::cout << "b " << b_ << " wsum " << wsum_ << " psum " << psum_ << " r " << r_ << std::endl;
	DBG(std::cout << "Partial sort END" << std::endl;)
}

Instance Instance::sort_partially_by_efficiency(const Instance& instance)
{
	Instance ins;
	ins.init(instance);
	ins.name_ += "_peff";
	ins.n_   = instance.item_number();
	ins.c_   = instance.capacity();
	ins.opt_ = instance.optimum();
	ins.i_   = new ItemIdx[ins.n_];
	std::iota(ins.i_, ins.i_+instance.item_number(), 1);
	ins.sort_type_ = "partial_efficiency";
	ins.partial_sort(0);
	ins.set_profits_and_weights_from_parent();
	return ins;
}

#undef DBG

#define DBG(x)
//#define DBG(x) x

void Instance::surrogate(const Instance& instance, Weight multiplier, ItemIdx bound)
{
	DBG(std::cout << "Instance::surrogate()..." << std::endl;)

	name_   = instance.name() + "_surrogate";
	c_      = instance.capacity();
	parent_ = &instance;
	opt_    = 0;
	n_      = 0;
	for (ItemIdx i=1; i<=instance.item_number(); ++i) {
		if (instance.weight(i) + multiplier > 0) {
			n_++;
			set_index(n_, i);
			solution_->set(i, false);
		} else {
			c_ -= (instance.weight(i) + multiplier);
			solution_->set(i, true);
		}
	}
	c_ += multiplier * bound;
	partial_sort(multiplier);
	for (ItemIdx i=1; i<=n_; ++i) {
		assert(weight_parent(i) + multiplier > 0);
		set_weight(i, weight_parent(i) + multiplier);
		set_profit(i, profit_parent(i));
	}
	DBG(std::cout << "Instance::surrogate()... END" << std::endl;)
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

