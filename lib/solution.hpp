#ifndef SOLUTION_HPP_JYHGMADO
#define SOLUTION_HPP_JYHGMADO

#include "instance.hpp"

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
    void set(ItemPos i, int b);
    int contains(ItemPos i) const;
    int contains_idx(ItemIdx i) const;
    void clear();

    /**
     * Replace the current solution by "sol" if it is better.
     */
    bool update(const Solution& sol);

    void update_from_binary(const BSolFactory& bsolf, BSol bsol);

    /**
     * Write the solution in the input file.
     * One item per line. 1 if in, 0 if out.
     */
    void write_cert(std::string file);

    std::string print_bin() const;

private:

    const Instance& instance_;
    ItemIdx k_ = 0;
    Profit  p_ = 0;
    Weight  w_ = 0;
    std::vector<int> x_;
};

std::ostream& operator<<(std::ostream &os, const Solution& solution);

#endif /* end of include guard: SOLUTION_HPP_JYHGMADO */
