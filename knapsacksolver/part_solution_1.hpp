#pragma once

#include "knapsacksolver/solution.hpp"

namespace knapsacksolver
{

class PartSolFactory1
{

public:

    PartSolFactory1(const Instance& instance, ItemPos size, ItemPos b, ItemPos f, ItemPos l):
        instance_(instance), size_(size), b_(b), f_(f), l_(l)
    {
        if (l-f+1 < size) {
            x1_ = f;
            x2_ = l;
        } else if ((b-f+1) <= size/2) {
            x1_ = f;
            x2_ = f+size-1;
        } else if ((l-b+1) <= size/2) {
            x1_ = l-size+1;
            x2_ = l;
        } else {
            x1_ = b-size/2;
            x2_ = x1_+size-1;
        }
    }

    ItemPos x1() const { return x1_; }
    ItemPos x2() const { return x2_; }
    ItemPos size() const { return x2_ - x1_ + 1; }

    PartSol1 bsol_break() const
    {
        PartSol1 bsolbreak = 0UL;
        for (ItemPos j = x1_; j < b_; ++j)
            add(bsolbreak, j);
        return bsolbreak;
    }

    int contains(PartSol1 s, ItemPos j) const
    {
        assert(x1_ <= j && j <= x2_);
        return ((s >> (j-x1_)) & 1UL);
    }

    PartSol1 add(const PartSol1& s, ItemPos j) const
    {
        if (j < x1_ || x2_ < j)
            return s;
        assert(!contains(s,j));
        return (s | (1UL << (j-x1_)));
    }

    PartSol1 remove(const PartSol1& s, ItemPos j) const
    {
        if (j < x1_ || x2_ < j)
            return s;
        assert(contains(s,j));
        return (s & (~(1UL << (j-x1_))));
    }

    PartSol1 toggle(PartSol1& s, ItemPos j) const
    {
        if (j < x1_ || x2_ < j)
            return s;
        return (s ^ (1UL << (j-x1_)));
    }

    std::string print(PartSol1& s) const
    {
        std::string str = "";
        for (ItemPos j = x1_; j <= x2_; ++j)
            str += std::to_string(contains(s,j));
        return str;
    }

    std::vector<int> vector(PartSol1& s) const
    {
        ItemPos f = std::max(instance_.first_item(), x1());
        ItemPos l = std::min(instance_.last_item(), x2());
        std::vector<int> vec(instance_.number_of_items(), 0);
        for (ItemPos j = f; j <= l; ++j)
            vec[j] = (contains(s, j))? 1: -1;
        return vec;
    }

    void update_solution(PartSol1 psol, Solution& sol)
    {
        for (ItemPos j = x1(); j <= x2(); ++j)
            sol.set(j, contains(psol, j));
    }

private:

    const Instance& instance_;
    ItemPos size_;
    ItemPos b_;
    ItemPos f_;
    ItemPos l_;
    ItemPos x1_;
    ItemPos x2_;

};

}

