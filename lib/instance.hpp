#ifndef PARSER_HPP_CWOQFZGE
#define PARSER_HPP_CWOQFZGE

#include <cstdint>
#include <random>
#include <string>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <chrono>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/program_options.hpp>

typedef int_fast64_t Profit;
typedef int_fast64_t Weight;
typedef int_fast64_t ItemIdx;
typedef int_fast64_t ItemPos;
typedef int_fast64_t StateIdx;

class Solution;
typedef int64_t BSol;
class BSolFactory;

struct Info
{
    Info(): t1(std::chrono::high_resolution_clock::now()) {  }
    boost::property_tree::ptree pt;
    bool verbose_ = false;
    std::chrono::high_resolution_clock::time_point t1;

    void verbose(bool b) { verbose_ = b; }
    static bool verbose(const Info* info)
    {
        return (info != NULL && info->verbose_);
    }

    void write_ini(std::string file)
    {
        if (file != "")
            boost::property_tree::write_ini(file, pt);
    }

    double elapsed_time() const
    {
        std::chrono::high_resolution_clock::time_point t2
            = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> time_span
            = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
        return time_span.count();
    }
};

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

struct Interval
{
    ItemPos f;
    ItemPos l;
};

struct SortData {
    Interval ir;
    Interval il;
    Weight   w = 0;
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
    Instance(std::vector<Item> items, Weight c);

    /**
     * Create instance from file.
     */
    Instance(boost::filesystem::path filename);

    /**
     * Copy constructor
     */
    Instance(const Instance& ins);

    /**
     * Copy instance without reduced items
     */
    Instance(const Instance& ins, std::vector<Interval> v);

    /*
     * Sort items according to non-increasing profit-to-weight ratio.
     */
    void sort();
    bool sorted() const { return sorted_; }

    /**
     * Sort items partially according to non-increasing profit-to-weight
     * ratio, i.e. the break item is the same as if the items were fully
     * sorted. This is achieved with a quick-sort like algorithm as
     * described in "A Minimal Algorithm for the 0-1 Knapsack Problem"
     * (Pisinger, 1997).
     */
    void sort_partially();
    void set_first_last_item();
    void sort_right(Profit lb);
    void sort_left(Profit lb);
    ItemPos int_right_size() const { return int_right_.size(); }
    ItemPos int_left_size() const { return int_left_.size(); }
    bool break_item_found() const { return (b_ != -1); }

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
     * If true is returned, then sol_curr is optimal, else, sol_curr may or may
     * not be optimal: the optimal solution of the original instance is the best
     * one from sol_curr and the optimal solution of the reduced instance.
     */
    bool reduce1(Profit lb, bool verbose = false);
    bool reduce2(Profit lb, bool verbose = false);

    const Solution* reduced_solution() const { return sol_red_; }
    const Solution* optimal_solution() const { return sol_opt_; }
    const Solution* break_solution()   const { return sol_break_; }
    Profit optimum() const;

    /**
     * Reduce item f..j-1, and add them to the reduced solution
     */
    void set_first_item(ItemPos j);
    /**
     * Reduce items j+1..l (there are not added in the reduced solution)
     */
    void set_last_item(ItemPos j);

    void fix(BSolFactory bsolf, BSol bsol);

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

    inline ItemIdx item_number() const { return l_-f_+1; }
    inline ItemPos first_item()  const { return f_; }
    inline ItemPos last_item()   const { return l_; }
    inline ItemIdx total_item_number() const { return items_.size(); }
    inline Weight  total_capacity()    const { return c_orig_; }
    inline const Item& item(ItemIdx i) const { assert(i >= 0 && i < total_item_number()); return items_[i]; }
    Weight capacity() const;

    ItemPos break_item()     const { assert(b_ >= first_item() && b_ <= last_item() + 1); return b_; }
    Profit  break_profit()   const;
    Weight  break_weight()   const;
    Weight  break_capacity() const;

    const Item& max_weight_item()     const { return i_wmax_; }
    const Item& max_profit_item()     const { return i_pmax_; }
    const Item& max_efficiency_item() const { return i_emax_; }

    const Item& isum(ItemPos i) const { assert(sorted()); return isum_[i]; }
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

    /**
     * return "LB XXXX GAP XXXX" if optimal_solution() != NULL,
     *        "LB XXXX" otherwise.
     */
    std::string print_lb(Profit lb) const;
    std::string print_ub(Profit ub) const;
    std::string print_opt(Profit opt) const;

private:

    void read_standard(boost::filesystem::path filename);
    void read_pisinger(boost::filesystem::path filename);

    ItemPos partition(ItemPos f, ItemPos l);
    bool check();
    inline void swap(ItemPos i, ItemPos j) { Item tmp = items_[i]; items_[i] = items_[j]; items_[j] = tmp; };
    void swap(ItemPos i1, ItemPos i2, ItemPos i3, ItemPos i4);
    void update_isum();
    void compute_break_item();
    void compute_max_items();
    /*
     * Remove items which weight is greater than the updated capacity
     */
    void remove_big_items();

    std::string name_;
    std::string format_;

    ItemPos f_;
    ItemPos l_;
    Weight  c_orig_;
    bool sorted_ = false;
    std::vector<Interval> int_right_;
    std::vector<Interval> int_left_;
    std::vector<Item> items_;

    Solution* sol_opt_   = NULL; // Optimal solution
    Solution* sol_red_   = NULL; // Reduced solution
    Solution* sol_break_ = NULL; // Break solution
    ItemPos b_ = -1; // Break item
    Item i_wmax_ = {-1, -1, -1}; // Max weight item
    Item i_wmin_ = {-1, c_orig_+1, -1}; // Min weight item
    Item i_pmax_ = {-1, -1, -1}; // Max profit item
    Item i_emax_ = {-1, 0, -1};  // Max efficiency item;
    std::vector<Item> isum_;
    bool sol_red_opt_ = false;
};

std::ostream& operator<<(std::ostream &os, const Item& item);
std::ostream& operator<<(std::ostream &os, const Interval& interval);
std::ostream& operator<<(std::ostream &os, const Instance& instance);

#endif /* end of include guard: PARSER_HPP_CWOQFZGE */
