#include "instance.hpp"
#include "solution.hpp"

#include <sstream>

Instance::Instance(ItemIdx n, Weight c, std::vector<Item> items):
    name_(""), format_(""), n_(n), c_(c), c_orig_(c)
{
    items_.reserve(n);
    sol_opt_ = new Solution(*this);
    sol_red_ = new Solution(*this);
    for (ItemPos i=0; i<n_; ++i)
        items_[i] = items[i];
}

Instance::Instance(boost::filesystem::path filepath)
{
    if (!boost::filesystem::exists(filepath)) {
        std::cout << filepath << ": file not found." << std::endl;
        assert(false);
    }

    boost::filesystem::path FORMAT = filepath.parent_path() / "FORMAT.txt";
    if (!boost::filesystem::exists(FORMAT)) {
        std::cout << FORMAT << ": file not found." << std::endl;
        assert(false);
    }

    boost::filesystem::fstream file(FORMAT, std::ios_base::in);
    std::getline(file, format_);
    if        (format_ == "knapsack_standard") {
        read_standard(filepath);
    } else if (format_ == "knapsack_pisinger") {
        read_pisinger(filepath);
    } else {
        std::cout << format_ << ": Unknown instance format." << std::endl;
        assert(false);
    }

    sol_red_ = new Solution(*this);
    assert(check());
}

void Instance::read_standard(boost::filesystem::path filepath)
{
    name_ = filepath.stem().string();
    boost::filesystem::fstream file(filepath, std::ios_base::in);
    file >> n_ >> c_;
    c_orig_ = c_;
    items_.resize(n_);
    sol_opt_ = new Solution(*this);
    ItemIdx id;
    Profit p;
    Weight w;
    int    x;
    for (ItemPos i=0; i<n_; ++i) {
        file >> id >> p >> w >> x;
        items_[i] = {id,w,p};
        if (x == 1)
            sol_opt_->set(id, true);
    }

    file.close();
}

void Instance::read_pisinger(boost::filesystem::path filepath)
{
    boost::filesystem::fstream file(filepath, std::ios_base::in);
    uint_fast64_t null;

    std::getline(file, name_);

    std::string line;
    std::istringstream iss;

    std::getline(file, line, ' ');
    std::getline(file, line);
    std::istringstream(line) >> n_;

    std::getline(file, line, ' ');
    std::getline(file, line);
    std::istringstream(line) >> c_;
    c_orig_ = c_;

    std::getline(file, line, ' ');
    std::getline(file, line);
    std::istringstream(line) >> null;

    std::getline(file, line);

    items_.resize(n_);
    sol_opt_ = new Solution(*this);

    ItemIdx id;
    Profit p;
    Weight w;
    int    x;
    for (ItemPos i=0; i<n_; ++i) {
        std::getline(file, line, ',');
        std::istringstream(line) >> id;
        std::getline(file, line, ',');
        std::istringstream(line) >> p;
        std::getline(file, line, ',');
        std::istringstream(line) >> w;
        std::getline(file, line);
        std::istringstream(line) >> x;
        items_[i] = {i,w,p};
        if (x == 1)
            sol_opt_->set(i, true);
    }

    file.close();
}

bool Instance::check()
{
    for (ItemPos i=0; i<item_number(); ++i)
        if (item(i).w <= 0 || item(i).w > capacity())
            return false;
    return true;
}

Instance::~Instance()
{
    delete sol_opt_;
    delete sol_red_;
}

Profit Instance::check(boost::filesystem::path cert_file)
{
    if (!boost::filesystem::exists(cert_file))
        return -1;
    boost::filesystem::ifstream file(cert_file, std::ios_base::in);
    bool x;
    Profit p = 0;
    Weight c = 0;
    for (ItemPos i=0; i<item_number(); ++i) {
        file >> x;
        if (x) {
            p += item(i).p;
            c += item(i).w;
        }
    }
    if (c > capacity())
        return -1;
    return p;
}

////////////////////////////////////////////////////////////////////////////////

