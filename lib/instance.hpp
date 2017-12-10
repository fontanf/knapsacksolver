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
typedef  int_fast64_t ProWei;
typedef size_t ItemIdx;
typedef size_t ValIdx;

class Solution;

class Instance
{

public:

	Instance(boost::filesystem::path filename);
	void read_standard(boost::filesystem::path filename);
	void read_pisinger(boost::filesystem::path filename);

	Instance() { };

	/**
	 * Copy constructor
	 */
	Instance(const Instance& instance);
	Instance& operator=(const Instance& instance);

	/**
	 * Create an instance with name_, format_, parent_ and solution_
	 * members initialized.
	 */
	void init(const Instance& instance);

	static Instance child(const Instance& instance);

	/*
	 * Create an instance with sorted items.
	 */
	static Instance sort_by_profit(const Instance& instance);
	static Instance sort_by_weight(const Instance& instance);
	static Instance sort_by_density(const Instance& instance);

	static Instance divide_floor(const Instance& instance, Weight divisor);
	static Instance divide_ceil(const Instance& instance, Weight divisor);

	static Instance reduce(const Instance& instance, Profit lower_bound);

	void surrogate_plus(const Instance& instance, Weight multiplier, ItemIdx bound);
	void surrogate_minus(const Instance& instance, Weight multiplier, ItemIdx bound);

	static Instance remove_first_item(const Instance& instance);

	/**
	 * Setters
	 */
	inline void add_name(std::string name) { name_ += name; }
	inline void set_item_number(ItemIdx n) { n_ = n; }
	inline void set_optimum(Profit opt) { opt_ = opt; }
	inline const Instance& set_capacity(Weight c) { c_ = c; return *this; }
	inline void set_profit(ItemIdx i, Profit p) { assert(i > 0 && i <= n_); p_[i-1] = p; }
	inline void set_weight(ItemIdx i, Weight w) { assert(i > 0 && i <= n_); w_[i-1] = w; }
	inline void set_index(ItemIdx i, ItemIdx j) { assert(i > 0 && i <= n_); i_[i-1] = j; }

	~Instance();

	/**
	 * Getters
	 */
	inline std::string name()       const { return name_; }
	inline std::string format()     const { return format_; }
	Solution*       solution()      const { return solution_; }
	const Instance* parent()        const { return parent_; }
	inline ItemIdx item_number()    const { return n_; }
	inline Weight capacity()        const { return c_; }
	inline Profit optimum()         const { return opt_; }
	inline bool  optimum(ItemIdx i) const { assert(i > 0 && i <= n_ && x_ != NULL); return x_[i-1]; }
	inline Weight weight(ItemIdx i) const { assert(i > 0 && i <= n_); return w_[i-1]; }
	inline Profit profit(ItemIdx i) const { assert(i > 0 && i <= n_); return p_[i-1]; }
	inline ItemIdx index(ItemIdx i) const { assert(i > 0 && i <= n_); return i_[i-1]; }

	Weight weight_max() const;
	Profit profit_max() const;
	Weight gcd()        const;

	const Instance* instance_orig()                         const;
	ItemIdx            index_orig(ItemIdx i)                const;
	Solution        solution_orig(const Solution& solution) const;
	Profit            profit_orig()                         const;

	Profit check(boost::filesystem::path cert_file);
	void info(std::ostream& os);

private:

	void set_profits_and_weights_from_parent();
	Profit profit_parent(ItemIdx i) const { return parent()->profit(index(i));}
	Weight weight_parent(ItemIdx i) const { return parent()->weight(index(i));}

	std::string name_;
	std::string format_;

	ItemIdx n_;
	Weight  c_;
	Profit  opt_ = 0;
	Weight* w_ = NULL;
	Profit* p_ = NULL;
	bool*   x_ = NULL;
	std::vector<ItemIdx> i_;
	const Instance* parent_   = NULL;
	Solution*       solution_ = NULL;

};

std::ostream& operator<<(std::ostream &os, const Instance& instance);

#endif /* end of include guard: PARSER_HPP_CWOQFZGE */
