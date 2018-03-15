#include "instance.hpp"
#include "solution.hpp"

#include <sstream>

Instance::Instance(std::vector<Item> items, Weight c):
    name_(""), format_(""), f_(0), l_(items.size()-1), c_(c), c_orig_(c), items_(items)
{
    sol_red_ = new Solution(*this);
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
    file >> l_ >> c_;
    f_ = 0;
    l_--;
    c_orig_ = c_;
    items_.resize(item_number());
    sol_opt_ = new Solution(*this);
    ItemIdx id;
    Profit p;
    Weight w;
    int    x;
    for (ItemPos i=0; i<item_number(); ++i) {
        file >> id >> p >> w >> x;
        items_[i] = {id,w,p};
        // Update max profit, weight and efficiency items
        if (p * i_emax_.w > i_emax_.p * w)
            i_emax_ = {id,w,p};
        if (p > i_pmax_.p)
            i_pmax_ = {id,w,p};
        if (w > i_wmax_.w)
            i_wmax_ = {id,w,p};
        // Update Optimal solution
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
    std::istringstream(line) >> l_;
    f_ = 0;
    l_--;

    std::getline(file, line, ' ');
    std::getline(file, line);
    std::istringstream(line) >> c_;
    c_orig_ = c_;

    std::getline(file, line, ' ');
    std::getline(file, line);
    std::istringstream(line) >> null;

    std::getline(file, line);

    items_.resize(item_number());
    sol_opt_ = new Solution(*this);

    ItemIdx id;
    Profit p;
    Weight w;
    int    x;
    for (ItemPos i=0; i<item_number(); ++i) {
        std::getline(file, line, ',');
        std::istringstream(line) >> id;
        std::getline(file, line, ',');
        std::istringstream(line) >> p;
        std::getline(file, line, ',');
        std::istringstream(line) >> w;
        std::getline(file, line);
        std::istringstream(line) >> x;
        items_[i] = {i,w,p};
        // Update max profit, weight and efficiency items
        if (p * i_emax_.w > i_emax_.p * w)
            i_emax_ = {id,w,p};
        if (p > i_pmax_.p)
            i_pmax_ = {id,w,p};
        if (w > i_wmax_.w)
            i_wmax_ = {id,w,p};
        // Update Optimal solution
        if (x == 1)
            sol_opt_->set(i, true);
    }

    file.close();
}

Instance::Instance(const Instance& ins)
{
    name_ = ins.name();
    format_ = ins.format();

    c_ = ins.capacity();
    c_orig_ = ins.total_capacity();
    sorted_ = ins.sorted();
    items_ = ins.items_;

    if (ins.optimal_solution() != NULL) {
        sol_opt_ = new Solution(*this);
        *sol_opt_ = *ins.optimal_solution();
    }
    if (ins.break_solution() != NULL) {
        sol_break_ = new Solution(*this);
        *sol_break_ = *ins.break_solution();
    }
    sol_red_ = new Solution(*this);
    *sol_red_ = *ins.reduced_solution();
    sol_red_opt_ = ins.sol_red_opt_;
    b_ = ins.break_item();
    isum_ = ins.isum_;
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
    if (sol_opt_ != NULL)
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

#define DBG(x)
//#define DBG(x) x

void Instance::sort()
{
    DBG(std::cout << "SORT..." << std::endl;)
    if (sorted())
        return;
    sorted_ = true;
    if (item_number() > 1)
        std::sort(items_.begin(), items_.begin() + item_number(),
                [](const Item& i1, const Item& i2) {
                return i1.p * i2.w > i2.p * i1.w;});

    compute_break_item();
    update_isum();
    DBG(std::cout << "SORT... END" << std::endl;)
}

#undef DBG

#define DBG(x)
//#define DBG(x) x

void Instance::update_isum()
{
    DBG(std::cout << "UPDATEISUM..." << std::endl;)
    assert(sorted());
    isum_ = std::vector<Item>(item_number()+1);
    isum_[0] = {0,0,0};
    for (ItemPos i=1; i<=item_number(); ++i) {
        isum_[i].i = i;
        isum_[i].w = isum_[i-1].w + item(i-1).w;
        isum_[i].p = isum_[i-1].p + item(i-1).p;
    }
    //std::cout << *this << std::endl;
    //DBG(for (const Item& item: isum_)
        //std::cout << item << " ";
    //std::cout << std::endl;)
    //DBG(std::cout << "UPDATEISUM... END" << std::endl;)
}

#undef DBG

ItemPos Instance::ub_item(Item item) const
{
    assert(sorted());
    auto s = std::upper_bound(isum_.begin(), isum_.end(), item,
            [](const Item& i1, const Item& i2) { return i1.w < i2.w;});
    if (s == isum_.end())
        return item_number();
    return s->i-1;
}

#define DBG(x)
//#define DBG(x) x

void Instance::compute_break_item()
{
    DBG(std::cout << "COMPUTEBREAKITEM..." << std::endl;)
    if (sol_break_ == NULL) {
        sol_break_ = new Solution(*reduced_solution());
    } else {
        *sol_break_ = *reduced_solution();
    }
    for (b_=first_item(); b_<=last_item(); ++b_) {
        //std::cout << "I " << b_ << " " << item(b_) << std::endl;
        if (item(b_).w > sol_break_->remaining_capacity())
            break;
        sol_break_->set(b_, true);
    }
    DBG(std::cout << "b " << b_ << " wsum " << sol_break_->weight() << " psum " << sol_break_->profit() << std::endl;)
    DBG(std::cout << "COMPUTEBREAKITEM... END" << std::endl;)
}

Profit Instance::break_profit() const
{
    return break_solution()->profit() - reduced_solution()->profit();
}

Weight Instance::break_weight() const
{
    return break_solution()->weight() - reduced_solution()->weight();
}

Weight Instance::break_capacity() const
{
    return break_solution()->remaining_capacity();
}

#undef DBG

void Instance::remove_big_items()
{
    for (ItemPos i=first_item(); i<=last_item();) {
        if (item(i).w > capacity()) {
            swap(i, l_);
            l_--;
        } else {
            i++;
        }
    }
}

#define DBG(x)
//#define DBG(x) x

ItemPos Instance::partition(ItemPos f, ItemPos l)
{
    ItemPos pivot = f + 1 + rand() % (l - f); // Select pivot
    //DBG(std::cout << "F " << f << " L " << l << " PIVOT " << pivot << std::endl;)

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

    return j;
}

void Instance::sort_partially()
{
    DBG(std::cout << "PARTSORT..." << std::endl;)
    if (break_item_found())
        return;

    if (item_number() > 1) {
        // Quick sort like algorithm
        ItemPos f = first_item();
        ItemPos l = last_item();
        Weight c = capacity();
        while (f < l) {
            //std::cout << "F " << f << " L " << l << std::flush;
            ItemPos j = partition(f, l);
            //std::cout << " J " << j << std::flush;
            ItemPos w = 0;
            for (ItemPos i=f; i<j; ++i)
                w += item(i).w;
            //std::cout << " W " << w << " C " << c << std::flush;
            if (w + item(j).w <= c) {
                //std::cout << " =>" << std::endl;
                c -= (w + item(j).w);
                int_right_.push_back({f, std::max(f, j-1)});
                f = j+1;
            } else if (w > c) {
                //std::cout << " <=" << std::endl;
                int_left_.push_back({std::min(l, j+1), l});
                l = j-1;
            } else {
                //std::cout << " =" << std::endl;
                int_right_.push_back({f, std::max(f, j-1)});
                int_left_.push_back({std::min(l, j+1), l});
                break;
            }
        }
    }

    compute_break_item();
    DBG(std::cout << "PARTSORT... END" << std::endl;)
}

#undef DBG

#define DBG(x)
//#define DBG(x) x

void Instance::surrogate(Weight multiplier, ItemIdx bound)
{
    DBG(std::cout << "SURROGATE..." << std::endl;)
    ItemIdx k = 0;
    for (ItemIdx i=first_item(); i<=last_item(); ++i) {
        items_[i].w += multiplier;
        if (item(i).w <= 0) {
            swap(k, i);
            ++k;
        }
    }
    c_ += multiplier * bound;
    c_orig_ += multiplier * bound;
    if (sol_opt_ != NULL) {
        delete sol_opt_;
        sol_opt_ = NULL;
    }
    sorted_ = false;
    b_ = -1;
    sort_partially();
    DBG(std::cout << "SURROGATE... END" << std::endl;)
}

#undef DBG

////////////////////////////////////////////////////////////////////////////////

#define DBG(x)
//#define DBG(x) x

void Instance::swap(ItemPos i1, ItemPos i2, ItemPos i3, ItemPos i4)
{
    Item tmp   = items_[i1];
    items_[i1] = items_[i2];
    items_[i2] = items_[i3];
    items_[i3] = items_[i4];
    items_[i4] = tmp;
}

bool Instance::reduce1(Profit lb, bool verbose)
{
    DBG(std::cout << "REDUCE1... LB " << lb << std::endl;)
    assert(break_item_found());
    assert(b_ != l_+1);
    sol_red_opt_ = (lb == optimum());

    DBG(std::cout << "b " << b_ << std::endl;)
    for (ItemIdx i=0; i<=b_;) {
        DBG(std::cout << "I " << i << " (" << item(i).i << ")";)
        Profit ub = reduced_solution()->profit() + break_profit() - item(i).p
                + ((break_capacity() + item(i).w) * item(b_).p) / item(b_).w;
        DBG(std::cout << " UB " << ub;)
        if (ub <= lb) {
            assert(sol_red_opt_ || optimal_solution()->contains(i));
            DBG(std::cout << " 1";)
            sol_red_->set(i, true);
            c_    -= item(i).w;
            if (i != f_)
                swap(i, f_);
            f_++;
            if (c_ < 0)
                return true;
        } else {
            DBG(std::cout << " ?";)
        }
        i++;
        DBG(std::cout << std::endl;)
    }
    for (ItemPos i=l_; i>=b_;) {
        DBG(std::cout << "I " << i << " (" << item(i).i << ")";)
        Profit ub = reduced_solution()->profit() + break_profit() + item(i).p
                + ((break_capacity() - item(i).w) * item(b_).p) / item(b_).w;
        DBG(std::cout << " UB " << ub;)
        if (ub <= lb) {
            assert(sol_red_opt_ || !optimal_solution()->contains(i));
            DBG(std::cout << " 0";)
            if (i != l_)
                swap(i, l_);
            l_--;
        } else {
            DBG(std::cout << " ?";)
        }
        i--;
        DBG(std::cout << std::endl;)
    }

    remove_big_items();

    if (verbose) {
        std::cout
            << "REDUCTION: " << print_lb(lb) << " - "
            << "N " << item_number() << "/" << total_item_number() << " (" << (double)item_number() / (double)total_item_number() << ") - "
            << "C " << capacity()    << "/" << total_capacity()    << " (" << (double)capacity()    / (double)total_capacity()    << ")"
            << std::endl;
    }

    DBG(std::cout << "REDUCE1... END" << std::endl;)
    return (item_number() == 0 || capacity() <= 0);
}

bool Instance::reduce2(Profit lb, bool verbose)
{
    DBG(std::cout << "REDUCE2... LB " << lb << std::endl;)
    assert(sorted());
    sol_red_opt_ = (lb == optimum());

    std::vector<Item> not_fixed;
    std::vector<Item> fixed_1;
    std::vector<Item> fixed_0;

    DBG(std::cout << "b " << b_ << std::endl;)
    for (ItemIdx i=0; i<=b_; ++i) {
        DBG(std::cout << "i " << i << " (" << item(i).i << ")" << std::flush;)
        Item ubitem = {0, capacity() + item(i).w, 0};
        ItemPos bb = ub_item(ubitem);
        DBG(std::cout << " bb " << bb << std::flush;)
        Profit ub = 0;
        if (bb == item_number()) {
            ub = isum(item_number()).p - item(i).p;
            DBG(std::cout << " UB " << ub << std::flush;)
        } else if (bb == item_number() - 1) {
            Profit ub1 = isum(bb  ).p - item(i).p;
            Profit ub2 = isum(bb+1).p - item(i).p + ((capacity() + item(i).w - isum(bb+1).w) * item(bb-1).p + 1) / item(bb-1).w - 1;
            ub = (ub1 > ub2)? ub1: ub2;
            DBG(std::cout << " UB1 " << ub1 << " UB2 " << ub2 << " UB " << ub << std::flush;)
        } else {
            Profit ub1 = isum(bb  ).p - item(i).p + ((capacity() + item(i).w - isum(bb  ).w) * item(bb+1).p    ) / item(bb+1).w;
            Profit ub2 = isum(bb+1).p - item(i).p + ((capacity() + item(i).w - isum(bb+1).w) * item(bb-1).p + 1) / item(bb-1).w - 1;
            ub = (ub1 > ub2)? ub1: ub2;
            DBG(std::cout << " UB1 " << ub1 << " UB2 " << ub2 << " UB " << ub << std::flush;)
        }
        if (ub <= lb) {
            DBG(std::cout << " 1";)
            assert(sol_red_opt_ || optimal_solution()->contains(i));
            sol_red_->set(i, true);
            fixed_1.push_back(item(i));
            if (sol_red_->weight() > capacity())
                return true;
        } else {
            DBG(std::cout << " ?" << std::flush;)
            if (i != b_) {
                not_fixed.push_back(item(i));
            }
        }
        DBG(std::cout << std::endl;)
    }
    for (ItemIdx i=b_; i<=l_; ++i) {
        if (i == b_ && !fixed_1.empty() && fixed_1.back().i == item(b_).i)
            continue;
        DBG(std::cout << "i " << i << " (" << item(i).i << ")" << std::flush;)

        Item ubitem = {0, capacity() - item(i).w, 0};
        ItemPos bb = ub_item(ubitem);
        DBG(std::cout << " bb " << bb << std::flush;)
        Profit ub = 0;
        if (bb == item_number()) {
            ub = isum(item_number()).p + item(i).p;
            DBG(std::cout << " UB " << ub << std::flush;)
        } else if (bb == item_number() - 1) {
            Profit ub1 = isum(bb  ).p + item(i).p;
            Profit ub2 = isum(bb+1).p + item(i).p + ((capacity() - item(i).w - isum(bb+1).w) * item(bb-1).p + 1) / item(bb-1).w - 1;
            ub = (ub1 > ub2)? ub1: ub2;
            DBG(std::cout << " UB " << ub << std::flush;)
        } else if (bb == 0) {
            ub = ((capacity() + item(i).w) * item(bb).p) / item(bb).w;
            DBG(std::cout << " UB " << ub << std::flush;)
        } else {
            Profit ub1 = isum(bb  ).p + item(i).p + ((capacity() - item(i).w - isum(bb  ).w) * item(bb+1).p) / item(bb+1).w;
            Profit ub2 = isum(bb+1).p + item(i).p + ((capacity() - item(i).w - isum(bb+1).w) * item(bb-1).p + 1) / item(bb-1).w - 1;
            ub = (ub1 > ub2)? ub1: ub2;
            DBG(std::cout << " UB1 " << ub1 << " UB2 " << ub2 << " UB " << ub << std::flush;)
        }

        if (ub <= lb) {
            DBG(std::cout << " 0" << std::flush;)
            assert(sol_red_opt_ || !optimal_solution()->contains(i));
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
    DBG(std::cout << "i " << i << " i0 " << i0 << " i1 " << i1 << " n " << item_number() << std::endl;)
    assert(i + i0 + i1 == item_number());
    l_ = i-1;
    b_ -= i1;

    // Update capacity
    for (Item& item: fixed_1)
        c_ -= item.w;
    std::copy(not_fixed.begin(), not_fixed.end(), items_.begin());
    std::copy(fixed_0.begin(), fixed_0.end(), items_.begin() + i);
    for (ItemPos k=0; k<(int)fixed_1.size(); ++k)
        items_[k+i+i0] = fixed_1[k];

    remove_big_items();
    update_isum();

    if (verbose) {
        std::cout
            << "REDUCTION: " << print_lb(lb) << " - "
            << "N " << item_number() << "/" << total_item_number() << " (" << (double)item_number() / (double)total_item_number() << ") - "
            << "C " << (double)capacity()    / (double)total_capacity()
            << std::endl;
    }
    DBG(std::cout << "REDUCE2... END" << std::endl;)
    return (item_number() == 0 || capacity() <= 0);
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
    if (optimal_solution() == NULL)
        return 0;
    return optimal_solution()->profit();
}

////////////////////////////////////////////////////////////////////////////////

bool Instance::check_opt(Profit p) const
{
    if (optimal_solution() != NULL
            && !sol_red_opt_
            && p != optimum()) {
        std::cout << "p " << p << " != OPT " << optimum() << std::endl;
        return false;
    }
    return true;
}

bool Instance::check_sopt(const Solution& sol) const
{
    if (sol.weight() > total_capacity()) {
        std::cout << "w(S) " << sol.weight() << " > c " << total_capacity() << std::endl;
        return false;
    }
    if (optimal_solution() != NULL
                && !sol_red_opt_
                && sol.profit() != optimum()) {
        std::cout << "p(S) " << sol.profit() << " != OPT " << optimum() << std::endl;
        return false;
    }

    return true;
}

bool Instance::check_ub(Profit p) const
{
    if (optimal_solution() != NULL
            && item_number() == total_item_number()
            && p < optimum()) {
        std::cout << "u " << p << " < OPT " << optimum() << std::endl;
        return false;
    }
    return true;
}

bool Instance::check_lb(Profit p) const
{
    return (optimal_solution() == NULL
            || item_number() != total_item_number()
            || p <= optimum());
}

bool Instance::check_sol(const Solution& sol) const
{
    if (sol.weight() > total_capacity()) {
        std::cout << "w(S) " << sol.weight() << " > c " << total_capacity() << std::endl;
        return false;
    }
    if (optimal_solution() != NULL
                && item_number() == total_item_number()
                && sol.profit() > optimum()) {
        std::cout << "p(S) " << sol.profit() << " > OPT " << optimum() << std::endl;
        return false;
    }
    return true;
}

////////////////////////////////////////////////////////////////////////////////

std::string Instance::print_lb(Profit lb) const
{
    return (optimal_solution() == NULL)?
        "LB " + std::to_string(lb):
        "LB " + std::to_string(lb) + " GAP " + std::to_string(optimum() - lb);
}

std::string Instance::print_ub(Profit ub) const
{
    return (optimal_solution() == NULL)?
        "UB " + std::to_string(ub):
        "UB " + std::to_string(ub) + " GAP " + std::to_string(ub - optimum());
}

std::string Instance::print_opt(Profit opt) const
{
    return (optimal_solution() != NULL && optimum() != opt)?
        "OPT " + std::to_string(opt) + " ERROR!":
        "OPT " + std::to_string(opt);
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
    if (instance.break_item_found())
        os << "b " << instance.break_item() << " wsum " << instance.break_weight() << " psum " << instance.break_profit() << std::endl;
    for (ItemIdx i=0; i<instance.total_item_number(); ++i) {
        os << i << ": " << instance.item(i) << std::flush;
        if (instance.optimal_solution() != NULL)
            os << " " << instance.optimal_solution()->contains(i);
        os << std::endl;
    }
    return os;
}

