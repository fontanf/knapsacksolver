#include "knapsack/lib/instance.hpp"
#include "knapsack/lib/solution.hpp"
#include "knapsack/lib/part_solution_1.hpp"
#include "knapsack/lib/part_solution_2.hpp"

#include <sstream>

#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/filter/zlib.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include <boost/iostreams/filter/bzip2.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/serialization/vector.hpp>

using namespace knapsack;

Instance::Instance(ItemIdx n, Weight c):
    name_(""), format_(""), c_orig_(c), f_(0), l_(-1)
{
    items_.reserve(n);
    sol_red_ = new Solution(*this);
}


void Instance::add_item(Weight w, Profit p)
{
    add_item(w, p, -1);
}

void Instance::add_item(Weight w, Profit p, Label l)
{
    ItemIdx j = items_.size();
    items_.push_back({j, w, p, l});
    l_ = j;
    version_++;
    sol_opt_ = NULL;
    sol_red_->resize(j+1);
    sol_break_ = NULL;
}

void Instance::add_items(const std::vector<std::pair<Weight, Profit>>& wp)
{
    for (auto i: wp)
        add_item(i.first, i.second);
}

const Item& Instance::max_weight_item()
{
    compute_max_items();
    return j_wmax_;
}

const Item& Instance::max_profit_item()
{
    compute_max_items();
    return j_pmax_;
}

const Item& Instance::max_efficiency_item()
{
    compute_max_items();
    return j_emax_;
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

    name_ = filepath.stem().string();
    std::stringstream data;
    if (filepath.extension() == ".txt") {
        boost::filesystem::ifstream file(filepath, std::ios_base::in);
        data << file.rdbuf();
        file.close();
    } else if (filepath.extension() == ".bz2") {
        boost::filesystem::ifstream file(filepath, std::ios_base::in);
        boost::iostreams::filtering_istream in;
        in.push(boost::iostreams::bzip2_decompressor());
        in.push(file);
        boost::iostreams::copy(in, data);
        file.close();
    } else {
        std::cout << filepath.extension() << ": extension unknown." << std::endl;
        assert(false);
    }

    boost::filesystem::fstream file(FORMAT, std::ios_base::in);
    std::getline(file, format_);
    if        (format_ == "knapsack_standard") {
        read_standard(data);
        boost::filesystem::path sol = filepath;
        sol += ".sol";
        if (boost::filesystem::exists(sol))
            read_standard_solution(sol);
    } else if (format_ == "subsetsum_standard") {
        read_subsetsum_standard(data);
        boost::filesystem::path sol = filepath;
        sol += ".sol";
        if (boost::filesystem::exists(sol))
            read_standard_solution(sol);
    } else if (format_ == "knapsack_pisinger") {
        read_pisinger(data);
    } else {
        std::cout << format_ << ": Unknown instance format." << std::endl;
        assert(false);
    }

    sol_red_ = new Solution(*this);
}

void Instance::read_standard(std::stringstream& data)
{
    ItemIdx n;
    data >> n >> c_orig_;

    f_ = 0;
    l_ = n-1;

    items_.reserve(n);
    Profit p;
    Weight w;
    for (ItemPos j=0; j<n; ++j) {
        data >> p >> w;
        add_item(w,p);
    }
}

void Instance::read_subsetsum_standard(std::stringstream& data)
{
    ItemIdx n;
    data >> n >> c_orig_;

    f_ = 0;
    l_ = n-1;

    items_.reserve(n);
    Weight w;
    for (ItemPos j=0; j<n; ++j) {
        data >> w;
        add_item(w,w);
    }
}

void Instance::read_standard_solution(boost::filesystem::path filepath)
{
    sol_opt_ = new Solution(*this);
    boost::filesystem::ifstream file(filepath, std::ios_base::in);

    int x = 0;
    for (ItemPos j=0; j<total_item_number(); ++j) {
        file >> x;
        sol_opt_->set(j, x);
    }
}

