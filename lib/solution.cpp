#include "knapsack/lib/solution.hpp"

#include <iomanip>

using namespace knapsack;

Solution::Solution(const Instance& ins):
    instance_(ins),
    x_(ins.total_item_number(), 0)
{ }

Solution::Solution(const Instance& ins, std::string filepath):
    instance_(ins),
    x_(ins.total_item_number(), 0)
{
    if (filepath.empty())
        return;
    std::ifstream file(filepath);
    if (!file.good()) {
        std::cerr << "\033[31m" << "ERROR, unable to open file \"" << filepath << "\"" << "\033[0m" << std::endl;
        return;
    }

    int x = 0;
    for (ItemPos j=0; j<ins.total_item_number(); ++j) {
        file >> x;
        set(j, x);
    }
}

Solution::Solution(const Solution& sol):
    instance_(sol.instance_),
    k_(sol.k_),
    p_(sol.p_),
    w_(sol.w_),
    x_(sol.x_)
{ }

Solution& Solution::operator=(const Solution& sol)
{
    if (this != &sol) {
        if (&this->instance() == &sol.instance()) {
            k_ = sol.k_;
            p_ = sol.p_;
            w_ = sol.w_;
            x_ = sol.x_;
        } else {
            // Used to convert a solution of a surrogate instance to a
            // solution of its original instance.
            k_ = sol.k_;
            x_ = sol.x_;
            p_ = sol.p_;
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

void Solution::write_cert(std::string filepath)
{
    if (filepath.empty())
        return;
    std::ofstream cert(filepath);
    if (!cert.good()) {
        std::cerr << "\033[31m" << "ERROR, unable to open file \"" << filepath << "\"" << "\033[0m" << std::endl;
        assert(false);
        return;
    }

    cert << *this;
    cert.close();
}

std::ostream& knapsack::operator<<(std::ostream& os, const Solution& sol)
{
    for (ItemPos j=0; j<sol.instance().total_item_number(); ++j)
        os << sol.data()[j] << std::endl;
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

/********************************** Display ***********************************/

void knapsack::init_display(Profit lb, Profit ub, Info& info)
{
    VER(info, std::left << std::setw(10) << "T (ms)");
    VER(info, std::left << std::setw(14) << "LB");
    VER(info, std::left << std::setw(14) << "UB");
    VER(info, std::left << std::setw(10) << "GAP");
    VER(info, "");
    VER(info, std::endl);

    double t = round(info.elapsed_time() * 10000) / 10;
    std::string ub_str = (ub == -1)? "inf": std::to_string(ub);
    std::string gap_str = (ub == -1)? "inf": std::to_string(ub - lb);
    VER(info, std::left << std::setw(10) << t);
    VER(info, std::left << std::setw(14) << lb);
    VER(info, std::left << std::setw(14) << ub_str);
    VER(info, std::left << std::setw(10) << gap_str);
    VER(info, "" << std::endl);
}

void knapsack::update_lb(knapsack::Output& output, Profit lb_new, const std::stringstream& s, Info& info)
{
    if (output.lower_bound >= lb_new)
        return;

    info.output->mutex_sol.lock();

    if (output.lower_bound < lb_new) {
        output.lower_bound = lb_new;
        double t = round(info.elapsed_time() * 10000) / 10;
        std::string ub_str = (output.upper_bound == -1)? "inf": std::to_string(output.upper_bound);
        std::string gap_str = (output.upper_bound == -1)? "inf": std::to_string(output.upper_bound - output.lower_bound);
        VER(info, std::left << std::setw(10) << t);
        VER(info, std::left << std::setw(14) << output.lower_bound);
        VER(info, std::left << std::setw(14) << ub_str);
        VER(info, std::left << std::setw(10) << gap_str);
        VER(info, s.str() << std::endl);
    }

    info.output->mutex_sol.unlock();
}

void knapsack::update_sol(knapsack::Output& output, const Solution& sol, const std::stringstream& s, Info& info)
{
    if (!output.solution.feasible() || output.solution.profit() >= sol.profit())
        return;

    info.output->mutex_sol.lock();

    if (output.solution.profit() < sol.profit()) {
        output.solution = sol;
    }

    if (output.lower_bound < sol.profit()) {
        output.lower_bound = sol.profit();
        double t = round(info.elapsed_time() * 10000) / 10;
        std::string ub_str = (output.upper_bound == -1)? "inf": std::to_string(output.upper_bound);
        std::string gap_str = (output.upper_bound == -1)? "inf": std::to_string(output.upper_bound - output.lower_bound);
        VER(info, std::left << std::setw(10) << t);
        VER(info, std::left << std::setw(14) << output.lower_bound);
        VER(info, std::left << std::setw(14) << ub_str);
        VER(info, std::left << std::setw(10) << gap_str);
        VER(info, s.str() << std::endl);
    }

    info.output->mutex_sol.unlock();
}

void knapsack::update_ub(knapsack::Output& output, Profit ub_new, const std::stringstream& s, Info& info)
{
    if (output.upper_bound != -1 && output.upper_bound <= ub_new)
        return;

    info.output->mutex_sol.lock();

    if (output.upper_bound == -1 || output.upper_bound > ub_new) {
        output.upper_bound = ub_new;
        double t = round(info.elapsed_time() * 10000) / 10;
        VER(info, std::left << std::setw(10) << t);
        VER(info, std::left << std::setw(14) << output.lower_bound);
        VER(info, std::left << std::setw(14) << output.upper_bound);
        VER(info, std::left << std::setw(10) << output.upper_bound - output.lower_bound);
        VER(info, s.str() << std::endl);
    }

    info.output->mutex_sol.unlock();
}

void knapsack::algorithm_end(Profit upper_bound, Info& info)
{
    double t = round(info.elapsed_time() * 10000) / 10;
    PUT(info, "Bound", "Value", upper_bound);
    PUT(info, "Bound", "Time", t);
    VER(info, "---" << std::endl
            << "Bound: " << upper_bound << std::endl
            << "Time (ms): " << t << std::endl);
}

void knapsack::algorithm_end(const knapsack::Output& output, Info& info)
{
    double t = round(info.elapsed_time() * 10000) / 10;
    std::string ub_str = (output.upper_bound == -1)? "inf": std::to_string(output.upper_bound);
    std::string gap_str = (output.upper_bound == -1)? "inf": std::to_string(output.upper_bound - output.lower_bound);
    std::string sol_str = (output.solution.feasible() && output.solution.profit() == output.lower_bound)? "OK": "none";
    double gap = (output.lower_bound == 0 || output.upper_bound == -1)?
        std::numeric_limits<double>::infinity():
        (double)(10000 * (output.upper_bound - output.lower_bound) / output.lower_bound) / 100;
    PUT(info, "Solution", "Value", output.lower_bound);
    PUT(info, "Bound", "Value", output.upper_bound);
    PUT(info, "Solution", "Time", t);
    PUT(info, "Bound", "Time", t);
    VER(info, "---" << std::endl
            << "Value: " << output.lower_bound << std::endl
            << "Bound: " << ub_str << std::endl
            << "Gap: " << gap_str << std::endl
            << "Gap (%): " << gap << std::endl
            << "Solution: " << sol_str << std::endl
            << "Time (ms): " << t << std::endl);
}

