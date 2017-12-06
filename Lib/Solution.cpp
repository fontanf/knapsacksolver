#include "Solution.hpp"

Solution::Solution(const Instance& instance):
	instance_(instance), n_(instance.item_number()),
	p_(0), r_(instance.capacity())
{
	x_ = new bool[n_];
	for (ItemIdx i=1; i<=n_; i++)
		x_[i-1] = false;
	assert(profit() == 0);
}

Solution::Solution(const Solution& solution):
	instance_(solution.instance()), n_(instance_.item_number()),
	p_(solution.profit()), r_(solution.remaining_capacity())
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

void Solution::set(ItemIdx i, bool b)
{
	if (b) {
		assert(!get(i));
		assert(instance_.weight(i) <= r_);
		p_ += instance_.profit(i);
		r_ -= instance_.weight(i);
	} else {
		assert(get(i));
		p_ -= instance_.profit(i);
		r_ += instance_.weight(i);
	}
	x_[i-1] = b;
}

Solution Solution::get_orig() const
{
	Solution solution(*(instance_.solution()));
	assert(instance_.instance_orig()->item_number() == instance_.solution()->instance().item_number());
	for (ItemIdx i=1; i<=n_; ++i)
		if (get(i))
			solution.set(instance_.index(i), true);
	return solution;
}

std::ostream& operator<<(std::ostream& os, const Solution& solution)
{
	for (ItemIdx i=1; i<=solution.instance().item_number(); ++i)
		os << (solution.get(i)? 1: 0) << std::endl;
	return os;
}