void Instance::sort()
{
    if (sort_type() == "eff")
        return;
    sort_type_ = "eff";
    std::sort(items_.begin(), items_.begin() + n_,
            [](const Item& i1, const Item& i2) {
            return i1.p * i2.w > i2.p * i1.w;});
    for (ItemPos i=0; i<item_number(); ++i) {
        if (wsum_ + item(i).w > capacity()) {
            b_ = i;
            break;
        }
        wsum_ += item(i).w;
        psum_ += item(i).p;
    }
    r_ = capacity() - wsum_;
}

#define DBG(x)
//#define DBG(x) x

void Instance::sort_partially()
{
    DBG(std::cout << "PARTSORT..." << std::endl;)
    if (sort_type() == "peff" || sort_type() == "eff")
        return;
    sort_type_ = "peff";

    // Trivial cases
    if (item_number() == 0) {
        psum_ = 0;
        wsum_ = 0;
        r_ = capacity();
        b_ = -1;
        return;
    } else if (item_number() == 1) {
        psum_ = item(0).p;
        wsum_ = item(0).w;
        r_ = capacity() - wsum_;
        b_ = -1;
        return;
    }

    // Quick sort like algorithm
    ItemPos f = 0;
    ItemPos l = (n_ > 0)? n_ - 1: 0;
    wsum_ = 0;
    psum_ = 0;
    while (f < l) {
        ItemPos pivot = f + 1 + rand() % (l - f); // Select pivot
        DBG(std::cout << "f " << f << " l " << l << " pivot " << pivot << std::endl;)

        // Partition
        swap(pivot, l);
        ItemPos j = f;
        for (ItemPos i=f; i<l; ++i) {
            if (item(i).p*item(l).w <= item(l).p*item(i).w)
                continue;
            swap(i, j);
            j++;
        }
        swap(j, l);

        // Compute w_curr
        Weight w_curr = wsum_;
        for (ItemPos i=f; i<j; ++i)
            w_curr += item(i).w;

        // Update f and l
        if (w_curr + item(j).w <= c_) {
            for (ItemPos i=f; i<=j; ++i)
                psum_ += item(i).p;
            wsum_ = w_curr + item(j).w;
            f = j+1;
        } else if (w_curr > c_) {
            l = j-1;
        } else {
            for (ItemPos i=f; i<j; ++i)
                psum_ += item(i).p;
            wsum_ = w_curr;
            f = j;
            break;
        }
    }

    // Compute break weight, break profit and break item
    DBG(std::cout << "f " << f << " l " << l << std::endl;)
    if (f == item_number()) {
        b_ = -1;
    } else if (f == item_number() - 1 && wsum_ + item(f).w <= c_) {
        b_ = -1;
        wsum_ += item(f).w;
        psum_ += item(f).p;
    } else {
        b_ = f;
    }
    r_ = c_ - wsum_;

    DBG(std::cout << "PARTSORT... END" << std::endl;)
}

#undef DBG

#define DBG(x)
//#define DBG(x) x

void Instance::surrogate(Weight multiplier, ItemIdx bound)
{
    DBG(std::cout << "SURROGATE..." << std::endl;)
        ItemIdx k = 0;
    for (ItemIdx i=0; i<item_number(); ++i) {
        items_[i].w += multiplier;
        if (item(i).w <= 0) {
            swap(k, i);
            ++k;
        }
    }
    c_ += multiplier * bound;
    sort_type_ = "";
    sort_partially();
    DBG(std::cout << "SURROGATE... END" << std::endl;)
}

#undef DBG

////////////////////////////////////////////////////////////////////////////////

void Instance::swap(ItemPos i1, ItemPos i2, ItemPos i3, ItemPos i4)
{
    Item tmp   = items_[i1];
    items_[i1] = items_[i2];
    items_[i2] = items_[i3];
    items_[i3] = items_[i4];
    items_[i4] = tmp;
}

#define DBG(x)
//#define DBG(x) x

