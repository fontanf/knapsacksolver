#ifndef SOLUTION_HPP_JYHGMADO
#define SOLUTION_HPP_JYHGMADO

#include "Instance.hpp"

class Solution
{

public:

	Solution(const Instance& instance);
	Solution(const Solution& solution);
	Solution& operator=(const Solution& solution);
	~Solution() { delete[] x_; };

	const Instance& instance() const { return instance_; }

	inline Weight remaining_capacity() const { return r_; }
	inline Profit profit()             const { return p_; }

	void set(ItemIdx i, bool b);
	inline bool get(ItemIdx i) const { return x_[i-1]; }

	Solution get_orig() const;

private:

	const Instance& instance_;
	ItemIdx n_;
	bool* x_;
	Profit p_;
	Weight r_;

};

std::ostream& operator<<(std::ostream &os, const Solution& solution);

#endif /* end of include guard: SOLUTION_HPP_JYHGMADO */
