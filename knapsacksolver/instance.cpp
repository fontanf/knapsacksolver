#include "knapsacksolver/instance.hpp"
#include "knapsacksolver/solution.hpp"
#include "knapsacksolver/algorithms/upper_bound_dembo.hpp"

#include <sstream>
#include <iomanip>

using namespace knapsacksolver;

////////////////////////////////////////////////////////////////////////////////
/////////////////////////////// Create instances ///////////////////////////////
////////////////////////////////////////////////////////////////////////////////

Instance::Instance():
    f_(0),
    l_(-1)
{
}

void Instance::add_item(Weight weight, Profit profit)
{
    Item item;
    item.j = items_.size();
    item.w = weight;
    item.p = profit;
    items_.push_back(item);
    l_ = items_.size() - 1;
}

void Instance::clear()
{
    items_.clear();
    capacity_ = 0;
    optimal_solution_ = nullptr;
    b_ = -1;
    f_ = 0;
    l_ = -1;
    s_init_ = -1;
    t_init_ = -1;
    s_prime_ = -1;
    t_prime_ = -1;
    sort_status_ = 0;
    int_right_.clear();
    int_left_.clear();
    reduced_solution_ = nullptr;
    break_solution_ = nullptr;
}

Instance::Instance(Weight c, const std::vector<std::pair<Weight, Profit>>& wp):
    capacity_(c), f_(0), l_(-1)
{
    for (const auto& p: wp)
        add_item(p.first, p.second);
}

void Instance::set_optimal_solution(Solution& solution)
{
    optimal_solution_ = std::unique_ptr<Solution>(new Solution(solution));
}

Instance::Instance(std::string instance_path, std::string format):
    path_(instance_path)
{
    std::ifstream file(instance_path);
    std::ifstream file_sol(instance_path + ".sol");
    if (!file.good()) {
        throw std::runtime_error(
                "Unable to open file \"" + instance_path + "\".");
    }

    if (format == "standard") {
        read_standard(file);
    } else if (format == "pisinger") {
        read_pisinger(file);
    } else if (format == "jooken") {
        read_jooken(file);
    } else if (format == "subsetsum_standard") {
        read_subsetsum_standard(file);
    } else {
        throw std::invalid_argument(
                "Unknown instance format \"" + format + "\".");
    }

    f_ = 0;
    l_ = number_of_items() - 1;
}

void Instance::read_standard(std::ifstream& file)
{
    ItemIdx n;
    Weight c;
    file >> n >> c;
    set_capacity(c);

    items_.reserve(n);
    Weight w;
    Profit p;
    for (ItemPos j = 0; j < n; ++j) {
        file >> w >> p;
        add_item(w, p);
    }
}

void Instance::read_jooken(std::ifstream& file)
{
    ItemIdx n;
    file >> n;

    items_.reserve(n);
    ItemIdx tmp;
    Weight w;
    Profit p;
    for (ItemPos j = 0; j < n; ++j) {
        file >> tmp >> p >> w;
        add_item(w, p);
    }

    Weight c;
    file >> c;
    set_capacity(c);
}

static inline void rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}

std::vector<std::string> split(std::string line)
{
    std::vector<std::string> v;
    std::stringstream ss(line);
    std::string tmp;
    while (getline(ss, tmp, ',')) {
        rtrim(tmp);
        v.push_back(tmp);
    }
    return v;
}

void Instance::read_pisinger(std::ifstream& file)
{
    ItemIdx n;
    Weight c;
    std::string tmp;
    Profit opt;
    file >> tmp >> tmp >> n >> tmp >> c >> tmp >> opt >> tmp >> tmp;
    set_capacity(c);

    items_.reserve(n);
    std::vector<int> x(n);
    getline(file, tmp);
    for (ItemPos j = 0; j < n; ++j) {
        getline(file, tmp);
        std::vector<std::string> line = split(tmp);
        add_item(std::stol(line[2]), std::stol(line[1]));
        x[j] = std::stol(line[3]);
    }

    optimal_solution_ = std::unique_ptr<Solution>(new Solution(*this));
    for (ItemPos j = 0; j < n; ++j)
        optimal_solution_->set(j, x[j]);
    assert(optimal_solution_->profit() == opt);
}

void Instance::read_subsetsum_standard(std::ifstream& file)
{
    ItemIdx n;
    file >> n >> capacity_;

    items_.reserve(n);
    Weight w;
    for (ItemPos j = 0; j < n; ++j) {
        file >> w;
        add_item(w,w);
    }
}

Instance::Instance(const Instance& instance):
    items_(instance.items_),
    capacity_(instance.capacity_),
    b_(instance.b_),
    f_(instance.f_),
    l_(instance.l_),
    s_init_(instance.s_init_),
    t_init_(instance.t_init_),
    s_prime_(instance.s_prime_),
    t_prime_(instance.t_prime_),
    sort_status_(instance.sort_status_),
    int_right_(instance.int_right_),
    int_left_(instance.int_left_)
{
    if (instance.optimal_solution() != nullptr) {
        optimal_solution_ = std::unique_ptr<Solution>(new Solution(*this));
        for (ItemIdx j = 0; j < number_of_items(); ++j)
            if (instance.optimal_solution()->contains(j))
                optimal_solution_->set(j, 1);
    }
    if (instance.break_solution() != nullptr) {
        break_solution_ = std::unique_ptr<Solution>(new Solution(*this));
        for (ItemIdx j = 0; j < number_of_items(); ++j)
            if (instance.break_solution()->contains(j))
                break_solution_->set(j, 1);
    }
    if (instance.reduced_solution() != nullptr) {
        reduced_solution_ = std::unique_ptr<Solution>(new Solution(*this));
        for (ItemIdx j = 0; j < number_of_items(); ++j)
            if (instance.reduced_solution()->contains(j))
                reduced_solution_->set(j, 1);
    }
}

