#pragma once

#define FFOT_COMMA ,

#include "optimizationtools/utils/info.hpp"

#include <cstdint>
#include <random>
#include <string>
#include <algorithm>
#include <iostream>
#include <fstream>

namespace knapsacksolver
{

using optimizationtools::Info;

typedef int64_t Profit;
typedef int64_t Weight;
typedef int64_t ItemIdx;
typedef int64_t ItemPos;
typedef int64_t StateIdx;
typedef int64_t Counter;
typedef double Effciency;

class Solution;
typedef int64_t PartSol1;
typedef int64_t PartSol2;
class PartSolFactory1;
class PartSolFactory2;

struct Item
{
    /** Unique id of the item. */
    ItemIdx j = -1;

    /** Weight of the item, w >= 0. */
    Weight w = -1;

    /** Profit of the item, p >= 0 */
    Profit p = -1;

    /** Compute the efficiency of the item. */
    Effciency efficiency() const { return (double)p / (double)w; }
};

struct Interval
{
    ItemPos f;
    ItemPos l;
};

/**
 * Instance class for a Knapsack Problem.
 */
class Instance
{

public:

    /*
     * Constructors and destructor
     */

    /** Create instance from file. */
    Instance(std::string instance_path, std::string format);

    /** Manual constructor. */
    Instance();
    /** Add an item to the knapsack. */
    void add_item(Weight weight, Profit progit);
    /** Set the capacity of the knapsack. */
    void set_capacity(Weight capacity) { capacity_ = capacity; }
    /** Clear the instance. */
    void clear();

    /** Constructor for test instances. */
    Instance(Weight c, const std::vector<std::pair<Weight, Profit>>& wp);
    /** Set the optimal solution of the instance. */
    void set_optimal_solution(Solution& sol);

    /** Copy constructor. */
    Instance(const Instance& instance);
    /** Copy assignment operator. */
    Instance& operator=(const Instance& instance);
    /** Destructor. */
    ~Instance();

    /** Copy constructor without algorithmic informations. */
    static Instance reset(const Instance& instance);

    /*
     * Getters
     */

    /** Get the number of items in the instance. */
    inline ItemIdx number_of_items() const { return items_.size(); }
    /** Get the capacity of the instance. */
    inline Weight capacity() const { return capacity_; }
    /** Get an item. */
    inline const Item& item(ItemIdx j) const { assert(j >= 0 && j < number_of_items()); return items_[j]; }
    inline std::string path() const { return path_; }

    const Solution* optimal_solution() const { return optimal_solution_.get(); }
    Profit optimum() const;

    ItemPos max_efficiency_item(FFOT_DBG(Info& info)) const;
    ItemPos before_break_item(FFOT_DBG(Info& info)) const;
    ItemPos max_weight_item(FFOT_DBG(Info& info)) const;
    ItemPos min_weight_item(FFOT_DBG(Info& info)) const;
    ItemPos max_profit_item(FFOT_DBG(Info& info)) const;
    ItemPos min_profit_item(FFOT_DBG(Info& info)) const;
    /**
     * Item of highest profit that can be added to the break solution once item
     * b - 1 has been removed.
     */
    ItemPos gamma1(FFOT_DBG(Info& info)) const;
    /**
     * Item of lowest profit which has to be removed from the break solution so
     * that item b can be added.
     */
    ItemPos gamma2(FFOT_DBG(Info& info)) const;
    /** Item of highest profit that can be added to the break solution.  */
    ItemPos beta1(FFOT_DBG(Info& info)) const;
    /**
     * Item of lowest profit which has to be removed from the break solution so
     * that item b and b + 1 can be added.
     */
    ItemPos beta2(FFOT_DBG(Info& info)) const;
    std::vector<Weight> min_weights() const;

    /** Sort items according to non-increasing profit-to-weight ratio.  */
    void sort(FFOT_DBG(Info& info));
    /** Get the sort status of the instance. */
    int sort_status() const { return sort_status_; }
    /** Set the sort status of the instance. */
    void set_sort_status(int type) { sort_status_ = type; }

    /**
     * Sort items partially according to non-increasing profit-to-weight
     * ratio, i.e. the break item is the same as if the items were fully
     * sorted. This is achieved with a quick-sort like algorithm as
     * described in "A Minimal Algorithm for the 0-1 Knapsack Problem"
     * (Pisinger, 1997).
     */
    void sort_partially(FFOT_DBG(Info& info FFOT_COMMA) ItemIdx limit = 4);

