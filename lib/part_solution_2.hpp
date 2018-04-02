#pragma once

#include "instance.hpp"

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

    int contains(PartSol2 s, ItemPos i) const
    {
        return ((s >> i) & 1UL);
    }

    PartSol2 add(const PartSol2& s) const
    {
        return (s | (1UL << cur_));
    }

    PartSol2 remove(const PartSol2& s) const
    {
        return (s & (~(1UL << cur_)));
    }

    PartSol2 toggle(PartSol2& s, ItemPos i) const
    {
        assert(0 <= i && i < size_);
        return (s ^ (1UL << i));
    }

    std::string print(PartSol2& s) const
    {
        std::string str = "";
        for (ItemPos i=0; i<size_; ++i)
            if (idx_[i] != -1)
                str += std::to_string(idx_[i]) + " " + std::to_string(contains(s, i)) + "; ";
        return str;
    }

    void add_item(ItemPos i)
    {
        cur_++;
        if (cur_ == size_)
            cur_ = 0;
        idx_[cur_] = i;
    }

private:

    ItemPos size_;
    std::vector<ItemPos> idx_;
    ItemPos cur_ = -1;

};
