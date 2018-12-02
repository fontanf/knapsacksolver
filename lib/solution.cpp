#include "knapsack/lib/solution.hpp"

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
            p_ = 0;
            w_ = 0;
            k_ = solution.item_number();
            x_ = solution.data();
            for (ItemPos j=0; j<instance().total_item_number(); ++j) {
                if (contains(j)) {
                    p_ += instance().item(j).p;
                    w_ += instance().item(j).w;
                }
            }
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
    assert(j >= 0 && j < instance().total_item_number());
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

void Solution::update(const Solution& sol, Info& info, Cpt& solution_number, Profit ub)
{
    assert(sol.profit() > profit() && sol.remaining_capacity() >= 0);
    *this = sol;
    double t = info.elapsed_time();
    std::string sol_str = "Solution" + std::to_string(solution_number);
    info.pt.put(sol_str + ".Value", sol.profit());
    info.pt.put(sol_str + ".Time", t);

    if (info.verbose()) {
        std::string s = STR3(--- Lower bound, solution_number) + STR4(- Value:, sol.profit());
        if (ub != -1)
            s += STR4(- Gap:, ub - sol.profit());
        s += STR4(- Time:, t) + "\n";
        info.verbose(s);
    }

    solution_number++;
    if (!info.write_only_at_the_end()) {
        info.write_ini();
        write_cert(info.cert_file());
    }
}

void Solution::update_ub(Profit& ub, Profit ub_new, Info& info, Cpt& ub_number, Profit lb)
{
    assert(ub == -1 || ub_new < ub);
    ub = ub_new;
    double t = info.elapsed_time();
    std::string sol_str = "UB" + std::to_string(ub_number);
    info.pt.put(sol_str + ".Value", ub);
    info.pt.put(sol_str + ".Time", t);

    if (info.verbose()) {
        std::string s = STR3(--- Upper bound, ub_number) + STR4(- Value:, ub);
        if (lb != -1)
            s += STR4(- Gap:, ub - lb);
        s += STR4(- Time:, t) + "\n";
        info.verbose(s);
    }

    ub_number++;
    if (!info.write_only_at_the_end()) {
        info.write_ini();
    }
}

void Solution::update_lb(Profit& lb, Profit lb_new, Info& info, Cpt& solution_number, Profit ub)
{
    assert(lb_new > lb);
    lb = lb_new;
    double t = info.elapsed_time();
    std::string sol_str = "Solution" + std::to_string(solution_number);
    info.pt.put(sol_str + ".Value", lb);
    info.pt.put(sol_str + ".Time", t);

    if (info.verbose()) {
        std::string s = STR3(--- Lower bound, solution_number) + STR4(- Value:, lb);
        if (ub != -1)
            s += STR4(- Gap:, ub - lb);
        s += STR4(- Time:, t) + "\n";
        info.verbose(s);
    }

    solution_number++;
    if (!info.write_only_at_the_end()) {
        info.write_ini();
    }
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
    for (ItemPos j=0; j<instance().total_item_number(); ++j)
        if (x_[j])
            s += std::to_string(j) + ",";
    return s;
}

