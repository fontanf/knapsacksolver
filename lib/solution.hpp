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

    inline const Instance& instance() const { return instance_; }
    inline Weight weight()            const { return w_; }
    inline Profit profit()            const { return p_; }
    inline ItemIdx item_number()      const { return k_; }
    const std::vector<int>& data()   const { return x_; }

    void set(ItemPos i, int b);
    int contains(ItemPos i) const;

    bool update(const Solution& sol);

    void write_cert(std::string file);

private:

    const Instance& instance_;
    ItemIdx k_ = 0;
    Profit  p_ = 0;
    Weight  w_ = 0;
    std::vector<int> x_;
};

std::ostream& operator<<(std::ostream &os, const Solution& solution);

#endif /* end of include guard: SOLUTION_HPP_JYHGMADO */
