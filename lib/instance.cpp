#include "instance.hpp"
#include "solution.hpp"
#include "binary_solution.hpp"

#include <sstream>

Instance::Instance(std::vector<Item> items, Weight c):
    name_(""), format_(""), f_(0), l_(items.size()-1), c_orig_(c), items_(items)
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
    compute_max_items();
    assert(check());
}

void Instance::read_standard(boost::filesystem::path filepath)
{
    name_ = filepath.stem().string();
    boost::filesystem::fstream file(filepath, std::ios_base::in);
    file >> l_ >> c_orig_;
    f_ = 0;
    l_--;
    items_.resize(item_number());
    sol_opt_ = new Solution(*this);
    ItemIdx id;
    Profit p;
    Weight w;
    int    x;
    for (ItemPos i=0; i<item_number(); ++i) {
        file >> id >> p >> w >> x;
        items_[i] = {id,w,p};
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
    std::istringstream(line) >> c_orig_;

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

    c_orig_ = ins.total_capacity();
    sorted_ = ins.sorted();
    items_ = ins.items_;
    f_ = ins.first_item();
    l_ = ins.last_item();

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
    compute_max_items();
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
        std::sort(items_.begin()+first_item(), items_.begin()+last_item()+1,
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
    isum_ = std::vector<Item>(total_item_number()+1);
    isum_[0] = {0,0,0};
    for (ItemPos i=1; i<=total_item_number(); ++i) {
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
    auto s = std::upper_bound(isum_.begin()+f_, isum_.begin()+l_+1, item,
            [](const Item& i1, const Item& i2) { return i1.w < i2.w;});
    if (s == isum_.begin()+l_+1)
        return l_+1;
    return s->i-1;
}

#define DBG(x)
//#define DBG(x) x

void Instance::compute_max_items()
{
    i_wmax_ = {-1, -1, -1}; // Max weight item
    i_wmin_ = {-1, c_orig_+1, -1}; // Min weight item
    i_pmax_ = {-1, -1, -1}; // Max profit item
    i_emax_ = {-1, 0, -1};  // Max efficiency item;

    for (ItemPos i = f_; i<=l_; ++i) {
        Profit p = item(i).p;
        Weight w = item(i).w;
        ItemIdx id = item(i).i;
        if (p * i_emax_.w > i_emax_.p * w)
            i_emax_ = {id,w,p};
        if (p > i_pmax_.p)
            i_pmax_ = {id,w,p};
        if (w > i_wmax_.w)
            i_wmax_ = {id,w,p};
    }
}

#undef DBG

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
        DBG(std::cout
                << "I " << b_
                << " " << item(b_)
                << " R " << sol_break_->remaining_capacity()
                << std::endl;)
        if (item(b_).w > sol_break_->remaining_capacity())
            break;
        sol_break_->set(b_, true);
    }
    DBG(std::cout << "B " << b_ << " WSUM " << sol_break_->weight() << " psum " << sol_break_->profit() << std::endl;)
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

Weight Instance::capacity() const
{
    return total_capacity() - reduced_solution()->weight();
}

#undef DBG

#define DBG(x)
//#define DBG(x) x

void Instance::remove_big_items()
{
    DBG(std::cout << "REMOVEBIGITEMS..." << std::endl;)
    if (sorted()) {
        std::vector<Item> not_fixed;
        std::vector<Item> fixed_0;
        for (ItemPos i=first_item(); i<=last_item(); ++i) {
            if (item(i).w > capacity()) {
                fixed_0.push_back(item(i));
            } else {
                not_fixed.push_back(item(i));
            }
        }
        if (fixed_0.size() != 0) {
            ItemPos i = not_fixed.size();
            std::copy(not_fixed.begin(), not_fixed.end(), items_.begin()+f_);
            std::copy(fixed_0.begin(), fixed_0.end(), items_.begin()+f_+i);
            l_ = f_+i-1;
        }
    } else {
        bool break_item_removed = false;
        for (ItemPos i=first_item(); i<=last_item(); ++i) {
            DBG(std::cout << "I " << i
                    << " " << item(i)
                    << " F " << f_
                    << " L " << l_
                    << " B " << b_
                    << " C " << capacity()
                    << std::flush;)
            if (item(i).w > capacity()) {
                if (i == b_)
                    break_item_removed = true;
                if (i <= b_) {
                    swap(i, f_);
                    f_++;
                } else if (i > b_) {
                    swap(i, l_);
                    l_--;
                }
            }
            DBG(std::cout << std::endl;)
        }
        if (break_item_removed)
            sort_partially();
    }
    DBG(std::cout << "REMOVEBIGITEMS... END" << std::endl;)
}

#undef DBG

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

    int_right_.clear();
    int_left_.clear();
    if (item_number() > 1) {
        // Quick sort like algorithm
        ItemPos f = first_item();
        ItemPos l = last_item();
        Weight c = capacity();
        while (f < l) {
            if (l - f < 128) {
                std::sort(items_.begin()+f, items_.begin()+l+1,
                        [](const Item& i1, const Item& i2) {
                        return i1.p * i2.w > i2.p * i1.w;});
                ItemPos b = f;
                for (; b<=l; ++b) {
                    if (c < item(b).w)
                        break;
                    c -= item(b).w;
                }
                if (f < b)
                    int_left_.push_back({f, b-1});
                if (b < l)
                    int_right_.push_back({b+1, l});
                break;
            }
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
                if (f <= j)
                    int_left_.push_back({f, j});
                f = j+1;
            } else if (w > c) {
                //std::cout << " <=" << std::endl;
                if (j <= l)
                    int_right_.push_back({j, l});
                l = j-1;
            } else {
                //std::cout << " =" << std::endl;
                if (f <= j-1)
                    int_left_.push_back({f, j-1});
                if (j+1 <= l)
                    int_right_.push_back({j+1, l});
                break;
            }
        }
    }

    DBG(std::cout << "LEFT" << std::flush;
    for (auto i: int_left_)
        std::cout << " [" << i.f << "," << i.l << "]" << std::flush;
    std::cout << std::endl;
    std::cout << "RIGHT" << std::flush;
    for (auto i: int_right_)
        std::cout << " [" << i.l << "," << i.f << "]" << std::flush;
    std::cout << std::endl;)

    compute_break_item();
    s_ = b_;
    t_ = b_;
    DBG(std::cout << "PARTSORT... END" << std::endl;)
}

