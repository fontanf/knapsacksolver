#include "solution.hpp"

Solution::Solution(const Instance& instance):
	instance_(&instance), k_(0), p_(0), r_(instance.capacity())
{
	x_ = new bool[instance_->instance_orig()->item_number()];
	for (ItemIdx i=1; i<=instance_->item_number(); i++)
		x_[i-1] = false;
	assert(profit() == 0);
}

Solution::Solution(const Solution& solution):
	instance_(solution.instance()), k_(0), p_(0), r_(solution.instance()->capacity())
{
	ItemIdx n = instance_->item_number();
	x_ = new bool[n];
	for (ItemIdx i=1; i<=n; i++) {
		x_[i-1] = false;
		set(i, solution.get(i));
	}
}

Solution& Solution::operator=(const Solution& solution)
{
	if (this != &solution) {
		assert(instance() == solution.instance());
		ItemIdx n = solution.instance()->item_number();
		if (instance()->item_number() == n) {
			k_ = 0;
			r_ = solution.instance()->capacity();
			instance_ = solution.instance();
			for (ItemIdx i=1; i<=n; i++)
				set(i, solution.get(i));
		} else {
			k_ = 0;
			r_ = solution.instance()->capacity();
			instance_ = solution.instance();
			if (x_ != NULL)
				delete[] x_;
			x_ = new bool[n];
			for (ItemIdx i=1; i<=n; i++) {
				x_[i-1] = false;
				set(i, solution.get(i));
			}
		}
	}
	return *this;
}

void Solution::set(ItemIdx i, bool b)
{
	if (get(i) == b)
		return;
	if (b) {
		p_ += instance()->profit(i);
		r_ -= instance()->weight(i);
		k_++;
	} else {
		p_ -= instance()->profit(i);
		r_ += instance()->weight(i);
		k_--;
	}
	x_[i-1] = b;
}

Solution Solution::get_orig() const
{
	return instance()->solution_orig(*this);
}

std::ostream& operator<<(std::ostream& os, const Solution& solution)
{
	for (ItemIdx i=1; i<=solution.instance()->item_number(); ++i)
		os << (solution.get(i)? 1: 0) << std::endl;
	return os;
}