void Instance::reduce1(const Solution& sol_curr, bool verbose)
{
    DBG(std::cout << "REDUCE1... LB " << sol_curr.profit() << std::endl;)
    assert(sort_type() == "eff" || sort_type() == "peff");

    for (ItemIdx i=0; i<=b_; ++i) {
        if (!sol_curr.contains(i))
            continue;
        DBG(std::cout << "i " << i << " (" << item(i).i << ")";)
        Profit ub = reduced_solution()->profit() + break_profit() - item(i).p
                + ((break_capacity() + item(i).w) * item(b_).p) / item(b_).w;
        DBG(std::cout << " UB " << ub;)
        if (ub <= sol_curr.profit()) {
            assert(optimum() == 0
                    || optimum() == sol_curr.profit()
                    || optimal_solution()->contains(i));
            DBG(std::cout << " 1";)
            sol_red_->set(i, true);
            c_    -= item(i).w;
            wsum_ -= item(i).w;
            psum_ -= item(i).p;
            swap(i, b_-1, b_, n_-1);
            b_--;
            n_--;
            i--;
        }
        DBG(std::cout << std::endl;)
    }
    for (ItemPos i=b_; i<n_; ++i) {
        if (sol_curr.contains(item(i).i))
            continue;
        DBG(std::cout << "i " << i << " (" << item(i).i << ")";)
        Profit ub = reduced_solution()->profit() + break_profit() + item(i).p
                + ((break_capacity() - item(i).w) * item(b_).p) / item(b_).w;
        DBG(std::cout << " UB " << ub;)
        if (ub <= sol_curr.profit()) {
            assert(optimum() == -1
                    || optimum() == sol_curr.profit()
                    || !optimal_solution()->contains(i));
            DBG(std::cout << " 0";)
            swap(i, n_-1);
            n_ -= 1;
            i  -= 1;
        }
        DBG(std::cout << std::endl;)
    }

    if (verbose) {
        std::cout
            << item_number() << " / " << total_item_number() << " (" << (double)item_number() / (double)total_item_number() << "); "
            << capacity()    << " / " << total_capacity()    << " (" << (double)capacity()    / (double)total_capacity()    << "); "
            << std::endl;
    }

    DBG(std::cout << "REDUCE1... END" << std::endl;)
}

void Instance::reduce2(const Solution& sol_curr, bool verbose)
{
    DBG(std::cout << "REDUCE2... LB " << sol_curr.profit() << std::endl;)
    assert(sort_type() == "eff");

    std::vector<Item> not_fixed;
    std::vector<Item> fixed_1;
    std::vector<Item> fixed_0;

    std::vector<Item> isum(n_);
    isum[0] = {0,item(0).w, item(0).p};
    for (ItemPos i=1; i<item_number(); ++i) {
        isum[i].i = i;
        isum[i].w = isum[i-1].w + item(i).w;
        isum[i].p = isum[i-1].p + item(i).p;
    }

    for (ItemIdx i=0; i<=b_; ++i) {
        if (!sol_curr.contains(item(i).i) && i != b_) {
            not_fixed.push_back(item(i));
            continue;
        }
        DBG(std::cout << "i " << i << " (" << item(i).i << ")" << std::flush;)
        Item ubitem = {0, capacity() + item(i).w, 0};
        auto s = std::upper_bound(isum.begin(), isum.begin()+item_number(), ubitem,
                [](const Item& i1, const Item& i2) { return i1.w < i2.w;});
        DBG(std::cout << " c+w " << capacity() + item(i).w << std::flush;)
        DBG(std::cout << " s-1 " << *(s-1) << std::flush);
        DBG(std::cout << " s "   << *s     << std::flush);
        Profit ub1 = (s-1)->p - item(i).p + ((capacity() + item(i).w - (s-1)->w) * item((s+1)->i).p) / item((s+1)->i).w;
        Profit ub2 =     s->p - item(i).p + ((capacity() + item(i).w -     s->w) * item((s-1)->i).p) / item((s-1)->i).w;
        Profit ub = (ub1 > ub2)? ub1: ub2;
        DBG(std::cout << " UB " << ub << std::flush;)
        if (ub <= sol_curr.profit()) {
            DBG(std::cout << " 1";)
            assert(optimum() == -1
                    || optimum() == sol_curr.profit()
                    || optimal_solution()->contains(i));
            sol_red_->set(i, true);
            fixed_1.push_back(item(i));
        } else {
            if (i != b_)
                not_fixed.push_back(item(i));
        }
        DBG(std::cout << std::endl;)
    }
    for (ItemIdx i=b_; i<n_; ++i) {
        if (i == b_ && !fixed_1.empty() && fixed_1.back().i == b_)
            continue;
        if (sol_curr.contains(i)) {
            not_fixed.push_back(item(i));
            continue;
        }
        DBG(std::cout << "i " << i;)
        Item ubitem = {0, capacity() - item(i).w, 0};
        auto s = std::upper_bound(isum.begin(), isum.begin()+item_number(), ubitem,
                [](const Item& i1, const Item& i2) { return i1.w < i2.w;});
        Profit ub1 = (s-1)->p + item(i).p + ((capacity() - item(i).w - (s-1)->w) * item((s+1)->i).p) / item((s+1)->i).w;
        Profit ub2 =     s->p + item(i).p + ((capacity() - item(i).w -     s->w) * item((s-1)->i).p) / item((s-1)->i).w;
        Profit ub = (ub1 > ub2)? ub1: ub2;
        DBG(std::cout << " UB " << ub << std::flush;)
        if (ub <= sol_curr.profit()) {
            DBG(std::cout << " 0" << std::flush;)
            assert(optimum() == -1
                    || optimum() == sol_curr.profit()
                    || !optimal_solution()->contains(i));
            fixed_0.push_back(item(i));
        } else {
            DBG(std::cout << " ?" << std::flush;)
            not_fixed.push_back(item(i));
        }
        DBG(std::cout << std::endl;)
    }

    ItemPos i = not_fixed.size();
    ItemPos i0 = fixed_0.size();
    ItemPos i1 = fixed_1.size();
    DBG(std::cout << "i " << i << " i0 " << i0 << " i1 " << i1 << " n " << n_ << std::endl;)
    assert(i + i0 + i1 == n_);
    n_ = i;
    b_ -= i1;

    std::copy(not_fixed.begin(), not_fixed.end(), items_.begin());
    std::copy(fixed_0.begin(), fixed_0.end(), items_.begin() + i);
    ItemIdx k = i + i0;
    for (Item& item: fixed_1) {
        items_[k] = item;
        wsum_ -= item.w;
        psum_ -= item.p;
        c_    -= item.w;
        ++k;
    }
    DBG(std::cout << "b " << b_ << " wsum " << wsum_ << " psum " << psum_ << " r " << r_ << " c " << c_ << std::endl;)

    if (verbose) {
        std::cout
            << item_number() << " / " << total_item_number() << " (" << (double)item_number() / (double)total_item_number() << "); "
            << capacity()    << " / " << total_capacity()    << " (" << (double)capacity()    / (double)total_capacity()    << "); "
            << std::endl;
    }
    DBG(std::cout << "REDUCE2... END" << std::endl;)
}

