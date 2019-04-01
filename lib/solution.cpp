#include "knapsack/lib/solution.hpp"

#include <iomanip>

using namespace knapsack;

Solution::Solution(const Instance& instance): instance_(instance),
    x_(std::vector<int>(instance.total_item_number(), 0)) { }

Solution::Solution(const Solution& solution):
    instance_(solution.instance()), k_(solution.item_number()),
    p_(solution.profit()), w_(solution.weight()), x_(solution.data()) { }

Solution& Solution::operator=(const Solution& solution)
{
    if (this != &solution) {
        if (&this->instance() == &solution.instance()) {
            k_ = solution.item_number();
            p_ = solution.profit();
            w_ = solution.weight();
            x_ = solution.data();
        } else {
            // Used to convert a solution of a surrogate instance to a
            // solution of its original instance.
            k_ = solution.item_number();
            x_ = solution.data();
            p_ = solution.profit();
            w_ = 0;
            for (ItemPos j=0; j<instance().total_item_number(); ++j)
                if (contains(j))
                    w_ += instance().item(j).w;
        }
    }
    return *this;
}

int Solution::contains(ItemPos j) const
{
    assert(j >= 0 && j < instance().total_item_number());
    assert(instance().item(j).j >= 0 && instance().item(j).j < instance().total_item_number());
    return x_[instance().item(j).j];
}

int Solution::contains_idx(ItemIdx j) const
{
    assert(j >= 0 && j < instance().total_item_number());
    return x_[j];
}

void Solution::set(ItemPos j, int b)
{
    assert(b == 0 || b == 1);
    assert(j >= 0);
    assert(j < instance().total_item_number());
    assert(instance().item(j).j >= 0 && instance().item(j).j < instance().total_item_number());
    if (contains(j) == b)
        return;
    if (b) {
        p_ += instance().item(j).p;
        w_ += instance().item(j).w;
        k_++;
    } else {
        p_ -= instance().item(j).p;
        w_ -= instance().item(j).w;
        k_--;
    }
    x_[instance().item(j).j] = b;
}

void Solution::clear()
{
    k_ = 0;
    p_ = 0;
    w_ = 0;
    std::fill(x_.begin(), x_.end(), 0);
}

void Solution::write_cert(std::string file)
{
    if (file != "") {
        std::ofstream cert;
        cert.open(file);
        cert << *this;
        cert.close();
    }
}

std::ostream& knapsack::operator<<(std::ostream& os, const Solution& solution)
{
    const Instance& instance = solution.instance();
    for (ItemPos j=0; j<instance.total_item_number(); ++j)
        os << solution.data()[j] << std::endl;
    return os;
}

std::string Solution::to_string_binary() const
{
    std::string s = "";
    for (ItemPos j=0; j<instance().total_item_number(); ++j)
        s += std::to_string(x_[j]);
    return s;
}

std::string Solution::to_string_binary_ordered() const
{
    std::string s = "";
    for (ItemPos j=0; j<instance().total_item_number(); ++j)
        s += std::to_string(x_[instance().item(j).j]);
    return s;
}

std::string Solution::to_string_items() const
{
    std::string s = "";
    for (ItemPos j=0; j<instance().total_item_number(); ++j) {
        if (x_[j]) {
            if (!s.empty())
                s += ",";
            s += std::to_string(j);
        }
    }
    return s;
}

void knapsack::init_display(Profit lb, Profit ub, Info& info)
{
    VER(info, std::left << std::setw(10) << "Time");
    VER(info, std::left << std::setw(12) << "LB");
    VER(info, std::left << std::setw(12) << "UB");
    VER(info, std::left << std::setw(12) << "GAP");
    VER(info, "");
    VER(info, std::endl);

    double t = round(info.elapsed_time() * 10000) / 10;
    VER(info, std::left << std::setw(10) << t);
    VER(info, std::left << std::setw(12) << lb);
    VER(info, std::left << std::setw(12) << ub);
    VER(info, std::left << std::setw(12) << ub - lb);
    VER(info, "" << std::endl);
}

void knapsack::update_lb(Profit& lb, Profit ub, Profit lb_new, const std::stringstream& s, Info& info)
{
    info.output->mutex_sol.lock();

    if (lb < lb_new) {
        lb = lb_new;
        double t = round(info.elapsed_time() * 10000) / 10;
        VER(info, std::left << std::setw(10) << t);
        VER(info, std::left << std::setw(12) << lb);
        VER(info, std::left << std::setw(12) << ub);
        VER(info, std::left << std::setw(12) << ub - lb);
        VER(info, s.str() << std::endl);
    }

    info.output->mutex_sol.unlock();
}

void knapsack::update_ub(Profit lb, Profit& ub, Profit ub_new, const std::stringstream& s, Info& info)
{
    info.output->mutex_sol.lock();

    if (ub > ub_new) {
        ub = ub_new;
        double t = round(info.elapsed_time() * 10000) / 10;
        VER(info, std::left << std::setw(10) << t);
        VER(info, std::left << std::setw(12) << lb);
        VER(info, std::left << std::setw(12) << ub);
        VER(info, std::left << std::setw(12) << ub - lb);
        VER(info, s.str() << std::endl);
    }

    info.output->mutex_sol.unlock();
}

void knapsack::update_sol(Solution& sol, Profit ub, const Solution& sol_new, const std::stringstream& s, Info& info)
{
    info.output->mutex_sol.lock();

    if (sol.profit() < sol_new.profit()) {
        sol = sol_new;
        double t = round(info.elapsed_time() * 10000) / 10;
        VER(info, std::left << std::setw(10) << t);
        VER(info, std::left << std::setw(12) << sol.profit());
        VER(info, std::left << std::setw(12) << ub);
        VER(info, std::left << std::setw(12) << ub - sol.profit());
        VER(info, s.str() << std::endl);
    }

    info.output->mutex_sol.unlock();
}

