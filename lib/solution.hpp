#pragma once

#include "knapsack/lib/instance.hpp"

namespace knapsack
{

class Solution
{

public:

    Solution(const Instance& instance);
    Solution(const Solution& solution);
    Solution& operator=(const Solution& solution);
    ~Solution() { };

    inline const Instance& instance()  const { return instance_; }
    inline Weight weight()             const { return w_; }
    inline Weight remaining_capacity() const { return instance_.total_capacity() - weight(); }
    inline Profit profit()             const { return p_; }
    inline ItemIdx item_number()       const { return k_; }
    const std::vector<int>& data()     const { return x_; }

    /**
     * Add/remove an item to/from the solution.
     * If the item is/isn't already in the solution, nothing happens.
     * Weight, Profit and Item number of the solution are updated.
     * WARNING: the input correspond to the position of the item in the
     * instance, not its ID!
     */
    void set(ItemPos j, int b);
    int contains(ItemPos j) const;
    int contains_idx(ItemIdx j) const;
    void clear();
    void resize(ItemIdx n) { x_.resize(n, 0); }

    void update_from_partsol(const PartSolFactory1& psolf, PartSol1 psol);
    void update_from_partsol(const PartSolFactory2& psolf, PartSol2 psol);

    /**
     * Write the solution in the input file.
     * One item per line. 1 if in, 0 if out.
     */
    void write_cert(std::string file);

    std::string to_string_binary() const;
    std::string to_string_binary_ordered() const;
    std::string to_string_items() const;

private:

    const Instance& instance_;
    ItemIdx k_ = 0;
    Profit  p_ = 0;
    Weight  w_ = 0;
    std::vector<int> x_;
};

std::ostream& operator<<(std::ostream &os, const Solution& solution);

void init_display(Profit lb, Profit ub, Info& info);
void update_sol(Solution& sol, Profit ub, const Solution& sol_new, const std::stringstream& s, Info& info);
void update_lb(Profit& lb, Profit ub, Profit lb_new, const std::stringstream& s, Info& info);
void update_ub(Profit lb, Profit& ub, Profit ub_new, const std::stringstream& s, Info& info);

}

