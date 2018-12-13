#include "knapsack/lib/instance.hpp"
#include "knapsack/lib/solution.hpp"

#include <sstream>
#include <iomanip>

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
    sol_opt_ = NULL;
    sol_break_ = NULL;
}

void Instance::add_items(const std::vector<std::pair<Weight, Profit>>& wp)
{
    for (auto i: wp)
        add_item(i.first, i.second);
}

Instance::Instance(boost::filesystem::path filepath)
{
    if (!boost::filesystem::exists(filepath)) {
        std::cout << filepath << ": file not found." << std::endl;
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


    boost::filesystem::path FORMAT = filepath.parent_path() / "FORMAT.txt";
    if (!boost::filesystem::exists(FORMAT)) {
        format_ = "knapsack_standard";
    } else {
        boost::filesystem::fstream file(FORMAT, std::ios_base::in);
        std::getline(file, format_);
    }

    if (format_ == "knapsack_standard") {
        read_standard(data);
        //boost::filesystem::path sol = filepath;
        //sol += ".sol";
        //if (boost::filesystem::exists(sol))
        //read_standard_solution(sol);
    } else if (format_ == "subsetsum_standard") {
        read_subsetsum_standard(data);
        //boost::filesystem::path sol = filepath;
        //sol += ".sol";
        //if (boost::filesystem::exists(sol))
        //read_standard_solution(sol);
    } else if (format_ == "knapsack_pisinger") {
        read_pisinger(data);
    } else {
        std::cout << format_ << ": Unknown instance format." << std::endl;
        assert(false);
    }
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
    if (ins.reduced_solution() != NULL) {
        sol_red_ = new Solution(*this);
        *sol_red_ = *ins.reduced_solution();
    }
    b_ = ins.b_;
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
    if (sol_red_ != NULL)
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

ItemPos Instance::max_efficiency_item() const
{
    ItemPos j_max = 0;
    for (ItemPos j=0; j<total_item_number(); ++j)
        if (item(j).p * item(j_max).w > item(j_max).p * item(j).w)
            j_max = j;
    return j_max;
}

ItemPos Instance::max_profit_item() const
{
    ItemPos j_max = 0;
    for (ItemPos j=0; j<total_item_number(); ++j)
        if (item(j).p > item(j_max).p)
            j_max = j;
    return j_max;
}

ItemPos Instance::max_weight_item() const
{
    ItemPos j_max = 0;
    for (ItemPos j=0; j<total_item_number(); ++j)
        if (item(j).w > item(j_max).w)
            j_max = j;
    return j_max;
}

std::vector<Weight> Instance::min_weights() const
{
    ItemIdx n = total_item_number();
    std::vector<Weight> min_weight(n);
    min_weight[n-1] = item(n-1).w;
    for (ItemIdx i=n-2; i>=0; --i)
        min_weight[i] = std::min(item(i).w, min_weight[i+1]);
    return min_weight;
}

/******************************************************************************/

void Instance::sort()
{
    if (sorted())
        return;
    if (sol_red_ == NULL)
        sol_red_ = new Solution(*this);
    sorted_ = true;
    if (item_number() > 1)
        std::sort(items_.begin()+first_item(), items_.begin()+last_item()+1,
                [](const Item& i1, const Item& i2) {
                return i1.p * i2.w > i2.p * i1.w;});

    compute_break_item();
}

std::vector<Item> Instance::get_isum() const
{
    assert(sorted());
    std::vector<Item> isum;
    isum.reserve(total_item_number()+1);
    isum.clear();
    isum.push_back({0,0,0});
    for (ItemPos j=1; j<=total_item_number(); ++j)
        isum.push_back({j, isum[j-1].w + item(j-1).w, isum[j-1].p + item(j-1).p});
    return isum;
}

ItemPos Instance::ub_item(const std::vector<Item>& isum, Item item) const
{
    assert(sorted());
    auto s = std::upper_bound(isum.begin()+f_, isum.begin()+l_+1, item,
            [](const Item& i1, const Item& i2) { return i1.w < i2.w;});
    if (s == isum.begin()+l_+1)
        return l_+1;
    return s->j-1;
}

void Instance::compute_break_item()
{
    if (sol_break_ == NULL) {
        sol_break_ = new Solution(*reduced_solution());
    } else {
        *sol_break_ = *reduced_solution();
    }
    for (b_=first_item(); b_<=last_item(); ++b_) {
        if (item(b_).w > sol_break_->remaining_capacity())
            break;
        sol_break_->set(b_, true);
    }
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

void Instance::remove_big_items()
{
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
        for (ItemPos j=first_item(); j<=last_item();) {
            if (item(j).w <= capacity()) {
                j++;
                continue;
            }
            if (j == b_)
                b_ = -1;
            if (j <= b_) {
                swap(j, f_);
                f_++;
                j++;
            } else {
                swap(j, l_);
                l_--;
            }
        }
        sort_partially();
    }
}

ItemPos Instance::partition(ItemPos f, ItemPos l)
{
    ItemPos pivot = f + 1 + rand() % (l - f); // Select pivot

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
    if (break_item_found())
        return;
    if (sol_red_ == NULL)
        sol_red_ = new Solution(*this);

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

    compute_break_item();

    s_ = b_;
    t_ = b_;
}

void Instance::sort_right(Profit lb)
{
    Interval in = int_right_.back();
    int_right_.pop_back();
    ItemPos k = t_;
    for (ItemPos j=in.f; j<=in.l; ++j) {
        Profit ub = break_solution()->profit() + item(j).p
            + ((break_capacity() - item(j).w) * item(b_).p) / item(b_).w;
        if ((item(j).w <= capacity() && ub > lb)
                || (k == t_ && j == in.l)) {
            k++;
            swap(k, j);
        }
    }
    std::sort(items_.begin()+t_+1, items_.begin()+k+1,
            [](const Item& i1, const Item& i2) {
            return i1.p * i2.w > i2.p * i1.w;});
    t_ = k;
    if (int_right_.size() == 0)
        l_ = t_;
}

void Instance::sort_left(Profit lb)
{
    Interval in = int_left_.back();
    int_left_.pop_back();
    ItemPos k = s_;
    for (ItemPos j=in.l; j>=in.f; --j) {
        Profit ub = break_solution()->profit() - item(j).p
            + ((break_capacity() + item(j).w) * item(b_).p) / item(b_).w;
        if ((item(j).w <= capacity() && ub > lb)
                || (j == in.f && k == s_)) {
            k--;
            swap(k, j);
        } else {
            sol_red_->set(j, true);
        }
    }
    std::sort(items_.begin()+k, items_.begin()+s_,
            [](const Item& i1, const Item& i2) {
            return i1.p * i2.w > i2.p * i1.w;});
    s_ = k;
    if (int_left_.size() == 0)
        f_ = s_;
}

void Instance::surrogate(Weight multiplier, ItemIdx bound)
{
    surrogate(multiplier, bound, first_item());
}

void Instance::surrogate(Weight multiplier, ItemIdx bound, ItemPos first)
{
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

    sorted_ = false;
    b_      = -1;
    sort_partially();
}

/******************************************************************************/

void Instance::reduce1(Profit lb, Info& info)
{
    LOG(info, LOG_FOLD_START << " reduce1 - lb " << lb << " b_ " << b_ << std::endl;);

    assert(break_item_found());
    assert(b_ != l_+1);

    for (ItemIdx j=f_; j<b_;) {
        LOG(info, "j " << j << " (" << item(j) << ") f_ " << f_);

        Profit ub = reduced_solution()->profit() + break_profit() - item(j).p
                + ((break_capacity() + item(j).w) * item(b_).p) / item(b_).w;
        LOG(info, " ub " << ub);

        if (ub <= lb) {
            LOG(info, " 1" << std::endl);
            sol_red_->set(j, true);
            if (j != f_)
                swap(j, f_);
            f_++;
            if (capacity() < 0)
                return;
        } else {
            LOG(info, " ?" << std::endl);
        }
        j++;
    }
    for (ItemPos j=l_; j>b_;) {
        LOG(info, "j " << j << " (" << item(j) << ") l_ " << l_);

        Profit ub = reduced_solution()->profit() + break_profit() + item(j).p
                + ((break_capacity() - item(j).w) * item(b_).p) / item(b_).w;
        LOG(info, " ub " << ub)

        if (ub <= lb) {
            LOG(info, " 0" << std::endl);
            if (j != l_)
                swap(j, l_);
            l_--;
        } else {
            LOG(info, " ?" << std::endl);
        }
        j--;
    }

    remove_big_items();
    compute_break_item();

    VER(info, "Reduction: " << lb << " - "
            << "n " << item_number() << "/" << total_item_number()
            << " ("  << ((double)item_number() / (double)total_item_number()) << ") -"
            << " c " << ((double)capacity()    / (double)total_capacity()) << std::endl);
    LOG(info, "n " << item_number() << "/" << total_item_number() << std::endl);
    LOG(info, "c " << capacity() << "/" << total_capacity() << std::endl);
    LOG(info, LOG_FOLD_END << std::endl);
}

void Instance::reduce2(Profit lb, Info& info)
{
    LOG(info, LOG_FOLD_START << " Reduce 2: lb " << lb << " b_ " << b_ << std::endl);
    assert(sorted());

    std::vector<Item> isum = get_isum();

    std::vector<Item> not_fixed;
    std::vector<Item> fixed_1;
    std::vector<Item> fixed_0;

    for (ItemPos j=f_; j<=b_; ++j) {
        LOG(info, "j " << j << " (" << item(j) << ")");
        Item ubitem = {0, total_capacity() + item(j).w, 0};
        ItemPos bb = ub_item(isum, ubitem);
        LOG(info, " bb " << bb);
        Profit ub = 0;
        if (bb == last_item() + 1) {
            ub = isum[last_item()+1].p - item(j).p;
            LOG(info, " ub " << ub);
        } else if (bb == last_item()) {
            Profit ub1 = isum[bb  ].p - item(j).p;
            Profit ub2 = isum[bb+1].p - item(j).p + ((total_capacity() + item(j).w - isum[bb+1].w) * item(bb-1).p + 1) / item(bb-1).w - 1;
            ub = (ub1 > ub2)? ub1: ub2;
            LOG(info, " ub1 " << ub1 << " ub2 " << ub2 << " ub " << ub);
        } else {
            Profit ub1 = isum[bb  ].p - item(j).p + ((total_capacity() + item(j).w - isum[bb  ].w) * item(bb+1).p    ) / item(bb+1).w;
            Profit ub2 = isum[bb+1].p - item(j).p + ((total_capacity() + item(j).w - isum[bb+1].w) * item(bb-1).p + 1) / item(bb-1).w - 1;
            ub = (ub1 > ub2)? ub1: ub2;
            LOG(info, " ub1 " << ub1 << " ub2 " << ub2 << " ub " << ub);
        }
        if (ub <= lb) {
            LOG(info, " 1" << std::endl);
            sol_red_->set(j, true);
            fixed_1.push_back(item(j));
            if (capacity() < 0)
                return;
        } else {
            LOG(info, " ?" << std::endl);
            if (j != b_)
                not_fixed.push_back(item(j));
        }
    }
    for (ItemPos j=b_; j<=l_; ++j) {
        if (j == b_ && !fixed_1.empty() && fixed_1.back().j == item(b_).j)
            continue;
        LOG(info, "j " << j << " (" << item(j) << ")");

        Item ubitem = {0, total_capacity() - item(j).w, 0};
        ItemPos bb = ub_item(isum, ubitem);
        LOG(info, " bb " << bb);

        Profit ub = 0;
        if (bb == last_item() + 1) {
            ub = isum[last_item()+1].p + item(j).p;
            LOG(info, " ub " << ub);
        } else if (bb == last_item()) {
            Profit ub1 = isum[bb  ].p + item(j).p;
            Profit ub2 = isum[bb+1].p + item(j).p + ((total_capacity() - item(j).w - isum[bb+1].w) * item(bb-1).p + 1) / item(bb-1).w - 1;
            ub = (ub1 > ub2)? ub1: ub2;
            LOG(info, " ub1 " << ub1 << " ub2 " << ub2 << " ub " << ub);
        } else if (bb == 0) {
            ub = ((total_capacity() + item(j).w) * item(bb).p) / item(bb).w;
            LOG(info, " ub " << ub);
        } else {
            Profit ub1 = isum[bb  ].p + item(j).p + ((total_capacity() - item(j).w - isum[bb  ].w) * item(bb+1).p) / item(bb+1).w;
            Profit ub2 = isum[bb+1].p + item(j).p + ((total_capacity() - item(j).w - isum[bb+1].w) * item(bb-1).p + 1) / item(bb-1).w - 1;
            ub = (ub1 > ub2)? ub1: ub2;
            LOG(info, " ub1 " << ub1 << " ub2 " << ub2 << " ub " << ub);
        }

        if (ub <= lb) {
            LOG(info, " 0" << std::endl);
            fixed_0.push_back(item(j));
        } else {
            LOG(info, " ?" << std::endl);
            not_fixed.push_back(item(j));
        }
    }

    ItemPos j = not_fixed.size();
    ItemPos j0 = fixed_0.size();
    ItemPos j1 = fixed_1.size();
    LOG(info, "j " << j << " j0 " << j0 << " j1 " << j1 << " n " << item_number() << std::endl);

    std::copy(fixed_1.begin(), fixed_1.end(), items_.begin()+f_);
    std::copy(not_fixed.begin(), not_fixed.end(), items_.begin()+f_+j1);
    std::copy(fixed_0.begin(), fixed_0.end(), items_.begin()+f_+j1+j);
    f_ += j1;
    l_ -= j0;

    remove_big_items();
    compute_break_item();

    VER(info, "Reduction: " << lb << " - "
            << "n " << item_number() << "/" << total_item_number()
            << " ("  << ((double)item_number() / (double)total_item_number()) << ") -"
            << " c " << ((double)capacity()    / (double)total_capacity()) << std::endl);
    LOG(info, "n " << item_number() << "/" << total_item_number() << std::endl);
    LOG(info, "c " << capacity() << "/" << total_capacity() << std::endl);
    LOG(info, LOG_FOLD_END << std::endl);
}

/******************************************************************************/

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

bool Instance::update_sorted()
{
    if (sorted_)
        return true;
    if (b_ == -1)
        return false;
    if (first_item() >= first_sorted_item() && last_item() <= last_sorted_item()) {
        sorted_ = true;
        return true;
    }
    return false;
}

void Instance::fix(const std::vector<int> vec)
{
    std::vector<Item> not_fixed;
    std::vector<Item> fixed_1;
    std::vector<Item> fixed_0;
    for (ItemPos j=f_; j<=l_; ++j) {
        if (vec[j] == 0) {
            not_fixed.push_back(item(j));
        } else if (vec[j] == 1) {
            fixed_1.push_back(item(j));
            sol_red_->set(j, true);
        } else {
            assert(vec[j] == -1);
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

    remove_big_items();
    if (sorted()) {
        compute_break_item();
    } else {
        b_ = -1;
    }
}

/******************************************************************************/

Profit Instance::optimum() const
{
    if (optimal_solution() == NULL)
        return -1;
    return optimal_solution()->profit();
}

/******************************************************************************/

Solution knapsack::algorithm_end(const Solution& sol, Info& info)
{
    double t = info.elapsed_time();
    PUT(info, "Solution.Value", sol.profit());
    PUT(info, "Solution.Time", t);
    VER(info, "---" << std::endl
            << "Value: " << sol.profit() << std::endl
            << "Time: " << t << std::endl);
    return sol;
}

Profit knapsack::algorithm_end(Profit val, Info& info)
{
    double t = info.elapsed_time();
    PUT(info, "Solution.Value", val);
    PUT(info, "Solution.Time", t);
    VER(info, "---" << std::endl
            << "Value: " << val << std::endl
            << "Time: " << t << std::endl);
    return val;
}

/******************************************************************************/

std::ostream& knapsack::operator<<(std::ostream& os, const Item& it)
{
    os << it.j << "\t" << it.w << "\t" << it.p << "\t" << (double)it.p/(double)it.w
        << "\t" << it.l;
    return os;
}

std::ostream& knapsack::operator<<(std::ostream& os, const Instance& ins)
{
    os
        <<  "n_total " << ins.total_item_number()
        << " c_total "   << ins.total_capacity()
        << " opt " << ins.optimum()
        << std::endl;
    if (ins.reduced_solution() != NULL)
        os
            <<  "n " << ins.item_number() << " c " << ins.capacity()
            << " f " << ins.first_item() << " l " << ins.last_item()
            << std::endl;
    if (ins.break_item_found())
        os << "b " << ins.break_item()
            << " wsum " << ins.break_weight()
            << " psum " << ins.break_profit()
            << std::endl;
    os << "pos\tj\tw\tp\te\tl\tb\topt\tred\tb/f/l" << std::endl;
    for (ItemPos j=0; j<ins.total_item_number(); ++j) {
        const Item& it = ins.item(j);
        os
            << j << "\t"
            << it.j << "\t"
            << it.w << "\t"
            << it.p << "\t"
            << std::setprecision(2) << (double)it.p/(double)it.w << "\t"
            << it.l << "\t";

        if (ins.break_solution() != NULL) {
            os << ins.break_solution()->contains(j);
        } else {
            os << " ";
        }
        os << "\t" << std::flush;

        if (ins.optimal_solution() != NULL) {
            os << ins.optimal_solution()->contains(j);
        } else {
            os << " ";
        }
        os << "\t" << std::flush;

        if (ins.reduced_solution() != NULL) {
            os << ins.reduced_solution()->contains(j);
        } else {
            os << " ";
        }
        os << "\t" << std::flush;

        if (ins.break_item_found() && j == ins.break_item())
            os << "b" << std::flush;
        if (j == ins.first_item())
            os << "f" << std::flush;
        if (j == ins.last_item())
            os << "l" << std::flush;
        os << std::endl;
    }
    return os;
}

void Instance::plot(std::string filename)
{
    std::ofstream file(filename);
    file << "w p" << std::endl;
    for (ItemIdx i=0; i<item_number(); ++i)
        file << item(i).w << " " << item(i).p << std::endl;
    file.close();
}

void Instance::write(std::string filename)
{
    std::ofstream file(filename);
    file << item_number() << " " << total_capacity() << std::endl << std::endl;
    for (ItemIdx i=0; i<item_number(); ++i)
        file << item(i).w << " " << item(i).p << std::endl;
    file.close();
}

