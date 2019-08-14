#pragma once

#include "benchtools/info.hpp"
#include "benchtools/tools.hpp"

#include <cstdint>
#include <random>
#include <string>
#include <algorithm>
#include <iostream>
#include <fstream>

namespace knapsack
{

typedef int64_t Profit;
typedef int64_t Weight;
typedef int64_t ItemIdx;
typedef int64_t ItemPos;
typedef int64_t StateIdx;
typedef int64_t Cpt;
typedef double Effciency;

class Solution;
typedef int64_t PartSol1;
typedef int64_t PartSol2;
class PartSolFactory1;
class PartSolFactory2;

struct Item
{
    Item(ItemIdx j, Weight w, Profit p): j(j), w(w), p(p) { }
    ItemIdx j = -1; // Index, 0 <= j < n, must be different for each item
    Weight  w = -1; // Weight, w >= 0
    Profit  p = -1; // Profit, p >= 0
    Effciency efficiency() const { return (double)p/(double)w; }
};

struct Interval
{
    ItemPos f;
    ItemPos l;
};

class Instance
{

public:

    /**
     * Constructors and destructor
     */

    Instance(std::string filepath, std::string format);

    Instance();
    Instance(ItemIdx n, Weight c);
    ItemIdx add_item(Weight w, Profit p);
    void add_items(const std::vector<std::pair<Weight, Profit>>& wp);
    void set_capacity(Weight c) { c_orig_ = c; }

    Instance(const Instance& ins);

    void set_optimal_solution(Solution& sol);
    static Instance reset(const Instance& ins);

    ~Instance();

    /**
     * Getters
     */

    inline ItemIdx total_item_number() const { return items_.size(); }
    inline Weight  total_capacity()    const { return c_orig_; }
    inline const Item& item(ItemIdx j) const { assert(j >= 0 && j < total_item_number()); return items_[j]; }

    const Solution* optimal_solution() const { return sol_opt_.get(); }
    Profit optimum() const;

    ItemPos max_efficiency_item(Info& info) const;
    ItemPos before_break_item(Info& info) const;
    ItemPos max_weight_item(Info& info) const;
    ItemPos min_weight_item(Info& info) const;
    ItemPos max_profit_item(Info& info) const;
    ItemPos min_profit_item(Info& info) const;
    /*
     * Item of highest profit that can be added to the break solution once item
     * b - 1 has been removed.
     */
    ItemPos gamma1(Info& info) const;
    /*
     * Item of lowest profit which has to be removed from the break solution so
     * that item b can be added.
     */
    ItemPos gamma2(Info& info) const;
    /*
     * Item of highest profit that can be added to the break solution.
     */
    ItemPos beta1(Info& info) const;
    /*
     * Item of lowest profit which has to be removed from the break solution so
     * that item b and b + 1 can be added.
     */
    ItemPos beta2(Info& info) const;
    std::vector<Weight> min_weights() const;

    /*
     * Sort items according to non-increasing profit-to-weight ratio.
     */
    void sort(Info& info);
    int sort_type() const { return sort_type_; }
    void set_sort_type(int type) { sort_type_ = type; }

    /**
     * Sort items partially according to non-increasing profit-to-weight
     * ratio, i.e. the break item is the same as if the items were fully
     * sorted. This is achieved with a quick-sort like algorithm as
     * described in "A Minimal Algorithm for the 0-1 Knapsack Problem"
     * (Pisinger, 1997).
     */
    void sort_partially(Info& info, ItemIdx limit=4);

    const std::vector<Interval>& int_right() const { return int_right_; }
    const std::vector<Interval>& int_left()  const { return int_left_; }
    ItemPos s_init() const { return s_init_; }
    ItemPos t_init() const { return t_init_; }
    ItemPos s_prime() const { return s_prime_; }
    ItemPos t_prime() const { return t_prime_; }
    ItemPos s_second() const { return (int_left().empty())? 0: int_left().back().l + 1; }
    ItemPos t_second() const { return (int_right().empty())? total_item_number() - 1: int_right().back().f - 1; }
    ItemPos bound_item_left(ItemPos s, Profit lb, Info& info);
    ItemPos bound_item_right(ItemPos t, Profit lb, Info& info);
    /**
     * Compute improved initial core. See "Dynamic Programming and Strong
     * Bounds for the 0-1 Knapsack Problem", 3. The Initial Core (Martello,
     * 1999).
     */
    void init_combo_core(Info& info);

    /*
     * Move item j to the initial core and carefully update int_right_ and
     * int_left_.
     */
    void add_item_to_core(ItemPos s, ItemPos t, ItemPos j, Info& info);

