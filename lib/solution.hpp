#ifndef SOLUTION_HPP_JYHGMADO
#define SOLUTION_HPP_JYHGMADO

#include "instance.hpp"

class Solution
{

public:

	Solution(const Instance& instance);
	Solution(const Solution& solution);
	Solution& operator=(const Solution& solution);
	~Solution() { delete[] x_; };

	const Instance* instance() const { assert(instance_ != NULL); return instance_; }

	inline Weight remaining_capacity() const { return r_; }
	inline Profit profit()             const { return p_; }
	inline ItemIdx item_number()       const { return k_; }

	void set(ItemIdx i, bool b);
	inline bool get(ItemIdx i) const { return x_[i-1]; }

	Solution get_orig() const;
	Profit profit_orig() const { return p_ + instance()->profit_orig(); }

private:

	const Instance* instance_;

	ItemIdx k_;
	bool*   x_;
	Profit  p_;
	Weight  r_;

};

std::ostream& operator<<(std::ostream &os, const Solution& solution);

#endif /* end of include guard: SOLUTION_HPP_JYHGMADO */
