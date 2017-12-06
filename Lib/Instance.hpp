#ifndef PARSER_HPP_CWOQFZGE
#define PARSER_HPP_CWOQFZGE

#include <cstdint>
#include <random>
#include <string>
#include <algorithm>
#include <iostream>
#include <fstream>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/fstream.hpp>

typedef  int_fast64_t Profit;
typedef uint_fast64_t Weight;
typedef size_t ItemIdx;
typedef size_t ValIdx;

class Solution;

class Instance
{

public:

	Instance(boost::filesystem::path filename);
	void read_standard(boost::filesystem::path filename);
	void read_pisinger(boost::filesystem::path filename);

	/**
	 * Create an instance with the same items as in the one in parameter, but
	 * for which they are sorted in non-decreasing order of their profit over
	 * weight ratio.
	 * Note that if instance_2 = Instance(instance_1), Then instance_2.index(i)
	 * returns the index of item i in instance_1.
	 */
	Instance(const Instance& instance);

	/**
	 * Create an instance such that if instance_2 = Instance(instance_1,
	 * lower_bound), if item i is not in instance_2, then there exists an
	 * optimal solution such that item i is as in instance_2.solution().
	 */
	Instance(const Instance& instance, Profit lower_bound);

	~Instance();

	inline std::string name()       const { return name_; }
	inline ItemIdx item_number()    const { return n_; }
	inline Weight capacity()        const { return c_; }
	inline Profit optimum()         const { return opt_; }
	inline Weight weight(ItemIdx i) const { return w_[i-1]; }
	inline Profit profit(ItemIdx i) const { return p_[i-1]; }
	inline ItemIdx index(ItemIdx i) const { return i_[i-1]; }

	Solution* solution() const { return solution_; }
	const Instance* instance_orig() const { return instance_orig_; }
	Profit check(boost::filesystem::path cert_file);
	void info(std::ostream& os);

private:

	std::string name_;
	std::string format_;

	ItemIdx n_;
	Weight  c_;
	Weight  w_max_;
	Profit  p_max_;
	Profit  opt_ = 0;
	Weight* w_;
	Profit* p_;
	std::vector<ItemIdx> i_;
	const Instance* instance_orig_ = NULL;
	Solution* solution_ = NULL;

};

std::ostream& operator<<(std::ostream &os, const Instance& instance);

#endif /* end of include guard: PARSER_HPP_CWOQFZGE */