    /**
     * Apply variable reduction. See "Knapsack Problem", Chap 3.2:
     * "Variable Reduction" (Kellerer, 2004) for more details.
     * The first one uses Dembo and Hammer UB and only requires a partial
     * sorting of the items.
     * The second one uses Dantzig UB and requires a complete sorting of the
     * items.
     * Fixed items are moved at the beginning or at the end of the item vector
     * and the item number and the capacity are updated.
     * Note that the break item may change.
     * The solution with fixed variables is returned by the method
     * reduced_solution().
     * After a variable reduction, if the capacity of the instance is negative,
     * then the lower bound is optimal. Otherwise, the optimal value is the max
     * between the lower bound and the optimal value of the reduced solution.
     */
    void reduce1(Profit lb, Info& info);
    void reduce2(Profit lb, Info& info);
    const Solution* reduced_solution() const { return sol_red_.get(); }

    inline ItemIdx item_number() const { return l_-f_+1; }
    inline ItemPos first_item()  const { return f_; }
    inline ItemPos last_item()   const { return l_; }
    Weight capacity() const;

    /**
     * Reduce item f..j-1, and add them to the reduced solution
     */
    void set_first_item(ItemPos k, Info& info);
    /**
     * Reduce items j+1..l (there are not added in the reduced solution)
     */
    void set_last_item(ItemPos k);

    void fix(Info& info, const std::vector<int> vec);

    /**
     * Create an instance with capacitiy and weights divided, keeping the
     * floor (resp. the ceiling).
     */
    void divide_weights_floor(Weight divisor);
    void divide_weights_ceil(Weight divisor);
    void divide_profits_floor(Profit divisor);
    void divide_profits_ceil(Profit divisor);

    void surrogate(Info& info, Weight multiplier, ItemIdx bound, ItemPos first);
    void surrogate(Info& info, Weight multiplier, ItemIdx bound);

    const Solution* break_solution() const { return sol_break_.get(); }
    ItemPos break_item()     const { return b_; }
    Profit  break_profit()   const;
    Weight  break_weight()   const;
    Weight  break_capacity() const;

    ItemPos ub_item(Item item) const;

    void plot(std::string filepath);
    void write(std::string filepath);
    void plot_reduced(std::string filepath);
    void write_reduced(std::string filepath);

    /**
     * Return the profit of the certificate file.
     */
    Profit check(std::string cert_file);

    /**
     * return "LB XXXX GAP XXXX" if optimal_solution() != NULL,
     *        "LB XXXX" otherwise.
     */
    std::string print_lb(Profit lb) const;
    std::string print_ub(Profit ub) const;
    std::string print_opt(Profit opt) const;

private:

    /*
     * Methods
     */

    void read_standard(std::ifstream& file);
    void read_subsetsum_standard(std::ifstream& file);

    std::pair<ItemPos, ItemPos> partition(ItemPos f, ItemPos l, Info& info);
    bool check();
    bool check_partialsort(Info& info) const;

    inline void swap(ItemPos j, ItemPos k) { Item tmp = items_[j]; items_[j] = items_[k]; items_[k] = tmp; };

    std::vector<Item> get_isum() const;
    ItemPos ub_item(const std::vector<Item>& isum, Item item) const;
    void compute_break_item(Info& info);
    /*
     * Remove items which weight is greater than the updated capacity
     */
    void remove_big_items(Info& info);

    void sort_right(Info& info, Profit lb);
    void sort_left(Info& info, Profit lb);


    /*
     * Attributes
     */

    std::vector<Item> items_;
    Weight c_orig_;
    std::unique_ptr<Solution> sol_opt_; // Optimal solution

    ItemPos b_ = -1; // Break item

    // First and last items. Items moved before f_ or after l_ have their value
    // fixed in the reduced solution.
    ItemPos f_ = -1;
    ItemPos l_ = -1;

    // Initial core.
    ItemPos s_init_ = -1;
    ItemPos t_init_ = -1;

    ItemPos s_prime_ = -1;
    ItemIdx t_prime_ = -1;

    /**
     * 0: not sorted
     * 1: partially sorted
     * 2: fully sorted
     */
    int sort_type_ = 0;

    std::vector<Interval> int_right_;
    std::vector<Interval> int_left_;

    std::unique_ptr<Solution> sol_red_; // Reduced solution
    std::unique_ptr<Solution> sol_break_; // Break solution

};

std::ostream& operator<<(std::ostream &os, const Item& item);
std::ostream& operator<<(std::ostream &os, const Interval& interval);
std::ostream& operator<<(std::ostream &os, const Instance& instance);

Solution algorithm_end(const Solution& sol, Info& info);
Profit   algorithm_end(Profit val, Info& info);

}