#undef DBG

#define DBG(x)
//#define DBG(x) x

void Instance::sort_right(Profit lb)
{
    DBG(std::cout << "SORTRIGHT..." << std::endl;)
    DBG(std::cout << *this << std::endl;)

    Interval in = int_right_.back();
    //std::cout << "SORT " << in << std::endl;
    int_right_.pop_back();
    ItemPos k = t_;
    for (ItemPos i=in.f; i<=in.l; ++i) {
        DBG(std::cout << "I " << i << std::flush;)
            Profit ub = break_solution()->profit() + item(i).p
            + ((break_capacity() - item(i).w) * item(b_).p) / item(b_).w;
        DBG(std::cout << " LB " << lb << " UB " << ub << std::flush;)
        if ((item(i).w <= capacity() && ub > lb)
                || (k == t_ && i == in.l)) {
            k++;
            DBG(std::cout << " K " << k << std::endl;)
            swap(k, i);
        } else {
            assert(optimal_solution() == NULL
                    || optimal_solution()->profit() == lb
                    || !optimal_solution()->contains(i));
            DBG(std::cout << " REDUCE " << item(i) << std::endl;)
        }
    }
    std::sort(items_.begin()+t_+1, items_.begin()+k+1,
            [](const Item& i1, const Item& i2) {
            return i1.p * i2.w > i2.p * i1.w;});
    t_ = k;
    if (int_right_.size() == 0)
        l_ = t_;
    DBG(std::cout << *this << std::endl;)
    DBG(std::cout << "SORTRIGHT... END" << std::endl;)
}