void Instance::read_pisinger(std::stringstream& data)
{
    uint_fast64_t null;

    std::getline(data, name_);

    std::string line;
    std::istringstream iss;

    std::getline(data, line, ' ');
    std::getline(data, line);
    std::istringstream(line) >> l_;
    f_ = 0;
    l_--;

    std::getline(data, line, ' ');
    std::getline(data, line);
    std::istringstream(line) >> c_orig_;

    std::getline(data, line, ' ');
    std::getline(data, line);
    std::istringstream(line) >> null;

    std::getline(data, line);

    items_.reserve(item_number());
    sol_opt_ = new Solution(*this);

    ItemIdx id;
    Profit p;
    Weight w;
    int    x;
    for (ItemPos j=0; j<item_number(); ++j) {
        std::getline(data, line, ',');
        std::istringstream(line) >> id;
        std::getline(data, line, ',');
        std::istringstream(line) >> p;
        std::getline(data, line, ',');
        std::istringstream(line) >> w;
        std::getline(data, line);
        std::istringstream(line) >> x;
        add_item(w,p);
        // Update Optimal solution
        if (x == 1)
            sol_opt_->set(j, true);
    }
}

Instance::Instance(const Instance& ins)
{
    name_ = ins.name_;
    format_ = ins.format_;

    c_orig_ = ins.c_orig_;
    sorted_ = ins.sorted_;
    items_ = ins.items_;
    f_ = ins.f_;
    l_ = ins.l_;

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
    b_ = ins.b_;
    isum_ = ins.isum_;
}

bool Instance::check()
{
    for (ItemPos j=0; j<item_number(); ++j)
        if (item(j).w <= 0 || item(j).w > capacity())
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
    std::vector<int> so(total_item_number(), 0);
    for (ItemPos j=0; j<total_item_number(); ++j)
        file >> so[j];

    Solution sol(*this);
    for (ItemPos j=0; j<total_item_number(); ++j)
        if (so[item(j).j] == 1)
            sol.set(j, true);

    if (sol.weight() > capacity())
        return -1;
    return sol.profit();
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
    isum_.clear();
    isum_.reserve(total_item_number()+1);
    isum_.push_back({0,0,0});
    for (ItemPos j=1; j<=total_item_number(); ++j)
        isum_.push_back({j, isum_[j-1].w + item(j-1).w, isum_[j-1].p + item(j-1).p});
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
    return s->j-1;
}

#define DBG(x)
//#define DBG(x) x