Instance& Instance::operator=(const Instance& instance)
{
    if (this != &instance) {
        items_ = instance.items_;
        capacity_ = instance.capacity_;

        b_ = instance.b_;
        f_ = instance.f_;
        l_ = instance.l_;
        s_init_ = instance.s_init_;
        t_init_ = instance.t_init_;
        s_prime_ = instance.s_prime_;
        t_prime_ = instance.t_prime_;
        sort_status_ = instance.sort_status_;
        int_right_ = instance.int_right_;
        int_left_ = instance.int_left_;

        if (instance.optimal_solution() != nullptr) {
            optimal_solution_ = std::unique_ptr<Solution>(new Solution(*this));
            for (ItemIdx j = 0; j < number_of_items(); ++j)
                if (instance.optimal_solution()->contains(j))
                    optimal_solution_->set(j, 1);
        }
        if (instance.break_solution() != nullptr) {
            break_solution_ = std::unique_ptr<Solution>(new Solution(*this));
            for (ItemIdx j = 0; j < number_of_items(); ++j)
                if (instance.break_solution()->contains(j))
                    break_solution_->set(j, 1);
        }
        if (instance.reduced_solution() != nullptr) {
            reduced_solution_ = std::unique_ptr<Solution>(new Solution(*this));
            for (ItemIdx j = 0; j < number_of_items(); ++j)
                if (instance.reduced_solution()->contains(j))
                    reduced_solution_->set(j, 1);
        }
    }
    return *this;
}

Instance::~Instance() { }

Instance Instance::reset(const Instance& instance)
{
    Instance instance_new;
    instance_new.items_ = instance.items_;
    instance_new.capacity_ = instance.capacity_;
    instance_new.f_ = 0;
    instance_new.l_ = instance.items_.size() - 1;
    return instance_new;
}