#undef DBG

////////////////////////////////////////////////////////////////////////////////

Weight Instance::gcd() const
{
    Weight gcd = capacity();
    for (ItemPos i=0; i<item_number(); ++i) {
        if (gcd == 1)
            return gcd;
        Weight wi = item(i).w;
        while (wi != 0) {
            Weight tmp = gcd % wi;
            gcd = wi;
            wi = tmp;
        }
    }
    return gcd;
}

Profit Instance::optimum() const
{
    return optimal_solution()->profit();
}

////////////////////////////////////////////////////////////////////////////////

bool Instance::check_opt(Profit p) const
{
    return (optimum() == 0
            || item_number() != total_item_number()
            || p == optimum());
}

bool Instance::check_ub(Profit p) const
{
    return (optimum() == 0
            || item_number() != total_item_number()
            || p >= optimum());
}

bool Instance::check_lb(Profit p) const
{
    return (optimum() == 0
            || item_number() != total_item_number()
            || p <= optimum());
}

////////////////////////////////////////////////////////////////////////////////

std::ostream& operator<<(std::ostream& os, const Item& it)
{
    os << "(" << it.i << " " << it.w << " " << it.p << " " << (double)it.p/(double)it.w << ")";
    return os;
}

std::ostream& operator<<(std::ostream& os, const Instance& instance)
{
    os
        <<  "n "   << instance.item_number()
        << " c "   << instance.capacity()
        << " opt " << instance.optimum()
        << std::endl;
    os << "b " << instance.break_item() << " wsum " << instance.break_weight() << " psum " << instance.break_profit() << std::endl;
    for (ItemIdx i=0; i<instance.total_item_number(); ++i)
        os << i << ": " << instance.item(i) << " " << instance.optimal_solution()->contains(i) << std::endl;
    return os;
}

