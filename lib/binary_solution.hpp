#pragma once

#include "instance.hpp"

class BSolFactory
{

public:

    BSolFactory(ItemPos size, ItemPos b, ItemPos f, ItemPos l): size_(size), b_(b), f_(f), l_(l)
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

    BSolFactory(ItemPos b, ItemPos f, ItemPos l): BSolFactory(64, b, f, l) {  }

    ItemPos x1() const { return x1_; }
    ItemPos x2() const { return x2_; }
    ItemPos size() const { return x2_ - x1_ + 1; }

    BSol bsol_break() const
    {
        BSol bsolbreak = 0UL;
        for (ItemPos i=x1_; i<b_; ++i)
            add(bsolbreak, i);
        return bsolbreak;
    }

    int contains(BSol s, ItemPos i) const
    {
        assert(x1_ <= i && i <= x2_);
        return ((s >> (i-x1_)) & 1UL);
    }

    BSol add(const BSol& s, ItemPos i) const
    {
        if (i < x1_ || x2_ < i)
            return s;
        assert(!contains(s,i));
        return (s | (1UL << (i-x1_)));
    }

    BSol remove(const BSol& s, ItemPos i) const
    {
        if (i < x1_ || x2_ < i)
            return s;
        assert(contains(s,i));
        return (s & (~(1UL << (i-x1_))));
    }

    BSol toggle(BSol& s, ItemPos i) const
    {
        if (i < x1_ || x2_ < i)
            return s;
        return (s ^ (1UL << (i-x1_)));
    }

    std::string print(BSol& s) const
    {
        std::string str = "";
        for (ItemPos i=x1_; i<=x2_; ++i)
            str += std::to_string(contains(s,i));
        return str;
    }

private:

    ItemPos size_;
    ItemPos b_;
    ItemPos f_;
    ItemPos l_;
    ItemPos x1_;
    ItemPos x2_;

};