void Instance::compute_max_items()
{
    if (max_version_ == version_)
        return;
    max_version_ = version_;

    j_wmax_ = {-1, -1, -1};        // Max weight item
    j_wmin_ = {-1, c_orig_+1, -1}; // Min weight item
    j_pmax_ = {-1, -1, -1};        // Max profit item
    j_emax_ = {-1, 0, -1};         // Max efficiency item;

    for (ItemPos j=f_; j<=l_; ++j) {
        Profit p = item(j).p;
        Weight w = item(j).w;
        if (p * j_emax_.w > j_emax_.p * w)
            j_emax_ = item(j);
        if (p > j_pmax_.p)
            j_pmax_ = item(j);
        if (w > j_wmax_.w)
            j_wmax_ = item(j);
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
        for (ItemPos j=first_item(); j<=last_item(); ++j) {
            if (item(j).w > capacity()) {
                fixed_0.push_back(item(j));
            } else {
                not_fixed.push_back(item(j));
            }
        }
        if (fixed_0.size() != 0) {
            ItemPos j = not_fixed.size();
            std::copy(not_fixed.begin(), not_fixed.end(), items_.begin()+f_);
            std::copy(fixed_0.begin(), fixed_0.end(), items_.begin()+f_+j);
            l_ = f_+j-1;
        }
    } else {
        bool break_item_removed = false;
        for (ItemPos j=first_item(); j<=last_item(); ++j) {
            DBG(std::cout << "J " << j
                    << " " << item(j)
                    << " F " << f_
                    << " L " << l_
                    << " B " << b_
                    << " C " << capacity()
                    << std::flush;)
            if (item(j).w > capacity()) {
                if (j == b_)
                    break_item_removed = true;
                if (j <= b_) {
                    swap(j, f_);
                    f_++;
                } else if (j > b_) {
                    swap(j, l_);
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
    for (ItemPos k=f; k<l; ++k) {
        if (item(k).p*item(l).w <= item(l).p*item(k).w)
            continue;
        swap(k, j);
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
            for (ItemPos k=f; k<j; ++k)
                w += item(k).w;
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
    for (ItemPos j=in.f; j<=in.l; ++j) {
        DBG(std::cout << "J " << j << std::flush;)
            Profit ub = break_solution()->profit() + item(j).p
            + ((break_capacity() - item(j).w) * item(b_).p) / item(b_).w;
        DBG(std::cout << " LB " << lb << " UB " << ub << std::flush;)
        if ((item(j).w <= capacity() && ub > lb)
                || (k == t_ && j == in.l)) {
            k++;
            DBG(std::cout << " K " << k << std::endl;)
            swap(k, j);
        } else {
            assert(optimal_solution() == NULL
                    || optimal_solution()->profit() == lb
                    || !optimal_solution()->contains(j));
            DBG(std::cout << " REDUCE " << item(j) << std::endl;)
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
    for (ItemPos j=in.l; j>=in.f; --j) {
        DBG(std::cout << "J " << j << std::flush;)
        Profit ub = break_solution()->profit() - item(j).p
            + ((break_capacity() + item(j).w) * item(b_).p) / item(b_).w;
        DBG(std::cout << " LB " << lb << " UB " << ub << std::flush;)
        if ((item(j).w <= capacity() && ub > lb)
                || (j == in.f && k == s_)) {
            k--;
            DBG(std::cout << " K " << k << std::endl;)
            swap(k, j);
        } else {
            assert(optimal_solution() == NULL
                    || optimal_solution()->profit() == lb
                    || optimal_solution()->contains(j));
            sol_red_->set(j, true);
            DBG(std::cout << " REDUCE " << item(j) << " C " << capacity() << std::endl;)
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
    for (ItemIdx j=f_; j<=l_; ++j)
        sol_red_->set(j, false);
    bound -= sol_red_->item_number();
    for (ItemIdx j=f_; j<=l_; ++j) {
        items_[j].w += multiplier;
        if (items_[j].w <= 0) {
            sol_red_->set(j, true);
            swap(j, f_);
            f_++;
        }
    }
    c_orig_ += multiplier * bound;
    version_++;

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
    for (ItemIdx j=f_; j<b_;) {
        DBG(std::cout << "J " << j << " (" << item(j).j << ")" << " F " << f_;)
        Profit ub = reduced_solution()->profit() + break_profit() - item(j).p
                + ((break_capacity() + item(j).w) * item(b_).p) / item(b_).w;
        DBG(std::cout << " UB " << ub;)
        if (ub <= lb) {
            assert(sol_red_opt_ || optimal_solution()->contains(j));
            DBG(std::cout << " 1";)
            sol_red_->set(j, true);
            if (j != f_)
                swap(j, f_);
            f_++;
            if (capacity() < 0)
                return;
        } else {
            DBG(std::cout << " ?";)
        }
        j++;
        DBG(std::cout << std::endl;)
    }
    for (ItemPos j=l_; j>b_;) {
        DBG(std::cout << "J " << j << " (" << item(j).j << ")" << " L " << l_;)
        Profit ub = reduced_solution()->profit() + break_profit() + item(j).p
                + ((break_capacity() - item(j).w) * item(b_).p) / item(b_).w;
        DBG(std::cout << " UB " << ub;)
        if (ub <= lb) {
            assert(sol_red_opt_ || !optimal_solution()->contains(j));
            DBG(std::cout << " 0";)
            if (j != l_)
                swap(j, l_);
            l_--;
        } else {
            DBG(std::cout << " ?";)
        }
        j--;
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
    for (ItemPos j=f_; j<=b_; ++j) {
        DBG(std::cout << "J " << j << " (" << item(j).j << ")" << std::flush;)
        Item ubitem = {0, total_capacity() + item(j).w, 0};
        ItemPos bb = ub_item(ubitem);
        DBG(std::cout << " bb " << bb << std::flush;)
        Profit ub = 0;
        if (bb == last_item() + 1) {
            ub = isum(last_item()+1).p - item(j).p;
            DBG(std::cout << " UB " << ub << std::flush;)
        } else if (bb == last_item()) {
            Profit ub1 = isum(bb  ).p - item(j).p;
            Profit ub2 = isum(bb+1).p - item(j).p + ((total_capacity() + item(j).w - isum(bb+1).w) * item(bb-1).p + 1) / item(bb-1).w - 1;
            ub = (ub1 > ub2)? ub1: ub2;
            DBG(std::cout << " UB1 " << ub1 << " UB2 " << ub2 << " UB " << ub << std::flush;)
        } else {
            Profit ub1 = isum(bb  ).p - item(j).p + ((total_capacity() + item(j).w - isum(bb  ).w) * item(bb+1).p    ) / item(bb+1).w;
            Profit ub2 = isum(bb+1).p - item(j).p + ((total_capacity() + item(j).w - isum(bb+1).w) * item(bb-1).p + 1) / item(bb-1).w - 1;
            ub = (ub1 > ub2)? ub1: ub2;
            DBG(std::cout << " UB1 " << ub1 << " UB2 " << ub2 << " UB " << ub << std::flush;)
        }
        if (ub <= lb) {
            DBG(std::cout << " 1";)
            //assert(sol_red_opt_ || optimal_solution()->contains(j));
            sol_red_->set(j, true);
            fixed_1.push_back(item(j));
            if (capacity() < 0)
                return;
        } else {
            DBG(std::cout << " ?" << std::flush;)
            if (j != b_)
                not_fixed.push_back(item(j));
        }
        DBG(std::cout << std::endl;)
    }
    for (ItemPos j=b_; j<=l_; ++j) {
        if (j == b_ && !fixed_1.empty() && fixed_1.back().j == item(b_).j)
            continue;
        DBG(std::cout << "J " << j << " (" << item(j).j << ")" << std::flush;)

        Item ubitem = {0, total_capacity() - item(j).w, 0};
        ItemPos bb = ub_item(ubitem);
        DBG(std::cout << " bb " << bb << std::flush;)
        Profit ub = 0;
        if (bb == last_item() + 1) {
            ub = isum(last_item()+1).p + item(j).p;
            DBG(std::cout << " UB " << ub << std::flush;)
        } else if (bb == last_item()) {
            Profit ub1 = isum(bb  ).p + item(j).p;
            Profit ub2 = isum(bb+1).p + item(j).p + ((total_capacity() - item(j).w - isum(bb+1).w) * item(bb-1).p + 1) / item(bb-1).w - 1;
            ub = (ub1 > ub2)? ub1: ub2;
            DBG(std::cout << " UB " << ub << std::flush;)
        } else if (bb == 0) {
            ub = ((total_capacity() + item(j).w) * item(bb).p) / item(bb).w;
            DBG(std::cout << " UB " << ub << std::flush;)
        } else {
            Profit ub1 = isum(bb  ).p + item(j).p + ((total_capacity() - item(j).w - isum(bb  ).w) * item(bb+1).p) / item(bb+1).w;
            Profit ub2 = isum(bb+1).p + item(j).p + ((total_capacity() - item(j).w - isum(bb+1).w) * item(bb-1).p + 1) / item(bb-1).w - 1;
            ub = (ub1 > ub2)? ub1: ub2;
            DBG(std::cout << " UB1 " << ub1 << " UB2 " << ub2 << " UB " << ub << std::flush;)
        }

        if (ub <= lb) {
            DBG(std::cout << " 0" << std::flush;)
            //assert(sol_red_opt_ || !optimal_solution()->contains(j));
            fixed_0.push_back(item(j));
        } else {
            DBG(std::cout << " ?" << std::flush;)
            not_fixed.push_back(item(j));
        }
        DBG(std::cout << std::endl;)
    }

    ItemPos j = not_fixed.size();
    ItemPos j0 = fixed_0.size();
    ItemPos j1 = fixed_1.size();
    DBG(std::cout << "J " << j << " J0 " << j0 << " J1 " << j1 << " N " << item_number() << std::endl;) assert(j + j0 + j1 == item_number());

    std::copy(fixed_1.begin(), fixed_1.end(), items_.begin()+f_);
    std::copy(not_fixed.begin(), not_fixed.end(), items_.begin()+f_+j1);
    std::copy(fixed_0.begin(), fixed_0.end(), items_.begin()+f_+j1+j);
    f_ += j1;
    l_ -= j0;

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

void Instance::set_first_item(ItemPos k)
{
    assert(k >= f_);
    for (ItemPos j=f_; j<k; ++j)
        sol_red_->set(j, true);
    f_ = k;
}

void Instance::set_last_item(ItemPos k)
{
    assert(k <= l_);
    l_ = k;
}

void Instance::fix(PartSolFactory1 psolf, PartSol1 psol)
{
    DBG(std::cout << "FIX..." << std::endl;)
    ItemPos f = std::max(f_, psolf.x1());
    ItemPos l = std::min(l_, psolf.x2());
    DBG(std::cout << "F " << f << " L " << l << std::endl;)
    std::vector<Item> not_fixed;
    std::vector<Item> fixed_1;
    std::vector<Item> fixed_0;
    for (ItemPos j=f_; j<f; ++j)
        not_fixed.push_back(item(j));
    for (ItemPos j=f; j<=l; ++j) {
        DBG(std::cout << "J " << j << std::flush;)
        if (psolf.contains(psol, j)) {
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

    ItemPos j = not_fixed.size();
    ItemPos j1 = fixed_1.size();
    ItemPos j0 = fixed_0.size();
    std::copy(fixed_1.begin(), fixed_1.end(), items_.begin()+f_);
    std::copy(not_fixed.begin(), not_fixed.end(), items_.begin()+f_+j1);
    std::copy(fixed_0.begin(), fixed_0.end(), items_.begin()+f_+j1+j);

    f_ += j1;
    l_ -= j0;
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

void Instance::fix(PartSolFactory2 psolf, PartSol2 psol)
{
    DBG(std::cout << "FIX..." << std::endl;)

    std::vector<int> vec(total_item_number(), 0);
    for (ItemPos j=0; j<psolf.size(); ++j) {
        ItemPos idx = psolf.indices()[j];
        if (idx == -1)
            continue;
        vec[psolf.indices()[j]] = (psolf.contains(psol, j))? 1: -1;
    }

    std::vector<Item> not_fixed;
    std::vector<Item> fixed_1;
    std::vector<Item> fixed_0;
    for (ItemPos j=f_; j<=l_; ++j) {
        DBG(std::cout << "J " << j << std::flush;)
        if (vec[j] == 0) {
            DBG(std::cout << " ?" << std::endl;)
            not_fixed.push_back(item(j));
        } else if (vec[j] == 1) {
            DBG(std::cout << " 1" << std::endl;)
            fixed_1.push_back(item(j));
            sol_red_->set(j, true);
        } else {
            assert(vec[j] == -1);
            DBG(std::cout << " 0" << std::endl;)
            fixed_0.push_back(item(j));
        }
    }

    ItemPos j = not_fixed.size();
    ItemPos j1 = fixed_1.size();
    ItemPos j0 = fixed_0.size();
    std::copy(fixed_1.begin(), fixed_1.end(), items_.begin()+f_);
    std::copy(not_fixed.begin(), not_fixed.end(), items_.begin()+f_+j1);
    std::copy(fixed_0.begin(), fixed_0.end(), items_.begin()+f_+j1+j);

    f_ += j1;
    l_ -= j0;
    DBG(std::cout << "F " << f_ << " L " << l_ << std::endl;)

    remove_big_items();
    if (sorted()) {
        compute_break_item();
        update_isum();
    } else {
        b_ = -1;
    }

    DBG(std::cout << *this << std::endl;)
    DBG(std::cout << "FIX... END" << std::endl;)
}

#undef DBG

////////////////////////////////////////////////////////////////////////////////

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

std::ostream& knapsack::operator<<(std::ostream& os, const Item& it)
{
    os << "(" << it.j << " " << it.w << " " << it.p << " " << (double)it.p/(double)it.w << " " << it.l << ")";
    return os;
}

std::ostream& knapsack::operator<<(std::ostream& os, const Interval& interval)
{
    os << "[" << interval.f << "," << interval.l << "]";
    return os;
}

std::ostream& knapsack::operator<<(std::ostream& os, const Instance& instance)
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
    for (ItemPos j=0; j<instance.total_item_number(); ++j) {
        os << j << ": " << instance.item(j) << std::flush;
        if (instance.break_solution() != NULL)
            os << " B " << instance.break_solution()->contains(j);
        if (instance.optimal_solution() != NULL)
            os << " O " << instance.optimal_solution()->contains(j);
        if (instance.reduced_solution() != NULL)
            os << " R " << instance.reduced_solution()->contains(j);
        if (instance.break_item_found() && j == instance.break_item())
            os << " B";
        if (j == instance.first_item())
            os << " F";
        if (j == instance.last_item())
            os << " L";
        os << std::endl;
    }
    return os;
}

