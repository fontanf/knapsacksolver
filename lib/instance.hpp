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
typedef int_fast64_t ItemPos;
typedef int_fast64_t StateIdx;

class Solution;

struct Item
{
    Item() { }
    Item(ItemIdx i, Weight w, Profit p): i(i), w(w), p(p) { }
    Item& operator=(const Item& item)
    {
        if (this != &item) {
            i = item.i;
            w = item.w;
            p = item.p;
        }
        return *this;
    }
    ItemIdx i = -1;
    Weight  w = -1;
    Profit  p = -1;
};

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
    Instance(ItemIdx n, Weight c, std::vector<Item> items);

    /**
     * Create instance from file.
     */
    Instance(boost::filesystem::path filename);

    /*
     * Sort items according to non-increasing profit-to-weight ratio.
     */
    void sort();

    /**
     * Sort items partially according to non-increasing profit-to-weight
     * ratio, i.e. the break item is the same as if the items were fully
     * sorted. This is achieved with a quick-sort like algorithm as
     * described in "A Minimal Algorithm for the 0-1 Knapsack Problem"
     * (Pisinger, 1997).
     */
    void sort_partially();

    /**
     * Apply variable reduction. See "Knapsack Problem", Chap 3.2:
     * "Variable Reduction" (Kellerer, 2004) for more details.
     * The first one uses Dembo and Hammer UB and only requires a partial
     * sorting of the items.
     * The second one uses Dantzig UB and requires a complete sorting of the
     * items.
     * Fixed items are moved at the end of the item vector and the item number
     * and the capacity are updated. The solution with fixed variables is
     * returned by the method reduced_solution().
     */
    void reduce1(const Solution& sol_curr, bool verbose = false);
    void reduce2(const Solution& sol_curr, bool verbose = false);

    const Solution* reduced_solution() const { return sol_red_; }
    const Solution* optimal_solution() const { return sol_opt_; }
    Profit optimum() const;

    /**
     * Create an instance with capacitiy and weights divided, keeping the
     * floor (resp. the ceiling).
     */
    void divide_weights_floor(Weight divisor);
    void divide_weights_ceil(Weight divisor);
    void divide_profits_floor(Profit divisor);
    void divide_profits_ceil(Profit divisor);
    void surrogate(Weight multiplier, ItemIdx bound);

    ~Instance();


    /**
     * Getters
     */

    inline std::string name()   const { return name_; }
    inline std::string format() const { return format_; }
    std::string sort_type()     const { return sort_type_; }

    inline ItemIdx item_number() const { return n_; }
    inline Weight  capacity()    const { return c_; }
    inline ItemIdx total_item_number() const { return items_.size(); }
    inline Weight  total_capacity()    const { return c_orig_; }
    inline const Item& item(ItemIdx i) const { assert(i >= 0 && i < total_item_number()); return items_[i]; }

    ItemPos break_item()     const { return b_; }
    Profit  break_profit()   const { return psum_; };
    Weight  break_weight()   const { return wsum_; }
    Weight  break_capacity() const { return r_; }

    const Item& isum(ItemPos i) const { assert(sort_type_ == "eff"); return isum_[i]; }
    ItemPos ub_item(Item item) const;

    /**
     * Compute GCD of capacity and weights.
     */
    Weight gcd() const;

    /**
     * Return the profit of the certificate file.
     */
    Profit check(boost::filesystem::path cert_file);

    bool check_opt(Profit p) const;
    bool check_sopt(const Solution& sol) const;
    bool check_ub(Profit p) const;
    bool check_lb(Profit p) const;
    bool check_sol(const Solution& sol) const;

private:

    void read_standard(boost::filesystem::path filename);
    void read_pisinger(boost::filesystem::path filename);

    bool check();
    inline void swap(ItemPos i, ItemPos j) { Item tmp = items_[i]; items_[i] = items_[j]; items_[j] = tmp; };
    void swap(ItemPos i1, ItemPos i2, ItemPos i3, ItemPos i4);

    std::string name_;
    std::string format_;

    ItemIdx n_;
    Weight  c_;
    Weight  c_orig_;
    std::string sort_type_ = "";
    std::vector<Item> items_;

    Solution* sol_opt_;
    Solution* sol_red_ = NULL;
    ItemPos b_ = -1;
    Weight r_;
    Profit psum_ = 0;
    Weight wsum_ = 0;
    std::vector<Item> isum_;
};

std::ostream& operator<<(std::ostream &os, const Item& item);
std::ostream& operator<<(std::ostream &os, const Instance& instance);

#endif /* end of include guard: PARSER_HPP_CWOQFZGE */
