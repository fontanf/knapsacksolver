#include "knapsacksolver/solution.hpp"

#include <iomanip>

using namespace knapsacksolver;

Solution::Solution(const Instance& instance):
    instance_(instance),
    x_(instance.item_number(), 0)
{ }

Solution::Solution(const Instance& instance, std::string filepath):
    instance_(instance),
    x_(instance.item_number(), 0)
{
    if (filepath.empty())
        return;
    std::ifstream file(filepath);
    if (!file.good()) {
        std::cerr << "\033[31m" << "ERROR, unable to open file \"" << filepath << "\"" << "\033[0m" << std::endl;
        return;
    }

    int x = 0;
    for (ItemPos j = 0; j < instance.item_number(); ++j) {
        file >> x;
        set(j, x);
    }
}

Solution::Solution(const Solution& solution):
    instance_(solution.instance_),
    item_number_(solution.item_number_),
    profit_(solution.profit_),
    weight_(solution.weight_),
    x_(solution.x_)
{ }

Solution& Solution::operator=(const Solution& solution)
{
    if (this != &solution) {
        if (&this->instance() == &solution.instance()) {
            item_number_ = solution.item_number_;
            profit_ = solution.profit_;
            weight_ = solution.weight_;
            x_ = solution.x_;
        } else {
            // Used to convert a solution of a surrogate instance to a
            // solution of its original instance.
            item_number_ = solution.item_number_;
            x_ = solution.x_;
            profit_ = solution.profit_;
            weight_ = 0;
            for (ItemPos j = 0; j < instance().item_number(); ++j)
                if (contains(j))
                    weight_ += instance().item(j).w;
        }
    }
    return *this;
}

int Solution::contains(ItemPos j) const
{
    assert(j >= 0 && j < instance().item_number());
    assert(instance().item(j).j >= 0 && instance().item(j).j < instance().item_number());
    return x_[instance().item(j).j];
}

int Solution::contains_idx(ItemIdx j) const
{
    assert(j >= 0 && j < instance().item_number());
    return x_[j];
}

void Solution::set(ItemPos j, int b)
{
    assert(b == 0 || b == 1);
    assert(j >= 0);
    assert(j < instance().item_number());
    assert(instance().item(j).j >= 0 && instance().item(j).j < instance().item_number());
    if (contains(j) == b)
        return;
    if (b) {
        profit_ += instance().item(j).p;
        weight_ += instance().item(j).w;
        item_number_++;
    } else {
        profit_ -= instance().item(j).p;
        weight_ -= instance().item(j).w;
        item_number_--;
    }
    x_[instance().item(j).j] = b;
}