void Instance::sort_left(Profit lb)
{
    DBG(std::cout << "SORTLEFT..." << std::endl;)
    DBG(std::cout << *this << std::endl;)

    Interval in = int_left_.back();
    DBG(std::cout << in << std::endl;)
    //std::cout << "SORT " << in << std::endl;
    int_left_.pop_back();
    ItemPos k = s_;
    DBG(std::cout << "K " << k << std::endl;)
    for (ItemPos i=in.l; i>=in.f; --i) {
        DBG(std::cout << "I " << i << std::flush;)
        Profit ub = break_solution()->profit() - item(i).p
            + ((break_capacity() + item(i).w) * item(b_).p) / item(b_).w;
        DBG(std::cout << " LB " << lb << " UB " << ub << std::flush;)
        if ((item(i).w <= capacity() && ub > lb)
                || (i == in.f && k == s_)) {
            k--;
            DBG(std::cout << " K " << k << std::endl;)
            swap(k, i);
        } else {
            assert(optimal_solution() == NULL
                    || optimal_solution()->profit() == lb
                    || optimal_solution()->contains(i));
            sol_red_->set(i, true);
            DBG(std::cout << " REDUCE " << item(i) << " C " << capacity() << std::endl;)
        }
    }
    std::sort(items_.begin()+k, items_.begin()+s_,
            [](const Item& i1, const Item& i2) {
            return i1.p * i2.w > i2.p * i1.w;});
    s_ = k;
    if (int_left_.size() == 0)
        f_ = s_;
    DBG(std::cout << *this << std::endl;)
    DBG(std::cout << "SORTLEFT... END" << std::endl;)
}

#undef DBG

#define DBG(x)
//#define DBG(x) x

void Instance::surrogate(Weight multiplier, ItemIdx bound)
{
    surrogate(multiplier, bound, first_item());
}

void Instance::surrogate(Weight multiplier, ItemIdx bound, ItemPos first)
{
    DBG(std::cout << "SURROGATE... K " << bound << " S " << multiplier << " F " << first << std::endl;)
    sol_break_->clear();
    if (sol_opt_ != NULL) {
        delete sol_opt_;
        sol_opt_ = NULL;
    }
    f_ = first;
    l_ = last_item();
    for (ItemIdx i=f_; i<=l_; ++i)
        sol_red_->set(i, false);
    bound -= sol_red_->item_number();
    for (ItemIdx i=f_; i<=l_; ++i) {
        items_[i].w += multiplier;
        if (items_[i].w <= 0) {
            sol_red_->set(i, true);
            swap(i, f_);
            f_++;
        }
    }
    c_orig_ += multiplier * bound;
    compute_max_items();

    sorted_ = false;
    b_      = -1;
    sort_partially();
    DBG(std::cout << "SURROGATE... END" << std::endl;)
}

#undef DBG

////////////////////////////////////////////////////////////////////////////////

#define DBG(x)
//#define DBG(x) x

