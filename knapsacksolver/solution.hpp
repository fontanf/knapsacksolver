#pragma once

#include "knapsacksolver/instance.hpp"

namespace knapsacksolver
{

class Solution
{

public:

    /** Create an empty solution. */
    Solution(const Instance& instance);
    /** Create a solution from a certificate file. */
    Solution(const Instance& instance, std::string certificate_path);
    /** Copy constructor. */
    Solution(const Solution& solution);
    /** Copy assignment operator. */
    Solution& operator=(const Solution& solution);
    /** Destructor. */
    ~Solution() { }

    /** Get the instance of the solution. */
    inline const Instance& instance() const { return instance_; }
    /** Get the weight of the solution. */
    inline Weight weight() const { return weight_; }
    /** Get the remaining capacity of the solution. */
    inline Weight remaining_capacity() const { return instance_.capacity() - weight(); }
    /** Get the profit of the solution. */
    inline Profit profit() const { return profit_; }
    /** Get the number of items in the solution. */
    inline ItemIdx number_of_items() const { return number_of_items_; }
    /**
     * Get the solution vector 'x'.
     *
     * 'x[j] == true' iff item 'j' is in the solution.
     */
    const std::vector<int>& data() const { return x_; }
    /** Return 'true' iff the solution is feasible. */
    inline bool feasible() const { return weight_ <= instance_.capacity(); }

    /**
     * Add/remove an item to/from the solution.
     *
     * If the item is/isn't already in the solution, nothing happens.
     * Weight, Profit and Item number of the solution are updated.
     *
     * WARNING: the input correspond to the position of the item in the
     * instance, not its ID!
     */
    void set(ItemPos j, int b);
    /** Return 'true' iff the solution contains the item at position 'j'. */
    int contains(ItemPos j) const;
    /** Return 'true' iff the solution contains item 'j'. */
    int contains_idx(ItemIdx j) const;
    /** Clear the solution. */
    void clear();

    void update_from_partsol(const PartSolFactory1& psolf, PartSol1 psol);
    void update_from_partsol(const PartSolFactory2& psolf, PartSol2 psol);

    /**
     * Write the solution in the input file.
     * One item per line. 1 if in, 0 if out.
     */
    void write(std::string certificate_path);

    std::string to_string_binary() const;
    std::string to_string_binary_ordered() const;
    std::string to_string_items() const;

private:

    /** Instance. */
    const Instance& instance_;
    /** Number of items in the solution. */
    ItemIdx number_of_items_ = 0;
    /** Profit of the solution. */
    Profit profit_ = 0;
    /** Weight of the solution. */
    Weight weight_ = 0;
    /**
     * Vector of the solution.
     *
     * 'x_[j] == true' iff the solution contains item 'j'.
     */
    std::vector<int> x_;

};

std::ostream& operator<<(std::ostream &os, const Solution& solution);

////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////// Output ////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

struct Output
{
    Output(const Instance& instance, Info& info);
    Solution solution;
    Profit lower_bound = 0;
    Profit upper_bound = -1;

    void print(Info& info, const std::stringstream& s) const;

    void update_lower_bound(
            Profit lower_bound_new,
            const std::stringstream& s,
            Info& info);

    void update_solution(
            const Solution& solution_new,
            const std::stringstream& s,
            Info& info);

    void update_upper_bound(
            Profit upper_bound_new,
            const std::stringstream& s,
            Info& info);

    Output& algorithm_end(Info& info);
};

Profit algorithm_end(Profit upper_bound, Info& info);

}

