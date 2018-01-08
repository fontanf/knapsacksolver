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

typedef int_fast64_t Profit;
typedef int_fast64_t Weight;
typedef int_fast64_t ItemIdx;
typedef int_fast64_t ValIdx;

class Solution;

class Instance
{

public:

	/**
	 * Constructors
	 */

	/**
	 * Manual constructor.
	 * This constructor should only be used for tests.
	 */
	Instance(ItemIdx n, Weight c, std::vector<Profit> p, std::vector<Weight> w);

	/**
	 * Create instance from file.
	 */
	Instance(boost::filesystem::path filename);

	/**
	 * Copy constructor
	 */
	Instance(const Instance& instance);
	Instance& operator=(const Instance& instance);

	/**
	 * Create an instance with items ordered according to non-increasing
	 * profit.
	 */
	static Instance sort_by_profit(const Instance& instance);

	/**
	 * Create an instance with items ordered according to non-decreasing
	 * weight.
	 */
	static Instance sort_by_weight(const Instance& instance);

	/*
	 * Create an instance with items ordered according to non-increasing
	 * profit-to-weight ratio.
	 */
	static Instance sort_by_efficiency(const Instance& instance);

	/**
	 * Create an instance with items partially ordered according to
	 * non-increasing profit / non-decreasing weight / non-increasing
	 * profit-to-weight ratio, i.e. the break item is the same as if the items
	 * were fully sorted. This is achieved with a quick-sort like algorithm as
	 * described in "A Minimal Algorithm for the 0-1 Knapsack Problem"
	 * (Pisinger, 1997).
	 */
	static Instance sort_partially_by_profit(const Instance& instance, Profit lb);
	static Instance sort_partially_by_weight(const Instance& instance);
	static Instance sort_partially_by_efficiency(const Instance& instance);

	/**
	 * Create an instance with capacitiy and weights divided, keeping the
	 * floor (resp. the ceiling).
	 */
	static Instance divide_weights_floor(const Instance& instance, Weight divisor);
	static Instance divide_weights_ceil(const Instance& instance, Weight divisor);

	static Instance divide_profits_floor(const Instance& instance, Profit divisor);
	static Instance divide_profits_ceil(const Instance& instance, Profit divisor);

	static Instance from_to(const Instance& instance, ItemIdx i1, ItemIdx i2, Weight c);
	static Instance fix(const Instance& instance, ItemIdx j, Weight w);

	/**
	 * Create an instance applying variable reduction. See "Knapsack Problem",
	 * Chap 3.2: "Variable Reduction" (Pferschy, 2004) for more details.
	 */
	static Instance reduce(const Instance& instance, Profit lower_bound);

	/**
	 * Create an instance partially ordered according to non-increasing
	 * profit-to-weight ratio, with weights equal to wi + S (resp.  wi - S) and
	 * capacity c + Sk (resp. c - Sk). This is used for surrogate relaxation.
	 * See "Dynamic Programming and Strong Bounds for the 0-1
	 * Knapsack Problem" (Martello, 1999).
	 */
	void surrogate_plus(const Instance& instance, Weight multiplier, ItemIdx bound);
	void surrogate_minus(const Instance& instance, Weight multiplier, ItemIdx bound);

	static Instance child(const Instance& instance);

	~Instance();


	/**
	 * Getters
	 */

	inline ItemIdx item_number() const { return n_; }
	inline Weight  capacity()    const { return c_; }

	/**
	 * Items properties
	 */
	inline Weight weight(ItemIdx i) const { assert(i > 0 && i <= n_); return w_[i-1]; }
	inline Profit profit(ItemIdx i) const { assert(i > 0 && i <= n_); return p_[i-1]; }

	/**
	 * Return optimal value if known, 0 otherwise.
	 */
	inline Profit optimum()         const { return opt_; }

	/**
	 * Return true if item i is in the known solution, false otherwise.
	 */
	inline bool optimum(ItemIdx i) const { assert(i > 0 && i <= n_ && x_ != NULL); return x_[i-1]; }

	/**
	 * Index of item i in the parent solution.
	 */
	inline ItemIdx index(ItemIdx i) const { assert(i > 0 && i <= n_); return i_[i-1]; }

	/**
	 * Index of item i in the original instance.
	 */
	ItemIdx index_orig(ItemIdx i) const;

	ItemIdx max_weight_item() const;
	ItemIdx max_profit_item() const;

	inline std::string name()   const { return name_; }
	inline std::string format() const { return format_; }

	std::string sort_type() const { return sort_type_; }

	/**
	 * Parent instance.
	 */
	const Instance* parent() const { return parent_; }

	/**
	 * Original instance.
	 */
	const Instance* instance_orig() const;

	/**
	 * After a reduction, return the solution of the reduced variables.
	 */
	Solution* solution() const { return solution_; }

	/**
	 * Return the solution converted in the original instance.
	 */
	Solution solution_orig(const Solution& solution) const;

	/**
	 * After a reduction, return the profit from the original instance.
	 */
	Profit profit_orig() const;

	/**
	 * Compute GCD of capacity and weights.
	 */
	Weight gcd() const;

	/**
	 * Return the profit of the certificate file.
	 */
	Profit check(boost::filesystem::path cert_file);

	void info(std::ostream& os);

private:

	Instance() { };

	void read_standard(boost::filesystem::path filename);
	void read_pisinger(boost::filesystem::path filename);

	/**
	 * Create an instance with name_, format_, parent_ and solution_
	 * members initialized.
	 */
	void init(const Instance& instance);

	inline void set_profit(ItemIdx i, Profit p) { assert(i > 0 && i <= n_); p_[i-1] = p; }
	inline void set_weight(ItemIdx i, Weight w) { assert(i > 0 && i <= n_); w_[i-1] = w; }
	inline void set_index(ItemIdx i, ItemIdx j) { assert(i > 0 && i <= n_); i_[i-1] = j; }

	void set_profits_and_weights_from_parent();
	Profit profit_parent(ItemIdx i) const { return parent()->profit(index(i));}
	Weight weight_parent(ItemIdx i) const { return parent()->weight(index(i));}

	std::string name_;
	std::string format_;

	ItemIdx n_;
	Weight  c_;
	Profit  opt_ = 0;
	std::string sort_type_ = "";
	Weight* w_  = NULL;
	Profit* p_  = NULL;
	bool*   x_  = NULL;
	ItemIdx* i_ = NULL;
	const Instance* parent_   = NULL;
	Solution*       solution_ = NULL;

};

std::ostream& operator<<(std::ostream &os, const Instance& instance);

#endif /* end of include guard: PARSER_HPP_CWOQFZGE */
