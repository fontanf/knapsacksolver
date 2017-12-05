#include "PartialInstance.hpp"

PartialInstance::PartialInstance(const Instance& instance)
{
	n_ = instance.item_number();
	c_ = instance.capacity();
	p_old_ = 0;

	i_ = std::vector<ItemIdx>(n_);
	iota(i_.begin(), i_.end(), 1);
	sort(i_.begin(), i_.end(),
			[&instance](ItemIdx i1, ItemIdx i2) {
			return instance.profit(i1) * instance.weight(i2)
			< instance.profit(i2) * instance.weight(i1);});

	w_ = new Weight[n_];
	p_ = new Profit[n_];
	for (ItemIdx i=0; i<n_; ++i) {
		w_[i] = instance.weight(i_[i]);
		p_[i] = instance.profit(i_[i]);
	}
}

PartialInstance::PartialInstance(
		const PartialInstance& instance,
		std::vector<char>& reduction_vector,
		std::vector<bool>& sol_cur)
{
	n_ = 0;
	c_ = instance.capacity();
	p_old_ = 0;
	for (ItemIdx i=1; i<=instance.item_number(); ++i) {
		if (reduction_vector[i-1] == '?') {
			n_++;
		} else if (reduction_vector[i-1] == '1') {
			c_ -= instance.weight(i);
			p_old_ += instance.profit(i);
			sol_cur[instance.index(i)] = true;
		}
	}
	w_ = new Weight[n_];
	p_ = new Profit[n_];
	ItemIdx j=1;
	for (ItemIdx i=1; i<=instance.item_number(); ++i) {
		if (reduction_vector[i-1] == '?') {
			i_.push_back(instance.index(i));
			w_[j-1] = instance.weight(i);
			p_[j-1] = instance.profit(i);
			j++;
		}
	}
}

Solution::Solution(const PartialInstance& instance):
	instance_(instance), n_(instance.item_number()), r_(instance.capacity())
{
	x_ = new bool[n_];
	for (ItemIdx i=1; i<=n_; i++)
		x_[i-1] = false;
}


Solution::Solution(const Solution& solution):
	instance_(solution.instance()), n_(instance_.item_number()), r_(instance_.capacity())
{
	x_ = new bool[n_];
	for (ItemIdx i=1; i<=n_; i++)
		x_[i-1] = solution.get(i);
}

Solution& Solution::operator=(const Solution& solution)
{
	if (this != &solution) {
		assert(&instance_ == &(solution.instance()));
		for (ItemIdx i=1; i<=n_; i++)
			x_[i-1] = solution.get(i);
		p_ = solution.profit();
		r_ = solution.remaining_capacity();
	}
	return *this;
}

PartialInstance::~PartialInstance()
{
	delete[] w_;
	delete[] p_;
};

inline void Solution::set(ItemIdx i, bool b)
{
	if (b) {
		assert(!x_[i]);
		assert(instance_.weight(i) <= r_);
		p_ += instance_.profit(i);
		r_ -= instance_.weight(i);
	} else {
		assert(x_[i]);
		p_ -= instance_.profit(i);
		r_ += instance_.weight(i);
	}
	x_[i] = b;
}