bool Instance::check()
{
    for (ItemPos j = 0; j < reduced_number_of_items(); ++j)
        if (item(j).w <= 0 || item(j).w > reduced_capacity())
            return false;
    return true;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

ItemPos Instance::max_efficiency_item(FFOT_DBG(Info& info)) const
{
    ItemPos k = -1;
    for (ItemPos j = first_item(); j <= last_item(); ++j)
        if (k == -1 || item(j).p * item(k).w > item(k).p * item(j).w)
            k = j;
    FFOT_LOG(info, "max_efficiency_item " << k << std::endl);
    return k;
}

ItemPos Instance::before_break_item(FFOT_DBG(Info& info)) const
{
    ItemPos k = -1;
    for (ItemPos j = first_item(); j < break_item(); ++j)
        if (k == -1 || (item(k).p * item(j).w > item(j).p * item(k).w))
            k = j;
    FFOT_LOG(info, "before_break_item " << k << std::endl);
    return k;
}

ItemPos Instance::max_profit_item(FFOT_DBG(Info& info)) const
{
    ItemPos k = -1;
    for (ItemPos j = first_item(); j <= last_item(); ++j)
        if (k == -1 || item(j).p > item(k).p)
            k = j;
    FFOT_LOG(info, "max_profit_item " << k << std::endl);
    return k;
}

ItemPos Instance::min_profit_item(FFOT_DBG(Info& info)) const
{
    ItemPos k = -1;
    for (ItemPos j = first_item(); j <= last_item(); ++j)
        if (k == -1 || item(j).p < item(k).p)
            k = j;
    FFOT_LOG(info, "min_profit_item " << k << std::endl);
    return k;
}

ItemPos Instance::max_weight_item(FFOT_DBG(Info& info)) const
{
    ItemPos k = -1;
    for (ItemPos j = first_item(); j <= last_item(); ++j)
        if (k == -1 || item(j).w > item(k).w)
            k = j;
    FFOT_LOG(info, "max_weight_item " << k << std::endl);
    return k;
}

ItemPos Instance::min_weight_item(FFOT_DBG(Info& info)) const
{
    ItemPos k = -1;
    for (ItemPos j = first_item(); j <= last_item(); ++j)
        if (k == -1 || item(j).w < item(k).w)
            k = j;
    FFOT_LOG(info, "min_weight_item " << k << std::endl);
    return k;
}

std::vector<Weight> Instance::min_weights() const
{
    ItemIdx n = number_of_items();
    std::vector<Weight> min_weight(n);
    min_weight[n - 1] = item(n - 1).w;
    for (ItemIdx i = n - 2; i >= 0; --i)
        min_weight[i] = std::min(item(i).w, min_weight[i + 1]);
    return min_weight;
}

std::vector<Item> Instance::get_isum() const
{
    assert(sort_status() == 2);
    std::vector<Item> isum;
    isum.reserve(number_of_items()+1);
    isum.clear();
    Item it;
    it.j = 0;
    it.w = 0;
    it.p = 0;
    isum.push_back(it);
    for (ItemPos j = 1; j <= number_of_items(); ++j) {
        Item it;
        it.j = j;
        it.w = isum[j - 1].w + item(j - 1).w;
        it.p = isum[j - 1].p + item(j - 1).p;
        isum.push_back(it);
    }
    return isum;
}

ItemPos Instance::gamma1(FFOT_DBG(Info& info)) const
{
    Weight w = break_weight() - item(break_item() - 1).w;
    ItemPos k = -1;
    for (ItemPos j = break_item() + 1; j <= last_item(); ++j)
        if ((k == -1 || item(k).p < item(j).p) && w + item(j).w <= capacity())
            k = j;
    FFOT_LOG(info, "gamma1 " << k << std::endl);
    return k;

}

ItemPos Instance::gamma2(FFOT_DBG(Info& info)) const
{
    Weight w = break_weight() + item(break_item()).w;
    ItemPos k = -1;
    for (ItemPos j = first_item(); j < break_item(); ++j)
        if ((k == -1 || item(k).p > item(j).p) && w - item(j).w <= capacity())
            k = j;
    FFOT_LOG(info, "gamma2 " << k << std::endl);
    return k;
}

ItemPos Instance::beta1(FFOT_DBG(Info& info)) const
{
    ItemPos k = -1;
    for (ItemPos j = break_item() + 1; j <= last_item(); ++j)
        if ((k == -1 || item(k).p < item(j).p) && break_weight() + item(j).w <= capacity())
            k = j;
    FFOT_LOG(info, "beta1 " << k << std::endl);
    return k;

}

ItemPos Instance::beta2(FFOT_DBG(Info& info)) const
{
    ItemPos k = -1;
    if (break_item() + 1 <= last_item()) {
        Weight w = break_weight() + item(break_item()).w + item(break_item() + 1).w;
        for (ItemPos j = first_item(); j < break_item(); ++j)
            if ((k == -1 || item(k).p > item(j).p) && w - item(j).w <= capacity())
                k = j;
    }
    FFOT_LOG(info, "beta2 " << k << std::endl);
    return k;
}

Profit Instance::optimum() const
{
    if (optimal_solution() == nullptr)
        return -1;
    return optimal_solution()->profit();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void Instance::compute_break_item(FFOT_DBG(Info& info))
{
    FFOT_LOG_FOLD_START(info, "compute_break_item f " << first_item() << std::endl);
    FFOT_LOG(info, "reduced solution " << reduced_solution()->to_string_items() << std::endl);

    if (break_solution_ == nullptr) {
        break_solution_ = std::unique_ptr<Solution>(new Solution(*reduced_solution()));
    } else {
        *break_solution_ = *reduced_solution();
    }
    for (b_ = first_item(); b_ <= last_item(); ++b_) {
        if (item(b_).w > break_solution_->remaining_capacity())
            break;
        break_solution_->set(b_, true);
    }
    FFOT_LOG(info, "break solution " << break_solution_->to_string_items() << std::endl);
    FFOT_LOG_FOLD_END(info, "compute_break_item b " << b_);
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

Weight Instance::reduced_capacity() const
{
    return (reduced_solution() == nullptr)?
        capacity(): capacity() - reduced_solution()->weight();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void Instance::sort(FFOT_DBG(Info& info))
{
    FFOT_LOG_FOLD_START(info, "sort" << std::endl);
    if (sort_status() == 2) {
        FFOT_LOG_FOLD_END(info, "sort already sorted");
        return;
    }
    if (reduced_solution() == nullptr)
        reduced_solution_ = std::unique_ptr<Solution>(new Solution(*this));
    sort_status_ = 2;
    if (reduced_number_of_items() > 1)
        std::sort(items_.begin()+first_item(), items_.begin()+last_item()+1,
                [](const Item& i1, const Item& i2) {
                return i1.p * i2.w > i2.p * i1.w;});

    compute_break_item(FFOT_DBG(info));
    FFOT_LOG_FOLD_END(info, "sort");
}

void Instance::remove_big_items(FFOT_DBG(Info& info))
{
    FFOT_LOG_FOLD_START(info, "remove_big_items" << std::endl);
    if (b_ != -1 && item(b_).w > reduced_capacity())
        b_ = -1;

    if (sort_status() == 2) {
        std::vector<Item> not_fixed;
        std::vector<Item> fixed_0;
        for (ItemPos j = first_item(); j <= last_item(); ++j) {
            if (item(j).w > reduced_capacity()) {
                fixed_0.push_back(item(j));
                FFOT_LOG(info, "remove " << j << " (" << item(j) << ")" << std::endl);
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

        if (b_ == -1)
            compute_break_item(FFOT_DBG(info));
    } else {
        for (ItemPos j = first_item(); j <= last_item();) {
            if (item(j).w > reduced_capacity()) {
                swap(j, l_);
                l_--;
            } else {
                j++;
            }
        }

        sort_status_ = 0;
        sort_partially(FFOT_DBG(info));
    }
    FFOT_LOG_FOLD_END(info, "remove_big_items");
}

std::pair<ItemPos, ItemPos> Instance::partition(ItemPos f, ItemPos l FFOT_DBG(FFOT_COMMA Info& info))
{
    FFOT_LOG_FOLD_START(info, "partition f " << f << " l " << l << std::endl);
    ItemPos pivot = f + 1 + rand() % (l - f); // Select pivot
    Weight w = item(pivot).w;
    Profit p = item(pivot).p;
    FFOT_LOG(info, "pivot " << pivot
            << " w_pivot " << w << " p_pivot " << p
            << " e_pivot " << item(pivot).efficiency() << std::endl);

    // Partition
    swap(pivot, l);
    ItemPos j = f;
    while (j <= l) {
        if (item(j).p * w > p * item(j).w) {
            swap(j, f);
            f++;
            j++;
        } else if (item(j).p * w < p * item(j).w) {
            swap(j, l);
            l--;
        } else {
            j++;
        }
    }

    // | | | | | | | | | | | | | | | | | | | | |
    //          f       j           l
    // -------|                       |-------
    // > pivot                         < pivot

    FFOT_LOG(info, "f " << f << " l " << l << std::endl);

    FFOT_LOG_FOLD_END(info, "partition");
    return {f,l};
}

void Instance::sort_partially(FFOT_DBG(Info& info FFOT_COMMA) ItemIdx limit)
{
    FFOT_LOG_FOLD_START(info, "sort_partially limit " << limit << std::endl);

    if (sort_status_ >= 1) {
        FFOT_LOG_FOLD_END(info, "sort_partially already sorted");
        return;
    }

    if (reduced_solution() == nullptr)
        reduced_solution_ = std::unique_ptr<Solution>(new Solution(*this));

    srand(0);
    int_right_.clear();
    int_left_.clear();

    // Quick sort like algorithm
    ItemPos f = first_item();
    ItemPos l = last_item();
    Weight c = reduced_capacity();
    while (f < l) {
        FFOT_LOG(info, "f " << f << " l " << l << std::endl);
        if (l - f < limit) {
            std::sort(items_.begin() + f, items_.begin() + l + 1,
                    [](const Item& i1, const Item& i2) {
                    return i1.p * i2.w > i2.p * i1.w;});
            break;
        }

        std::pair<ItemPos, ItemPos> fl = partition(f, l FFOT_DBG(FFOT_COMMA info));
        ItemPos w = 0;
        for (ItemPos k = f; k < fl.first; ++k)
            w += item(k).w;

        if (w > c) {
            if (fl.second + 1 <= l)
                int_right_.push_back({fl.second + 1, l});
            int_right_.push_back({fl.first, fl.second});
            l = fl.first - 1;
            continue;
        }

        for (ItemPos k = fl.first; k <= fl.second; ++k)
            w += item(k).w;
        if (w > c) {
            break;
        } else {
            c -= w;
            if (f <= fl.first - 1)
                int_left_.push_back({f, fl.first - 1});
            int_left_.push_back({fl.first, fl.second});
            f = fl.second + 1;
        }
    }

    sort_status_ = 1;

    compute_break_item(FFOT_DBG(info));

    if (f < b_)
        int_left_.push_back({f, b_ - 1});
    if (b_ < l)
        int_right_.push_back({b_ + 1, l});
    s_prime_ = b_;
    t_prime_ = b_;
    s_init_ = b_;
    t_init_ = b_;

    FFOT_DBG(assert(check_partialsort(info)));
    FFOT_LOG_FOLD(info, *this);
    FFOT_LOG_FOLD_END(info, "sort_partially");
}

void Instance::sort_right(Profit lb FFOT_DBG(FFOT_COMMA Info& info))
{
    FFOT_LOG_FOLD_START(info, "sort_right lb " << lb << std::endl);
    Interval in = int_right_.back();
    int_right_.pop_back();
    ItemPos k = t_prime();
    FFOT_LOG(info, "in.f " << in.f << " in.l " << in.l << std::endl);
    for (ItemPos j = in.f; j <= in.l; ++j) {
        FFOT_LOG(info, "j " << j << " (" << item(j) << ")");
        Profit p = break_solution()->profit() + item(break_item()).p + item(j).p;
        Weight r = break_capacity() - item(break_item()).w - item(j).w;
        assert(r < 0);
        Profit ub = upper_bound_dembo_rev(*this, break_item(), p, r);
        FFOT_LOG(info, " ub " << ub);
        if (item(j).w <= reduced_capacity() && ub > lb) {
            k++;
            swap(k, j);
            FFOT_LOG(info, " swap j " << j << " k " << k << std::endl);
        } else {
            FFOT_LOG(info, " set 0" << std::endl);
        }
    }
    std::sort(items_.begin() + t_prime() + 1, items_.begin() + k + 1,
            [](const Item& i1, const Item& i2) {
            return i1.p * i2.w > i2.p * i1.w;});
    t_prime_ = k;
    if (int_right_.size() == 0) {
        l_ = t_prime();
        FFOT_LOG(info, "l_ " << l_ << std::endl);
    }
    if (first_item() >= s_prime() && last_item() <= t_prime()) {
        if (s_init_ == t_init_) {
            sort_status_ = 2;
        } else {
            sort_status_ = 0;
        }
    }
    FFOT_LOG_FOLD_END(info, "sort_right");
}

void Instance::sort_left(Profit lb FFOT_DBG(FFOT_COMMA Info& info))
{
    FFOT_LOG_FOLD_START(info, "sort_left lb " << lb << std::endl);
    FFOT_LOG(info, "s_prime " << s_prime() << std::endl);
    Interval in = int_left_.back();
    int_left_.pop_back();
    ItemPos k = s_prime();
    FFOT_LOG(info, "in.l " << in.f << " in.f " << in.l << " b " << break_item() << std::endl);
    for (ItemPos j = in.l; j >= in.f; --j) {
        FFOT_LOG(info, "j " << j << " (" << item(j) << ")");
        Profit p = break_solution()->profit() - item(j).p;
        Weight r = break_capacity() + item(j).w;
        assert(r > 0);
        Profit ub = upper_bound_dembo(*this, break_item(), p, r);
        FFOT_LOG(info, " ub " << ub);
        if (item(j).w <= reduced_capacity() && ub > lb) {
            k--;
            swap(k, j);
            FFOT_LOG(info, " swap j " << j << " k " << k << std::endl);
        } else {
            FFOT_LOG(info, " set 1" << std::endl);
            reduced_solution_->set(j, true);
        }
    }
    std::sort(items_.begin() + k, items_.begin() + s_prime(),
            [](const Item& i1, const Item& i2) {
            return i1.p * i2.w > i2.p * i1.w;});
    s_prime_ = k;
    FFOT_LOG(info, "s_prime " << s_prime() << std::endl);
    if (int_left_.size() == 0) {
        f_ = s_prime();
        FFOT_LOG(info, "f_ " << f_ << std::endl);
    }
    if (first_item() >= s_prime() && last_item() <= t_prime()) {
        if (s_init_ == t_init_) {
            sort_status_ = 2;
        } else {
            sort_status_ = 0;
        }
    }
    FFOT_LOG_FOLD_END(info, "sort_left");
}

ItemPos Instance::bound_item_left(ItemPos s, Profit lb FFOT_DBG(FFOT_COMMA Info& info))
{
    FFOT_LOG_FOLD_START(info, "bound_item_left s " << s << std::endl);
    while (s < s_prime() && int_left().size() > 0)
        sort_left(lb FFOT_DBG(FFOT_COMMA info));
    if (s < first_item()) {
        FFOT_LOG_FOLD_END(info, "bound_item_left " << first_item() + 1);
        return first_item() - 1;
    } else if (s >= s_init()) {
        FFOT_LOG_FOLD_END(info, "bound_item_left " << break_item());
        return break_item();
    } else {
        FFOT_LOG_FOLD_END(info, "bound_item_left " << s);
        return s;
    }
}

ItemPos Instance::bound_item_right(ItemPos t, Profit lb FFOT_DBG(FFOT_COMMA Info& info))
{
    FFOT_LOG_FOLD_START(info, "bound_item_right t " << t << std::endl);
    while (t > t_prime() && int_right().size() > 0)
        sort_right(lb FFOT_DBG(FFOT_COMMA info));
    if (t >= last_item() + 1) {
        FFOT_LOG_FOLD_END(info, "bound_item_right " << last_item() + 1);
        return last_item() + 1;
    } else if (t <= t_init()) {
        FFOT_LOG_FOLD_END(info, "bound_item_right " << break_item());
        return break_item();
    } else {
        FFOT_LOG_FOLD_END(info, "bound_item_right " << t);
        return t;
    }
}

void Instance::add_item_to_core(ItemPos s, ItemPos t, ItemPos j FFOT_DBG(FFOT_COMMA Info& info))
{
    FFOT_LOG_FOLD_START(info, "add_item_to_initial_core j " << j << std::endl);
    if (j == -1) {
        FFOT_LOG_FOLD_END(info, "add_item_to_initial_core j = -1");
        return;
    }
    FFOT_LOG(info, "item " << item(j) << std::endl);
    if (s <= j && j <= t) {
        FFOT_LOG_FOLD_END(info, "add_item_to_initial_core j already in core");
        return;
    }

    Item it_tmp = items_[j];
    if (j < break_item()) {
        FFOT_LOG(info, "step 1" << std::endl);
        if (j < s_second()) {
            for (auto in = int_left_.begin(); in != int_left_.end();) {
                if (in->l < j) {
                    in++;
                    continue;
                }
                FFOT_LOG(info, "move " << in->l << " (" << item(in->l) << ") to " << j << std::endl);
                items_[j] = items_[in->l];
                j = in->l;
                in->l--;
                if (std::next(in) != int_left_.end())
                    std::next(in)->f--;
                if (in->f > in->l) {
                    in = int_left_.erase(in);
                } else {
                    in++;
                }
            }
            s_prime_--;
        }

        FFOT_LOG(info, "step 2" << std::endl);
        if (j < s_prime()) {
            FFOT_LOG(info, "move " << s_prime() << " (" << item(s_prime()) << ") to " << j << std::endl);
            items_[j] = items_[s_prime()];
            j = s_prime();
        }

        FFOT_LOG(info, "step 3" << std::endl);
        while (j != s) {
            FFOT_LOG(info, "move " << j + 1 << " (" << item(j + 1) << ") to " << j << std::endl);
            items_[j] = items_[j + 1];
            j++;
        }

        items_[s] = it_tmp;

        s_init_--;
    } else {
        FFOT_LOG(info, "step 1" << std::endl);
        if (j > t_second()) {
            for (auto in = int_right_.begin(); in != int_right_.end();) {
                FFOT_LOG(info, "interval " << *in << std::endl);
                if (in->f > j) {
                    in++;
                    continue;
                }
                FFOT_LOG(info, "move " << in->f << " (" << item(in->f) << ") to " << j << std::endl);
                items_[j] = items_[in->f];
                j = in->f;
                in->f++;
                if (std::next(in) != int_right_.end())
                    std::next(in)->l++;
                if (in->f > in->l) {
                    in = int_right_.erase(in);
                } else {
                    in++;
                }
            }
            t_prime_++;
        }

        FFOT_LOG(info, "step 2" << std::endl);
        if (j > t_prime()) {
            FFOT_LOG(info, "move " << t_prime() << " (" << item(t_prime()) << ") to " << j << std::endl);
            items_[j] = items_[t_prime()];
            j = t_prime();
        }

        FFOT_LOG(info, "step 3" << std::endl);
        while (j != t) {
            FFOT_LOG(info, "move " << j - 1 << " (" << item(j - 1) << ") to " << j << std::endl);
            items_[j] = items_[j - 1];
            j--;
        }

        items_[t] = it_tmp;

        t_init_++;
    }
    sort_status_ = 1;

    FFOT_LOG_FOLD(info, *this);
    FFOT_LOG_FOLD_END(info, "add_item_to_initial_core");
}

bool Instance::check_partialsort(FFOT_DBG(Info& info)) const
{
    FFOT_LOG_FOLD(info, *this);

    if (reduced_number_of_items() == 0) {
        if (break_item() != last_item() + 1) {
            std::cout << 0 << std::endl;
            FFOT_LOG_FOLD_END(info, "b " << break_item() << " != l + 1 " << last_item() + 1);
            return false;
        }
        return true;
    }

    Weight w_total = reduced_solution()->weight();
    for (ItemPos j = first_item(); j <= last_item(); ++j)
        w_total += item(j).w;
    if (w_total <= capacity()) {
        if (break_item() != last_item() + 1) {
            std::cout << 1 << std::endl;
            FFOT_LOG_FOLD_END(info, "w_red " << reduced_solution()->weight()
                    << " w_tot " << w_total
                    << " c_tot " << capacity()
                    << " b " << break_item()
                    << " != l + 1 " << last_item() + 1);
            return false;
        }
        return true;
    }

    if (break_item() < 0 || break_item() >= number_of_items()) {
        std::cout << 2 << std::endl;
        FFOT_LOG_FOLD_END(info, "b " << break_item());
        return false;
    }
    if (break_solution()->weight() > capacity()) {
        std::cout << *this << std::endl;
        std::cout << 3 << std::endl;
        FFOT_LOG_FOLD_END(info, "wbar " << break_solution()->weight() << " c " << capacity());
        return false;
    }
    if (break_solution()->weight() + item(break_item()).w <= capacity()) {
        std::cout << 4 << std::endl;
        FFOT_LOG_FOLD_END(info, "wbar + wb " << break_solution()->weight() + item(break_item()).w << " c " << capacity());
        return false;
    }
    for (ItemPos j = first_item(); j < break_item(); ++j) {
        if (item(j).p * item(break_item()).w < item(break_item()).p * item(j).w) {
            std::cout << 5 << std::endl;
            FFOT_LOG_FOLD_END(info, "j " << j << "(" << item(j) << ") b " << break_item() << "(" << item(break_item()) << ")");
            return false;
        }
    }
    for (ItemPos j = break_item() + 1; j <= last_item(); ++j) {
        if (item(j).p * item(break_item()).w > item(break_item()).p * item(j).w) {
            std::cout << 6 << std::endl;
            FFOT_LOG_FOLD_END(info, "j " << j << "(" << item(j) << ") b " << break_item() << "(" << item(break_item()) << ")");
            return false;
        }
    }

    if (int_left().size() != 0) {
        if (int_left().back().l > s_prime() - 1) {
            std::cout << 7 << std::endl;
            FFOT_LOG_FOLD_END(info, "int_left().back().l " << int_left().back().l << " s " << s_prime());
            return false;
        }
        for (auto it = int_left().begin(); it != std::prev(int_left().end()); ++it)
            if (it->l != std::next(it)->f - 1) {
                std::cout << 8 << std::endl;
                return false;
            }
        Effciency emin_prev = std::numeric_limits<Effciency>::infinity();
        for (auto i: int_left_) {
            if (i.f > i.l) {
                std::cout << 9 << std::endl;
                return false;
            }
            Effciency emax = 0;
            Effciency emin = std::numeric_limits<Effciency>::infinity();
            for (ItemPos j = i.f; j <= i.l; ++j) {
                if (emax < item(j).efficiency())
                    emax = item(j).efficiency();
                if (emin > item(j).efficiency())
                    emin = item(j).efficiency();
            }
            if (emax > emin_prev) {
                std::cout << 10 << std::endl;
                return false;
            }
            emin_prev = emin;
        }
    }
    if (int_right().size() != 0) {
        if (int_right_.back().f < t_prime() + 1) {
            std::cout << 11 << std::endl;
            return false;
        }
        for (auto it = int_right().rbegin(); it != std::prev(int_right().rend()); ++it)
            if (it->l != std::next(it)->f - 1) {
                std::cout << 12 << std::endl;
                return false;
            }
        Effciency emax_prev = 0;
        for (auto i: int_right_) {
            if (i.f > i.l) {
                std::cout << 13 << std::endl;
                return false;
            }
            Effciency emax = 0;
            Effciency emin = std::numeric_limits<Effciency>::infinity();
            for (ItemPos j = i.f; j <= i.l; ++j) {
                if (emax < item(j).efficiency())
                    emax = item(j).efficiency();
                if (emin > item(j).efficiency())
                    emin = item(j).efficiency();
            }
            if (emin < emax_prev) {
                std::cout << 14 << std::endl;
                return false;
            }
            emax_prev = emax;
        }
    }
    return true;
}

void Instance::init_combo_core(FFOT_DBG(Info& info))
{
    FFOT_LOG_FOLD_START(info, "init_combo_core" << std::endl);
    assert(sort_status_ >= 1);
    add_item_to_core(s_init_ - 1, t_init_ + 1, before_break_item(FFOT_DBG(info)) FFOT_DBG(FFOT_COMMA info));
    add_item_to_core(s_init_ - 1, t_init_ + 1, gamma1(FFOT_DBG(info)) FFOT_DBG(FFOT_COMMA info));
    add_item_to_core(s_init_ - 1, t_init_ + 1, gamma2(FFOT_DBG(info)) FFOT_DBG(FFOT_COMMA info));
    add_item_to_core(s_init_ - 1, t_init_ + 1, beta1(FFOT_DBG(info)) FFOT_DBG(FFOT_COMMA info));
    add_item_to_core(s_init_ - 1, t_init_ + 1, beta2(FFOT_DBG(info)) FFOT_DBG(FFOT_COMMA info));
    add_item_to_core(s_init_ - 1, t_init_ + 1, max_weight_item(FFOT_DBG(info)) FFOT_DBG(FFOT_COMMA info));
    add_item_to_core(s_init_ - 1, t_init_ + 1, min_weight_item(FFOT_DBG(info)) FFOT_DBG(FFOT_COMMA info));
    FFOT_DBG(assert(check_partialsort(info)));
    FFOT_LOG_FOLD_END(info, "init_combo_core");
}

void Instance::reduce1(Profit lb, Info& info)
{
    FFOT_LOG_FOLD_START(info, "reduce1 - lb " << lb << " b_ " << b_ << std::endl;);

    assert(b_ != l_+1);

    for (ItemIdx j = f_; j < b_;) {
        FFOT_LOG(info, "j " << j << " (" << item(j) << ") f_ " << f_);

        Profit ub = reduced_solution()->profit() + break_profit() - item(j).p
                + ((break_capacity() + item(j).w) * item(b_).p) / item(b_).w;
        FFOT_LOG(info, " ub " << ub);

        if (ub <= lb) {
            FFOT_LOG(info, " 1" << std::endl);
            reduced_solution_->set(j, true);
            if (j != f_)
                swap(j, f_);
            f_++;
            if (reduced_capacity() < 0)
                return;
        } else {
            FFOT_LOG(info, " ?" << std::endl);
        }
        j++;
    }
    for (ItemPos j = l_; j > b_;) {
        FFOT_LOG(info, "j " << j << " (" << item(j) << ") l_ " << l_);

        Profit ub = reduced_solution()->profit() + break_profit() + item(j).p
                + ((break_capacity() - item(j).w) * item(b_).p) / item(b_).w;
        FFOT_LOG(info, " ub " << ub)

        if (ub <= lb) {
            FFOT_LOG(info, " 0" << std::endl);
            if (j != l_)
                swap(j, l_);
            l_--;
        } else {
            FFOT_LOG(info, " ?" << std::endl);
        }
        j--;
    }

    remove_big_items(FFOT_DBG(info));

    info.os() << "Reduction: " << lb << " - "
            << "n " << reduced_number_of_items() << "/" << number_of_items()
            << " ("  << ((double)reduced_number_of_items() / (double)number_of_items()) << ") -"
            << " c " << ((double)reduced_capacity()    / (double)capacity()) << std::endl;
    FFOT_LOG(info, "n " << reduced_number_of_items() << "/" << number_of_items() << std::endl);
    FFOT_LOG(info, "c " << reduced_capacity() << "/" << capacity() << std::endl);
    FFOT_LOG_FOLD_END(info, "reduce1");
}

ItemPos Instance::ub_item(const std::vector<Item>& isum, Item item) const
{
    assert(sort_status() == 2);
    auto s = std::upper_bound(isum.begin() + f_, isum.begin() + l_ + 1, item,
            [](const Item& i1, const Item& i2) { return i1.w < i2.w;});
    if (s == isum.begin() + l_ + 1)
        return l_ + 1;
    return s->j - 1;
}

void Instance::reduce2(Profit lb, Info& info)
{
    FFOT_LOG_FOLD_START(info, "Reduce 2: lb " << lb << " b_ " << b_ << std::endl);
    assert(sort_status() == 2);

    std::vector<Item> isum = get_isum();

    std::vector<Item> not_fixed;
    std::vector<Item> fixed_1;
    std::vector<Item> fixed_0;

    for (ItemPos j = f_; j <= b_; ++j) {
        FFOT_LOG(info, "j " << j << " (" << item(j) << ")");
        Item ubitem;
        ubitem.j = 0;
        ubitem.w = capacity() + item(j).w;
        ubitem.p = 0;
        ItemPos bb = ub_item(isum, ubitem);
        FFOT_LOG(info, " bb " << bb);
        Profit ub = 0;
        if (bb == last_item() + 1) {
            ub = isum[last_item()+1].p - item(j).p;
            FFOT_LOG(info, " ub " << ub);
        } else if (bb == last_item()) {
            Profit ub1 = isum[bb  ].p - item(j).p;
            Profit ub2 = isum[bb+1].p - item(j).p + ((capacity() + item(j).w - isum[bb+1].w) * item(bb-1).p + 1) / item(bb-1).w - 1;
            ub = (ub1 > ub2)? ub1: ub2;
            FFOT_LOG(info, " ub1 " << ub1 << " ub2 " << ub2 << " ub " << ub);
        } else {
            Profit ub1 = isum[bb  ].p - item(j).p + ((capacity() + item(j).w - isum[bb  ].w) * item(bb+1).p    ) / item(bb+1).w;
            Profit ub2 = isum[bb+1].p - item(j).p + ((capacity() + item(j).w - isum[bb+1].w) * item(bb-1).p + 1) / item(bb-1).w - 1;
            ub = (ub1 > ub2)? ub1: ub2;
            FFOT_LOG(info, " ub1 " << ub1 << " ub2 " << ub2 << " ub " << ub);
        }
        if (ub <= lb) {
            FFOT_LOG(info, " 1" << std::endl);
            reduced_solution_->set(j, true);
            fixed_1.push_back(item(j));
            if (reduced_capacity() < 0)
                return;
        } else {
            FFOT_LOG(info, " ?" << std::endl);
            if (j != b_)
                not_fixed.push_back(item(j));
        }
    }
    for (ItemPos j = b_; j <= l_; ++j) {
        if (j == b_ && !fixed_1.empty() && fixed_1.back().j == item(b_).j)
            continue;
        FFOT_LOG(info, "j " << j << " (" << item(j) << ")");

        Item ubitem;
        ubitem.j = 0;
        ubitem.w = capacity() - item(j).w;
        ubitem.p = 0;
        ItemPos bb = ub_item(isum, ubitem);
        FFOT_LOG(info, " bb " << bb);

        Profit ub = 0;
        if (bb == last_item() + 1) {
            ub = isum[last_item()+1].p + item(j).p;
            FFOT_LOG(info, " ub " << ub);
        } else if (bb == last_item()) {
            Profit ub1 = isum[bb  ].p + item(j).p;
            Profit ub2 = isum[bb+1].p + item(j).p + ((capacity() - item(j).w - isum[bb+1].w) * item(bb-1).p + 1) / item(bb-1).w - 1;
            ub = (ub1 > ub2)? ub1: ub2;
            FFOT_LOG(info, " ub1 " << ub1 << " ub2 " << ub2 << " ub " << ub);
        } else if (bb == 0) {
            ub = ((capacity() + item(j).w) * item(bb).p) / item(bb).w;
            FFOT_LOG(info, " ub " << ub);
        } else {
            Profit ub1 = isum[bb  ].p + item(j).p + ((capacity() - item(j).w - isum[bb  ].w) * item(bb+1).p) / item(bb+1).w;
            Profit ub2 = isum[bb+1].p + item(j).p + ((capacity() - item(j).w - isum[bb+1].w) * item(bb-1).p + 1) / item(bb-1).w - 1;
            ub = (ub1 > ub2)? ub1: ub2;
            FFOT_LOG(info, " ub1 " << ub1 << " ub2 " << ub2 << " ub " << ub);
        }

        if (ub <= lb) {
            FFOT_LOG(info, " 0" << std::endl);
            fixed_0.push_back(item(j));
        } else {
            FFOT_LOG(info, " ?" << std::endl);
            not_fixed.push_back(item(j));
        }
    }

    ItemPos j = not_fixed.size();
    ItemPos j0 = fixed_0.size();
    ItemPos j1 = fixed_1.size();
    FFOT_LOG(info, "j " << j << " j0 " << j0 << " j1 " << j1 << " n " << reduced_number_of_items() << std::endl);

    std::copy(fixed_1.begin(), fixed_1.end(), items_.begin()+f_);
    std::copy(not_fixed.begin(), not_fixed.end(), items_.begin()+f_+j1);
    std::copy(fixed_0.begin(), fixed_0.end(), items_.begin()+f_+j1+j);
    f_ += j1;
    l_ -= j0;

    remove_big_items(FFOT_DBG(info));
    compute_break_item(FFOT_DBG(info));

    info.os() << "Reduction: " << lb << " - "
            << "n " << reduced_number_of_items() << "/" << number_of_items()
            << " ("  << ((double)reduced_number_of_items() / (double)number_of_items()) << ") -"
            << " c " << ((double)reduced_capacity()    / (double)capacity()) << std::endl;
    FFOT_LOG(info, "n " << reduced_number_of_items() << "/" << number_of_items() << std::endl);
    FFOT_LOG(info, "c " << reduced_capacity() << "/" << capacity() << std::endl);
    FFOT_LOG(info, "reduced solution " << reduced_solution()->to_string_items() << std::endl);
    FFOT_LOG_FOLD_END(info, "reduce2");
}

void Instance::set_first_item(ItemPos k FFOT_DBG(FFOT_COMMA Info& info))
{
    FFOT_LOG_FOLD_START(info, "set_first_item k " << k << std::endl);
    assert(k >= f_);
    for (ItemPos j = f_; j < k; ++j) {
        FFOT_LOG(info, "set " << j << " (" << item(j) << ")" << std::endl);
        reduced_solution_->set(j, true);
    }
    f_ = k;
    FFOT_LOG_FOLD_END(info, "set_first_item");
}

void Instance::set_last_item(ItemPos k)
{
    assert(k <= l_);
    l_ = k;
}

void Instance::fix(const std::vector<int> vec FFOT_DBG(FFOT_COMMA Info& info))
{
    FFOT_LOG_FOLD_START(info, "fix");
    FFOT_DBG(
            for (int i: vec)
                FFOT_LOG(info, " " << i);
            FFOT_LOG(info, std::endl);
    )
    FFOT_LOG(info, "reduced solution " << reduced_solution()->to_string_items() << std::endl);

    std::vector<Item> not_fixed;
    std::vector<Item> fixed_1;
    std::vector<Item> fixed_0;
    for (ItemPos j = f_; j <= l_; ++j) {
        if (vec[j] == 0) {
            not_fixed.push_back(item(j));
        } else if (vec[j] == 1) {
            FFOT_LOG(info, "fix " << j << " (" << item(j) << ") 1" << std::endl);
            fixed_1.push_back(item(j));
            reduced_solution_->set(j, true);
        } else {
            assert(vec[j] == -1);
            FFOT_LOG(info, "fix " << j << " (" << item(j) << ") 0" << std::endl);
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
    FFOT_LOG(info, "reduced solution " << reduced_solution()->to_string_items() << std::endl);

    remove_big_items(FFOT_DBG(info));

    if (sort_status() == 1) {
        sort_status_ = 0;
        sort_partially(FFOT_DBG(info));
    } else {
        compute_break_item(FFOT_DBG(info));
    }

    FFOT_LOG(info, "reduced solution " << reduced_solution()->to_string_items() << std::endl);
    FFOT_LOG_FOLD_END(info, "fix");
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void Instance::surrogate(Weight multiplier, ItemIdx bound FFOT_DBG(FFOT_COMMA Info& info))
{
    surrogate(multiplier, bound, first_item() FFOT_DBG(FFOT_COMMA info));
}

void Instance::surrogate(Weight multiplier, ItemIdx bound, ItemPos first FFOT_DBG(FFOT_COMMA Info& info))
{
    break_solution_->clear();
    if (optimal_solution_ != nullptr)
        optimal_solution_ = nullptr;
    f_ = first;
    l_ = last_item();
    for (ItemIdx j = f_; j <= l_; ++j)
        reduced_solution_->set(j, false);
    bound -= reduced_solution_->number_of_items();
    for (ItemIdx j = f_; j <= l_; ++j) {
        items_[j].w += multiplier;
        if (items_[j].w <= 0) {
            reduced_solution_->set(j, true);
            swap(j, f_);
            f_++;
        }
    }
    capacity_ += multiplier * bound;
    if (capacity_ <= reduced_solution()->weight())
        capacity_ = reduced_solution()->weight();

    sort_status_ = 0;
    sort_partially(FFOT_DBG(info));
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

std::ostream& knapsacksolver::operator<<(std::ostream &os, const Interval& interval)
{
    os << "[" << interval.f << "," << interval.l << "]";
    return os;
}

std::ostream& knapsacksolver::operator<<(std::ostream& os, const Item& it)
{
    os << "j " << it.j << " w " << it.w << " p " << it.p << " e " << it.efficiency();
    return os;
}

std::ostream& knapsacksolver::operator<<(std::ostream& os, const Instance& instance)
{
    os
        <<  "n_total " << instance.number_of_items()
        << " c_total "   << instance.capacity()
        << " opt " << instance.optimum()
        << std::endl;
    if (instance.reduced_solution() != nullptr)
        os
            <<  "n " << instance.reduced_number_of_items() << " c " << instance.reduced_capacity()
            << " f " << instance.first_item() << " l " << instance.last_item()
            << " p_red " << instance.reduced_solution()->profit()
            << std::endl;
    if (instance.break_solution() != nullptr)
        os << "b " << instance.break_item()
            << " wsum " << instance.break_weight()
            << " psum " << instance.break_profit()
            << " p_break " << instance.break_solution()->profit()
            << std::endl;
    os << "sort_status " << instance.sort_status() << std::endl;

    os << "left";
    for (Interval in: instance.int_left())
        os << " " << in;
    os << std::endl;
    os << "right";
    for (Interval in: instance.int_right())
        os << " " << in;
    os << std::endl;

    os << std::left << std::setw(8) << "pos";
    os << std::left << std::setw(8) << "j";
    os << std::left << std::setw(12) << "w";
    os << std::left << std::setw(12) << "p";
    os << std::left << std::setw(12) << "eff";
    os << std::left << std::setw(4) << "b";
    os << std::left << std::setw(4) << "opt";
    os << std::left << std::setw(4) << "red";
    os << std::left << std::setw(4) << "b/f/l";
    os << std::endl;

    for (ItemPos j = 0; j < instance.number_of_items(); ++j) {
        const Item& it = instance.item(j);
        os << std::left << std::setw(8) << j;
        os << std::left << std::setw(8) << it.j;
        os << std::left << std::setw(12) << it.w;
        os << std::left << std::setw(12) << it.p;
        os << std::left << std::setw(12) << it.efficiency();

        os << std::left << std::setw(4);
        if (instance.break_solution() != nullptr)
             os << instance.break_solution()->contains(j);

        os << std::left << std::setw(4);
        if (instance.optimal_solution() != nullptr) {
            os << instance.optimal_solution()->contains(j);
        } else {
            os << ".";
        }

        os << std::left << std::setw(4);
        if (instance.reduced_solution() != nullptr) {
            os << instance.reduced_solution()->contains(j);
        } else {
            os << ".";
        }

        if (j == instance.break_item())
            os << "b";
        if (j == instance.first_item())
            os << "f";
        if (j == instance.last_item())
            os << "l";
        if (j == instance.s_prime())
            os << "s'";
        if (j == instance.t_prime())
            os << "t'";
        if (j == instance.s_second())
            os << "s''";
        if (j == instance.t_second())
            os << "t''";

        os << std::endl;
    }
    return os;
}

void Instance::plot(std::string output_path)
{
    std::ofstream file(output_path);
    if (!file.good()) {
        throw std::runtime_error(
                "Unable to open file \"" + output_path + "\".");
    }

    file << "w p" << std::endl;
    for (ItemIdx i = 0; i < reduced_number_of_items(); ++i)
        file << item(i).w << " " << item(i).p << std::endl;
    file.close();
}

void Instance::write(std::string instance_path)
{
    std::ofstream file(instance_path);
    if (!file.good()) {
        throw std::runtime_error(
                "Unable to open file \"" + instance_path + "\".");
    }

    file << number_of_items() << " " << capacity() << std::endl << std::endl;
    for (ItemIdx i = 0; i < number_of_items(); ++i)
        file << item(i).w << " " << item(i).p << std::endl;
    file.close();
}

void Instance::plot_reduced(std::string output_path)
{
    std::ofstream file(output_path);
    if (!file.good()) {
        throw std::runtime_error(
                "Unable to open file \"" + output_path + "\".");
    }

    file << "w p" << std::endl;
    for (ItemIdx i = f_; i <= l_; ++i)
        file << item(i).w << " " << item(i).p << std::endl;
    file.close();
}

void Instance::write_reduced(std::string instance_path)
{
    std::ofstream file(instance_path);
    if (!file.good()) {
        throw std::runtime_error(
                "Unable to open file \"" + instance_path + "\".");
    }

    file << reduced_number_of_items() << " " << reduced_capacity() << std::endl << std::endl;
    for (ItemIdx i = f_; i <= l_; ++i)
        file << item(i).w << " " << item(i).p << std::endl;
    file.close();
}

void knapsacksolver::init_display(
        const Instance& instance,
        optimizationtools::Info& info)
{
    info.os()
            << "=====================================" << std::endl
            << "           Knapsack Solver           " << std::endl
            << "=====================================" << std::endl
            << std::endl
            << "Instance" << std::endl
            << "--------" << std::endl
            << "Number of items:  " << instance.number_of_items() << std::endl
            << "Capacity:         " << instance.capacity() << std::endl
            << std::endl;
}
