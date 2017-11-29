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

class KnapsackInstance
{

public:

	KnapsackInstance(boost::filesystem::path filename);
	void KnapsackInstanceStandard(boost::filesystem::path filename);
	void KnapsackInstancePisinger(boost::filesystem::path filename);

	~KnapsackInstance();

	std::string name() const { return name_; }
	ItemIdx itemNumber() const { return n_; }
	Weight capacity() const { return c_; }
	Profit optimum() const { return opt_; }
	Weight weight(ItemIdx i) const { return w_[i-1]; }
	Profit profit(ItemIdx i) const { return p_[i-1]; }
	Profit profit(std::vector<ItemIdx> v) const;

	Profit check(boost::filesystem::path cert_file);

private:

	std::string name_;
	std::string format_;

	ItemIdx n_;
	Weight  c_;
	Weight  w_max_;
	Profit  opt_ = 0;
	Profit  p_max_;
	Weight* w_;
	Profit* p_;

};

std::ostream& operator<<(std::ostream &os, const std::vector<bool>& sol);

#endif /* end of include guard: PARSER_HPP_CWOQFZGE */
