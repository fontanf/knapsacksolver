#pragma once

#include "knapsacksolver/solution.hpp"

namespace knapsacksolver
{

class PartSolFactory2
{

public:

    PartSolFactory2(const Instance& instance, ItemPos size): instance_(instance), size_(size)
    {
        idx_ = std::vector<ItemPos>(size, -1);
    }

    void reset()
    {
        std::fill(idx_.begin(), idx_.end(), -1);
    }

    PartSolFactory2(Instance& instance): PartSolFactory2(instance, 64) {  }

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
        for (ItemPos j = 0; j < size_; ++j) {
            if (str != "")
                str += " ";
            if (idx_[j] != -1)
                str += std::to_string(idx_[j]) + " " + std::to_string(contains(s, j)) + ";";
        }
        return str;
    }

    std::string print() const
    {
        std::string str = "";
        for (ItemPos j = 0; j < size_; ++j) {
            if (str != "")
                str += " ";
            str += std::to_string(idx_[j]);
        }
        return str;
    }

    void add_item(ItemPos j)
    {
        cur_++;
        if (cur_ == size_)
            cur_ = 0;
        idx_[cur_] = j;
    }

    std::vector<int> vector(PartSol2& s) const
    {
        std::vector<int> vec(instance_.number_of_items(), 0);
        for (ItemPos j = 0; j < size(); ++j) {
            ItemPos idx = indices()[j];
            if (idx == -1)
                continue;
            vec[indices()[j]] = (contains(s, j))? 1: -1;
        }
        return vec;
    }

    void update_solution(PartSol2 psol, Solution& sol)
    {
        for (ItemPos j = 0; j < size(); ++j)
            sol.set(indices()[j], contains(psol, j));
    }

private:

    const Instance& instance_;
    ItemPos size_;
    std::vector<ItemPos> idx_;
    ItemPos cur_ = -1;

};

}