void Solution::clear()
{
    item_number_ = 0;
    profit_ = 0;
    weight_ = 0;
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

std::ostream& knapsacksolver::operator<<(std::ostream& os, const Solution& solution)
{
    for (ItemPos j = 0; j < solution.instance().item_number(); ++j)
        os << solution.data()[j] << std::endl;
    return os;
}

std::string Solution::to_string_binary() const
{
    std::string s = "";
    for (ItemPos j = 0; j < instance().item_number(); ++j)
        s += std::to_string(x_[j]);
    return s;
}

std::string Solution::to_string_binary_ordered() const
{
    std::string s = "";
    for (ItemPos j = 0; j < instance().item_number(); ++j)
        s += std::to_string(x_[instance().item(j).j]);
    return s;
}

std::string Solution::to_string_items() const
{
    std::string s = "";
    for (ItemPos j = 0; j < instance().item_number(); ++j) {
        if (x_[j]) {
            if (!s.empty())
                s += ",";
            s += std::to_string(j);
        }
    }
    return s;
}

/*********************************** Output ***********************************/

Output::Output(const Instance& instance, Info& info): solution(instance)
{
    VER(info, std::left << std::setw(10) << "T (ms)");
    VER(info, std::left << std::setw(15) << "LB");
    VER(info, std::left << std::setw(15) << "UB");
    VER(info, std::left << std::setw(10) << "GAP");
    VER(info, "");
    VER(info, std::endl);
    print(info, std::stringstream(""));
}

void Output::print(Info& info, const std::stringstream& s) const
{
    std::string ub_str = (upper_bound == -1)? "inf": std::to_string(upper_bound);
    std::string gap_str = (upper_bound == -1)? "inf": std::to_string(upper_bound - lower_bound);
    double t = round(info.elapsed_time() * 10000) / 10;

    VER(info, std::left << std::setw(10) << t);
    VER(info, std::left << std::setw(15) << lower_bound);
    VER(info, std::left << std::setw(15) << ub_str);
    VER(info, std::left << std::setw(10) << gap_str);
    VER(info, s.str() << std::endl);

    if (!info.output->onlywriteattheend)
        info.write_ini();
}

void Output::update_lb(Profit lb_new, const std::stringstream& s, Info& info)
{
    if (lower_bound >= lb_new)
        return;

    info.output->mutex_sol.lock();

    if (lower_bound < lb_new) {
        lower_bound = lb_new;
        print(info, s);

        info.output->sol_number++;
        double t = round(info.elapsed_time() * 10000) / 10;
        std::string sol_str = "Solution" + std::to_string(info.output->sol_number);
        PUT(info, sol_str, "Cost", lower_bound);
        PUT(info, sol_str, "Time", t);
        if (!info.output->onlywriteattheend)
            solution.write_cert(info.output->certfile);
    }

    info.output->mutex_sol.unlock();
}

void Output::update_sol(const Solution& sol, const std::stringstream& s, Info& info)
{
    if (!sol.feasible() || solution.profit() >= sol.profit())
        return;

    info.output->mutex_sol.lock();

    if (solution.profit() < sol.profit()) {
        solution = sol;
    }

    if (lower_bound < sol.profit()) {
        lower_bound = sol.profit();
        print(info, s);

        info.output->sol_number++;
        double t = round(info.elapsed_time() * 10000) / 10;
        std::string sol_str = "Solution" + std::to_string(info.output->sol_number);
        PUT(info, sol_str, "Cost", lower_bound);
        PUT(info, sol_str, "Time", t);
        if (!info.output->onlywriteattheend)
            solution.write_cert(info.output->certfile);
    }

    info.output->mutex_sol.unlock();
}

void Output::update_ub(Profit ub_new, const std::stringstream& s, Info& info)
{
    if (upper_bound != -1 && upper_bound <= ub_new)
        return;

    info.output->mutex_sol.lock();

    if (upper_bound == -1 || upper_bound > ub_new) {
        upper_bound = ub_new;
        print(info, s);

        info.output->bnd_number++;
        double t = round(info.elapsed_time() * 10000) / 10;
        std::string sol_str = "Bound" + std::to_string(info.output->bnd_number);
        PUT(info, sol_str, "Cost", upper_bound);
        PUT(info, sol_str, "Time", t);
        if (!info.output->onlywriteattheend)
            solution.write_cert(info.output->certfile);
    }

    info.output->mutex_sol.unlock();
}

Output& Output::algorithm_end(Info& info)
{
    double t = round(info.elapsed_time() * 10000) / 10;
    std::string ub_str = (upper_bound == -1)? "inf": std::to_string(upper_bound);
    std::string gap_str = (upper_bound == -1)? "inf": std::to_string(upper_bound - lower_bound);
    std::string sol_str = (solution.feasible() && solution.profit() == lower_bound)? "OK": "none";
    double gap = (lower_bound == 0 || upper_bound == -1)?
        std::numeric_limits<double>::infinity():
        (double)(10000 * (upper_bound - lower_bound) / lower_bound) / 100;
    PUT(info, "Solution", "Value", lower_bound);
    PUT(info, "Bound", "Value", upper_bound);
    PUT(info, "Solution", "Time", t);
    PUT(info, "Bound", "Time", t);
    VER(info, "---" << std::endl
            << "Value: " << lower_bound << std::endl
            << "Bound: " << ub_str << std::endl
            << "Gap: " << gap_str << std::endl
            << "Gap (%): " << gap << std::endl
            << "Solution: " << sol_str << std::endl
            << "Time (ms): " << t << std::endl);

    info.write_ini();
    solution.write_cert(info.output->certfile);
    return *this;
}

Profit knapsacksolver::algorithm_end(Profit upper_bound, Info& info)
{
    double t = round(info.elapsed_time() * 10000) / 10;
    PUT(info, "Bound", "Value", upper_bound);
    PUT(info, "Bound", "Time", t);
    VER(info, "---" << std::endl
            << "Bound: " << upper_bound << std::endl
            << "Time (ms): " << t << std::endl);

    info.write_ini();
    return upper_bound;
}