void Instance::reduce1(Profit lb, bool verbose)
{
    DBG(std::cout << "REDUCE1... LB " << lb << std::endl;)
    assert(break_item_found());
    assert(b_ != l_+1);
    sol_red_opt_ = (lb == optimum());

    DBG(std::cout << "b " << b_ << std::endl;)
    for (ItemIdx i=f_; i<b_;) {
        DBG(std::cout << "I " << i << " (" << item(i).i << ")" << " F " << f_;)
        Profit ub = reduced_solution()->profit() + break_profit() - item(i).p
                + ((break_capacity() + item(i).w) * item(b_).p) / item(b_).w;
        DBG(std::cout << " UB " << ub;)
        if (ub <= lb) {
            assert(sol_red_opt_ || optimal_solution()->contains(i));
            DBG(std::cout << " 1";)
            sol_red_->set(i, true);
            if (i != f_)
                swap(i, f_);
            f_++;
            if (capacity() < 0)
                return;
        } else {
            DBG(std::cout << " ?";)
        }
        i++;
        DBG(std::cout << std::endl;)
    }
    for (ItemPos i=l_; i>b_;) {
        DBG(std::cout << "I " << i << " (" << item(i).i << ")" << " L " << l_;)
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
    compute_break_item();

    if (verbose) {
        std::cout
            << "REDUCTION: " << print_lb(lb) << " - "
            << "N " << item_number() << "/" << total_item_number() << " (" << (double)item_number() / (double)total_item_number() << ") - "
            << "C " << capacity()    << "/" << total_capacity()    << " (" << (double)capacity()    / (double)total_capacity()    << ")"
            << std::endl;
    }

    DBG(std::cout << "REDUCE1... END" << std::endl;)
}

void Instance::reduce2(Profit lb, bool verbose)
{
    DBG(std::cout << "REDUCE2... LB " << lb << std::endl;)
    assert(sorted());
    sol_red_opt_ = (lb == optimum());

    std::vector<Item> not_fixed;
    std::vector<Item> fixed_1;
    std::vector<Item> fixed_0;

    DBG(std::cout << "b " << b_ << std::endl;)
    for (ItemPos i=f_; i<=b_; ++i) {
        DBG(std::cout << "i " << i << " (" << item(i).i << ")" << std::flush;)
        Item ubitem = {0, total_capacity() + item(i).w, 0};
        ItemPos bb = ub_item(ubitem);
        DBG(std::cout << " bb " << bb << std::flush;)
        Profit ub = 0;
        if (bb == last_item() + 1) {
            ub = isum(last_item()+1).p - item(i).p;
            DBG(std::cout << " UB " << ub << std::flush;)
        } else if (bb == last_item()) {
            Profit ub1 = isum(bb  ).p - item(i).p;
            Profit ub2 = isum(bb+1).p - item(i).p + ((total_capacity() + item(i).w - isum(bb+1).w) * item(bb-1).p + 1) / item(bb-1).w - 1;
            ub = (ub1 > ub2)? ub1: ub2;
            DBG(std::cout << " UB1 " << ub1 << " UB2 " << ub2 << " UB " << ub << std::flush;)
        } else {
            Profit ub1 = isum(bb  ).p - item(i).p + ((total_capacity() + item(i).w - isum(bb  ).w) * item(bb+1).p    ) / item(bb+1).w;
            Profit ub2 = isum(bb+1).p - item(i).p + ((total_capacity() + item(i).w - isum(bb+1).w) * item(bb-1).p + 1) / item(bb-1).w - 1;
            ub = (ub1 > ub2)? ub1: ub2;
            DBG(std::cout << " UB1 " << ub1 << " UB2 " << ub2 << " UB " << ub << std::flush;)
        }
        if (ub <= lb) {
            DBG(std::cout << " 1";)
            //assert(sol_red_opt_ || optimal_solution()->contains(i));
            sol_red_->set(i, true);
            fixed_1.push_back(item(i));
            if (capacity() < 0)
                return;
        } else {
            DBG(std::cout << " ?" << std::flush;)
            if (i != b_)
                not_fixed.push_back(item(i));
        }
        DBG(std::cout << std::endl;)
    }
    for (ItemPos i=b_; i<=l_; ++i) {
        if (i == b_ && !fixed_1.empty() && fixed_1.back().i == item(b_).i)
            continue;
        DBG(std::cout << "I " << i << " (" << item(i).i << ")" << std::flush;)

        Item ubitem = {0, total_capacity() - item(i).w, 0};
        ItemPos bb = ub_item(ubitem);
        DBG(std::cout << " bb " << bb << std::flush;)
        Profit ub = 0;
        if (bb == last_item() + 1) {
            ub = isum(last_item()+1).p + item(i).p;
            DBG(std::cout << " UB " << ub << std::flush;)
        } else if (bb == last_item()) {
            Profit ub1 = isum(bb  ).p + item(i).p;
            Profit ub2 = isum(bb+1).p + item(i).p + ((total_capacity() - item(i).w - isum(bb+1).w) * item(bb-1).p + 1) / item(bb-1).w - 1;
            ub = (ub1 > ub2)? ub1: ub2;
            DBG(std::cout << " UB " << ub << std::flush;)
        } else if (bb == 0) {
            ub = ((total_capacity() + item(i).w) * item(bb).p) / item(bb).w;
            DBG(std::cout << " UB " << ub << std::flush;)
        } else {
            Profit ub1 = isum(bb  ).p + item(i).p + ((total_capacity() - item(i).w - isum(bb  ).w) * item(bb+1).p) / item(bb+1).w;
            Profit ub2 = isum(bb+1).p + item(i).p + ((total_capacity() - item(i).w - isum(bb+1).w) * item(bb-1).p + 1) / item(bb-1).w - 1;
            ub = (ub1 > ub2)? ub1: ub2;
            DBG(std::cout << " UB1 " << ub1 << " UB2 " << ub2 << " UB " << ub << std::flush;)
        }

        if (ub <= lb) {
            DBG(std::cout << " 0" << std::flush;)
            //assert(sol_red_opt_ || !optimal_solution()->contains(i));
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
    DBG(std::cout << "I " << i << " I0 " << i0 << " I1 " << i1 << " N " << item_number() << std::endl;)
    assert(i + i0 + i1 == item_number());

    std::copy(fixed_1.begin(), fixed_1.end(), items_.begin()+f_);
    std::copy(not_fixed.begin(), not_fixed.end(), items_.begin()+f_+i1);
    std::copy(fixed_0.begin(), fixed_0.end(), items_.begin()+f_+i1+i);
    f_ += i1;
    l_ -= i0;

    DBG(std::cout << std::endl << *this << std::endl;)

    remove_big_items();
    compute_break_item();
    update_isum();

    DBG(std::cout << std::endl << *this << std::endl;)

    if (verbose) {
        std::cout
            << "REDUCTION: " << print_lb(lb) << " - "
            << "N " << item_number() << "/" << total_item_number() << " (" << (double)item_number() / (double)total_item_number() << ") - "
            << "C " << (double)capacity()    / (double)total_capacity()
            << std::endl;
    }
    DBG(std::cout << "REDUCE2... END" << std::endl;)
}

#undef DBG

////////////////////////////////////////////////////////////////////////////////

#define DBG(x)
//#define DBG(x) x

void Instance::set_first_item(ItemPos j)
{
    assert(j >= f_);
    for (ItemPos i=f_; i<j; ++i)
        sol_red_->set(i, true);
    f_ = j;
}

void Instance::set_last_item(ItemPos j)
{
    assert(j <= l_);
    l_ = j;
}

void Instance::fix(BSolFactory bsolf, BSol bsol)
{
    DBG(std::cout << "FIX..." << std::endl;)
    ItemPos f = std::max(f_, bsolf.x1());
    ItemPos l = std::min(l_, bsolf.x2());
    DBG(std::cout << "F " << f << " L " << l << std::endl;)
    std::vector<Item> not_fixed;
    std::vector<Item> fixed_1;
    std::vector<Item> fixed_0;
    for (ItemPos j=f_; j<f; ++j)
        not_fixed.push_back(item(j));
    for (ItemPos j=f; j<=l; ++j) {
        DBG(std::cout << "J " << j << std::flush;)
        if (bsolf.contains(bsol, j)) {
            DBG(std::cout << " 1" << std::endl;)
            fixed_1.push_back(item(j));
            sol_red_->set(j, true);
            //assert(optimal_solution() == NULL || optimal_solution()->contains(j));
        } else {
            DBG(std::cout << " 0" << std::endl;)
            fixed_0.push_back(item(j));
            //assert(optimal_solution() == NULL || !optimal_solution()->contains(j));
        }
    }
    for (ItemPos j=l+1; j<=l_; ++j)
        not_fixed.push_back(item(j));

    ItemPos i = not_fixed.size();
    ItemPos i1 = fixed_1.size();
    ItemPos i0 = fixed_0.size();
    std::copy(fixed_1.begin(), fixed_1.end(), items_.begin()+f_);
    std::copy(not_fixed.begin(), not_fixed.end(), items_.begin()+f_+i1);
    std::copy(fixed_0.begin(), fixed_0.end(), items_.begin()+f_+i1+i);

    f_ += i1;
    l_ -= i0;
    DBG(std::cout << "F " << f_ << " L " << l_ << std::endl;)

    remove_big_items();
    if (sorted()) {
        compute_break_item();
        update_isum();
    } else {
        b_ = -1;
    }
    DBG(std::cout << "FIX... END" << std::endl;)
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

std::ostream& operator<<(std::ostream& os, const Interval& interval)
{
    os << "[" << interval.f << "," << interval.l << "]";
    return os;
}

std::ostream& operator<<(std::ostream& os, const Instance& instance)
{
    os
        <<  "n "   << instance.item_number()
        << " c "   << instance.capacity()
        << " opt " << instance.optimum() << std::endl
        << "F " << instance.first_item()
        << " L " << instance.last_item()
        << std::endl;
    if (instance.break_item_found())
        os << "b " << instance.break_item() << " wsum " << instance.break_weight() << " psum " << instance.break_profit() << std::endl;
    for (ItemPos i=0; i<instance.total_item_number(); ++i) {
        os << i << ": " << instance.item(i) << std::flush;
        if (instance.break_solution() != NULL)
            os << " B " << instance.break_solution()->contains(i);
        if (instance.optimal_solution() != NULL)
            os << " O " << instance.optimal_solution()->contains(i);
        if (instance.reduced_solution() != NULL)
            os << " R " << instance.reduced_solution()->contains(i);
        if (instance.break_item_found() && i == instance.break_item())
            os << " B";
        if (i == instance.first_item())
            os << " F";
        if (i == instance.last_item())
            os << " L";
        os << std::endl;
    }
    return os;
}

