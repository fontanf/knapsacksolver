#include "knapsacksolver/solution.hpp"

#include <iomanip>

using namespace knapsacksolver;

Solution::Solution(const Instance& instance):
    instance_(instance),
    x_(instance.number_of_items(), 0)
{ }

Solution::Solution(const Instance& instance, std::string filepath):
    instance_(instance),
    x_(instance.number_of_items(), 0)
{
    if (filepath.empty())
        return;
    std::ifstream file(filepath);
    if (!file.good()) {
        std::cerr << "\033[31m" << "ERROR, unable to open file \"" << filepath << "\"" << "\033[0m" << std::endl;
        return;
    }

    int x = 0;
    for (ItemPos j = 0; j < instance.number_of_items(); ++j) {
        file >> x;
        set(j, x);
    }
}

Solution::Solution(const Solution& solution):
    instance_(solution.instance_),
    number_of_items_(solution.number_of_items_),
    profit_(solution.profit_),
    weight_(solution.weight_),
    x_(solution.x_)
{ }

Solution& Solution::operator=(const Solution& solution)
{
    if (this != &solution) {
        if (&this->instance() == &solution.instance()) {
            number_of_items_ = solution.number_of_items_;
            profit_ = solution.profit_;
            weight_ = solution.weight_;
            x_ = solution.x_;
        } else {
            // Used to convert a solution of a surrogate instance to a
            // solution of its original instance.
            number_of_items_ = solution.number_of_items_;
            x_ = solution.x_;
            profit_ = solution.profit_;
            weight_ = 0;
            for (ItemPos j = 0; j < instance().number_of_items(); ++j)
                if (contains(j))
                    weight_ += instance().item(j).w;
        }
    }
    return *this;
}

int Solution::contains(ItemPos j) const
{
    assert(j >= 0 && j < instance().number_of_items());
    assert(instance().item(j).j >= 0 && instance().item(j).j < instance().number_of_items());
    return x_[instance().item(j).j];
}

int Solution::contains_idx(ItemIdx j) const
{
    assert(j >= 0 && j < instance().number_of_items());
    return x_[j];
}

void Solution::set(ItemPos j, int b)
{
    assert(b == 0 || b == 1);
    assert(j >= 0);
    assert(j < instance().number_of_items());
    assert(instance().item(j).j >= 0 && instance().item(j).j < instance().number_of_items());
    if (contains(j) == b)
        return;
    if (b) {
        profit_ += instance().item(j).p;
        weight_ += instance().item(j).w;
        number_of_items_++;
    } else {
        profit_ -= instance().item(j).p;
        weight_ -= instance().item(j).w;
        number_of_items_--;
    }
    x_[instance().item(j).j] = b;
}

void Solution::clear()
{
    number_of_items_ = 0;
    profit_ = 0;
    weight_ = 0;
    std::fill(x_.begin(), x_.end(), 0);
}

void Solution::write(std::string filepath)
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
    for (ItemPos j = 0; j < solution.instance().number_of_items(); ++j)
        os << solution.data()[j] << std::endl;
    return os;
}

std::string Solution::to_string_binary() const
{
    std::string s = "";
    for (ItemPos j = 0; j < instance().number_of_items(); ++j)
        s += std::to_string(x_[j]);
    return s;
}

std::string Solution::to_string_binary_ordered() const
{
    std::string s = "";
    for (ItemPos j = 0; j < instance().number_of_items(); ++j)
        s += std::to_string(x_[instance().item(j).j]);
    return s;
}

std::string Solution::to_string_items() const
{
    std::string s = "";
    for (ItemPos j = 0; j < instance().number_of_items(); ++j) {
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
    info.reset_time();
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

    if (!info.output->only_write_at_the_end)
        info.write_json_output();
}

void Output::update_lb(Profit lb_new, const std::stringstream& s, Info& info)
{
    if (lower_bound >= lb_new)
        return;

    info.output->mutex_solutions.lock();

    if (lower_bound < lb_new) {
        lower_bound = lb_new;
        print(info, s);

        info.output->number_of_solutions++;
        double t = round(info.elapsed_time() * 10000) / 10;
        std::string sol_str = "Solution" + std::to_string(info.output->number_of_solutions);
        PUT(info, sol_str, "Cost", lower_bound);
        PUT(info, sol_str, "Time", t);
        if (!info.output->only_write_at_the_end)
            solution.write(info.output->certificate_path);
    }

    info.output->mutex_solutions.unlock();
}

void Output::update_sol(const Solution& sol, const std::stringstream& s, Info& info)
{
    if (!sol.feasible() || solution.profit() >= sol.profit())
        return;

    info.output->mutex_solutions.lock();

    if (solution.profit() < sol.profit()) {
        solution = sol;
    }

    if (lower_bound < sol.profit()) {
        lower_bound = sol.profit();
        print(info, s);

        info.output->number_of_solutions++;
        double t = round(info.elapsed_time() * 10000) / 10;
        std::string sol_str = "Solution" + std::to_string(info.output->number_of_solutions);
        PUT(info, sol_str, "Cost", lower_bound);
        PUT(info, sol_str, "Time", t);
        if (!info.output->only_write_at_the_end)
            solution.write(info.output->certificate_path);
    }

    info.output->mutex_solutions.unlock();
}

void Output::update_ub(Profit ub_new, const std::stringstream& s, Info& info)
{
    if (upper_bound != -1 && upper_bound <= ub_new)
        return;

    info.output->mutex_solutions.lock();

    if (upper_bound == -1 || upper_bound > ub_new) {
        upper_bound = ub_new;
        print(info, s);

        info.output->number_of_bounds++;
        double t = round(info.elapsed_time() * 10000) / 10;
        std::string sol_str = "Bound" + std::to_string(info.output->number_of_bounds);
        PUT(info, sol_str, "Cost", upper_bound);
        PUT(info, sol_str, "Time", t);
        if (!info.output->only_write_at_the_end)
            solution.write(info.output->certificate_path);
    }

    info.output->mutex_solutions.unlock();
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

    info.write_json_output();
    solution.write(info.output->certificate_path);
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

    info.write_json_output();
    return upper_bound;
}