    const std::vector<Interval>& int_right() const { return int_right_; }
    const std::vector<Interval>& int_left()  const { return int_left_; }
    ItemPos s_init() const { return s_init_; }
    ItemPos t_init() const { return t_init_; }
    ItemPos s_prime() const { return s_prime_; }
    ItemPos t_prime() const { return t_prime_; }
    ItemPos s_second() const { return (int_left().empty())? 0: int_left().back().l + 1; }
    ItemPos t_second() const { return (int_right().empty())? number_of_items() - 1: int_right().back().f - 1; }
    ItemPos bound_item_left(ItemPos s, Profit lb FFOT_DBG(FFOT_COMMA Info& info));
    ItemPos bound_item_right(ItemPos t, Profit lb FFOT_DBG(FFOT_COMMA Info& info));
    /**
     * Compute improved initial core. See "Dynamic Programming and Strong
     * Bounds for the 0-1 Knapsack Problem", 3. The Initial Core (Martello,
     * 1999).
     */
    void init_combo_core(FFOT_DBG(Info& info));

    /*
     * Move item j to the initial core and carefully update int_right_ and
     * int_left_.
     */
    void add_item_to_core(ItemPos s, ItemPos t, ItemPos j FFOT_DBG(FFOT_COMMA Info& info));

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
    const Solution* reduced_solution() const { return reduced_solution_.get(); }

    inline ItemIdx reduced_number_of_items() const { return l_-f_+1; }
    inline ItemPos first_item() const { return f_; }
    inline ItemPos last_item() const { return l_; }
    Weight reduced_capacity() const;

    /** Reduce item f..j-1, and add them to the reduced solution */
    void set_first_item(ItemPos k FFOT_DBG(FFOT_COMMA Info& info));
    /** Reduce items j+1..l (there are not added in the reduced solution) */
    void set_last_item(ItemPos k);

    void fix(const std::vector<int> vec FFOT_DBG(FFOT_COMMA Info& info));

    void surrogate(Weight multiplier, ItemIdx bound, ItemPos first FFOT_DBG(FFOT_COMMA Info& info));
    void surrogate(Weight multiplier, ItemIdx bound FFOT_DBG(FFOT_COMMA Info& info));

    /** Get the break solution of the instance. */
    const Solution* break_solution() const { return break_solution_.get(); }
    /** Get the position of the break item of the instance. */
    ItemPos break_item() const { return b_; }
    /** Get the break profit of the instance. */
    Profit break_profit() const;
    /** Get the break weight of the instance. */
    Weight break_weight() const;
    /** Get the break capacity of the instance. */
    Weight break_capacity() const;

    ItemPos ub_item(Item item) const;

    void plot(std::string output_path);
    void write(std::string instance_path);
    void plot_reduced(std::string output_path);
    void write_reduced(std::string instance_path);

private:

    /*
     * Private methods.
     */

    void read_standard(std::ifstream& file);
    void read_pisinger(std::ifstream& file);
    void read_jooken(std::ifstream& file);
    void read_subsetsum_standard(std::ifstream& file);

    std::pair<ItemPos, ItemPos> partition(ItemPos f, ItemPos l FFOT_DBG(FFOT_COMMA Info& info));
    bool check();
    bool check_partialsort(FFOT_DBG(Info& info)) const;

    inline void swap(ItemPos j, ItemPos k) { Item tmp = items_[j]; items_[j] = items_[k]; items_[k] = tmp; };

    std::vector<Item> get_isum() const;
    ItemPos ub_item(const std::vector<Item>& isum, Item item) const;
    void compute_break_item(FFOT_DBG(Info& info));
    /** Remove items which weight is greater than the updated capacity */
    void remove_big_items(FFOT_DBG(Info& info));

    void sort_right(Profit lb FFOT_DBG(FFOT_COMMA Info& info));
    void sort_left(Profit lb FFOT_DBG(FFOT_COMMA Info& info));


    /*
     * Private attributes
     */

    /** Path of the instance. */
    std::string path_ = "";

    /** Items. */
    std::vector<Item> items_;
    /** Capacity of the knapsack. */
    Weight capacity_;
    /** Optimal solution. */
    std::unique_ptr<Solution> optimal_solution_;

    /** Break item. */
    ItemPos b_ = -1;

    /**
     * Position of the first item.
     *
     * Items moved before this position have their value fixed in the reduced
     * solution.
     */
    ItemPos f_ = -1;
    /**
     * Position of the last item.
     *
     * Items moved after this position have their value fixed in the reduced
     * solution.
     */
    ItemPos l_ = -1;

    /** Initial core. */
    ItemPos s_init_ = -1;
    ItemPos t_init_ = -1;

    ItemPos s_prime_ = -1;
    ItemIdx t_prime_ = -1;

    /**
     * Sort status of the instance:
     * - 0: not sorted
     * - 1: partially sorted
     * - 2: fully sorted
     */
    int sort_status_ = 0;

    std::vector<Interval> int_right_;
    std::vector<Interval> int_left_;

    /** Reduced solution. */
    std::unique_ptr<Solution> reduced_solution_;
    /** Break solution. */
    std::unique_ptr<Solution> break_solution_;

};

std::ostream& operator<<(std::ostream &os, const Item& item);
std::ostream& operator<<(std::ostream &os, const Interval& interval);
std::ostream& operator<<(std::ostream &os, const Instance& instance);

void init_display(
        const Instance& instance,
        optimizationtools::Info& info);

}

