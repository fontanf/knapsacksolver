#pragma once

#include "knapsack/lib/instance.hpp"

namespace knapsack
{

class PartSolFactory2
{

public:

    PartSolFactory2(ItemPos size): size_(size)
    {
        idx_ = std::vector<ItemPos>(size, -1);
    }

    PartSolFactory2(): PartSolFactory2(64) {  }

    ItemPos size() const { return size_; }

    const std::vector<ItemPos>& indices() const { return idx_; }

    int contains(PartSol2 s, ItemPos j) const
    {
        return ((s >> j) & 1UL);
    }

    PartSol2 add(const PartSol2& s) const
    {
        return (s | (1UL << cur_));
    }

    PartSol2 remove(const PartSol2& s) const
    {
        return (s & (~(1UL << cur_)));
    }

    PartSol2 toggle(PartSol2& s, ItemPos j) const
    {
        assert(0 <= j && j < size_);
        return (s ^ (1UL << j));
    }

    std::string print(PartSol2& s) const
    {
        std::string str = "";
        for (ItemPos j=0; j<size_; ++j)
            if (idx_[j] != -1)
                str += std::to_string(idx_[j]) + " " + std::to_string(contains(s, j)) + "; ";
        return str;
    }

    void add_item(ItemPos j)
    {
        cur_++;
        if (cur_ == size_)
            cur_ = 0;
        idx_[cur_] = j;
    }

private:

    ItemPos size_;
    std::vector<ItemPos> idx_;
    ItemPos cur_ = -1;

};

}

