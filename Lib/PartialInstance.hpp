#ifndef PARTIALINSTANCE_HPP_JO8HVNRY
#define PARTIALINSTANCE_HPP_JO8HVNRY

#include "Parser.hpp"

class PartialInstance
{

public:

	PartialInstance(const Instance& instance);
	PartialInstance(
			const PartialInstance& instance,
			std::vector<char>& reduction_vector,
			std::vector<bool>& sol_cur);
	~PartialInstance();

	inline ItemIdx item_number() const { return n_; }
	inline Weight capacity() const { return c_; }
	inline Weight weight(ItemIdx i) const { return w_[i-1]; }
	inline Profit profit(ItemIdx i) const { return p_[i-1]; }
	inline Profit old_profit() const { return p_old_; };
	inline ItemIdx index(ItemIdx i) const { return i_[i-1]; }

private:

	ItemIdx n_;
	Weight  c_;
	Profit  p_old_;
	Profit* p_;
	Weight* w_;
	std::vector<ItemIdx> i_;

};

class Solution
{

public:

	Solution(const PartialInstance& instance);
	Solution(const Solution& solution);
	Solution& operator=(const Solution& solution);
	~Solution() { delete[] x_; };

	const PartialInstance& instance() const { return instance_; }

	inline Weight remaining_capacity() const { return r_; }
	inline Profit profit()             const { return p_; }

	inline void set(ItemIdx i, bool b);
	inline bool get(ItemIdx i) const { return x_[i-1]; }

private:

	const PartialInstance& instance_;
	const ItemIdx n_;
	bool* x_;
	Profit p_;
	Weight r_;

};

#endif /* end of include guard: PARTIALINSTANCE_HPP_JO8HVNRY */
