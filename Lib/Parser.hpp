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

class Instance
{

public:

	Instance(boost::filesystem::path filename);
	void read_standard(boost::filesystem::path filename);
	void read_pisinger(boost::filesystem::path filename);

	~Instance();

	std::string name() const { return name_; }
	ItemIdx item_number() const { return n_; }
	Weight capacity() const { return c_; }
	Profit optimum() const { return opt_; }
	Weight weight(ItemIdx i) const { return w_[i-1]; }
	Profit profit(ItemIdx i) const { return p_[i-1]; }
	Profit profit(std::vector<bool> v) const;

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
