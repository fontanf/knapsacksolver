#include "knapsacksolver/solution.hpp"

#include <iomanip>

using namespace knapsacksolver;

Solution::Solution(const Instance& instance):
    instance_(&instance),
    x_(instance.number_of_items(), 0)
{ }

Solution::Solution(
        const Instance& instance,
        std::string certificate_path):
    Solution(instance)
{
    if (certificate_path.empty())
        return;
    std::ifstream file(certificate_path);
    if (!file.good()) {
        throw std::runtime_error(
                "Unable to open file \"" + certificate_path + "\".");
    }

    int x = 0;
    for (ItemPos j = 0; j < instance.number_of_items(); ++j) {
        file >> x;
        set(j, x);
    }
}

void Solution::update(const Solution& solution)
{
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

void Solution::write(std::string certificate_path)
{
    if (certificate_path.empty())
        return;
    std::ofstream cert(certificate_path);
    if (!cert.good()) {
        throw std::runtime_error(
                "Unable to open file \"" + certificate_path + "\".");
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

////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////// Output ////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

Output::Output(const Instance& instance, Info& info): solution(instance)
{
    info.reset_time();
    info.os()
            << std::setw(10) << "T (ms)"
            << std::setw(15) << "LB"
            << std::setw(15) << "UB"
            << std::setw(10) << "GAP"
            << std::setw(24) << "Comment"
            << std::endl
            << std::setw(10) << "------"
            << std::setw(15) << "--"
            << std::setw(15) << "--"
            << std::setw(10) << "---"
            << std::setw(24) << "-------"
            << std::endl;
    print(info, std::stringstream(""));
}

void Output::print(Info& info, const std::stringstream& s) const
{
    std::string ub_str = (upper_bound == -1)? "inf": std::to_string(upper_bound);
    std::string gap_str = (upper_bound == -1)? "inf": std::to_string(upper_bound - lower_bound);
    double t = round(info.elapsed_time() * 10000) / 10;

    info.os()
            << std::setw(10) << t
            << std::setw(15) << lower_bound
            << std::setw(15) << ub_str
            << std::setw(10) << gap_str
            << std::setw(24) << s.str() << std::endl;

    if (!info.output->only_write_at_the_end)
        info.write_json_output();
}

void Output::update_lower_bound(
        Profit lower_bound_new,
        const std::stringstream& s,
        Info& info)
{
    if (lower_bound >= lower_bound_new)
        return;

    info.lock();

    if (lower_bound < lower_bound_new) {
        lower_bound = lower_bound_new;
        print(info, s);

        info.output->number_of_solutions++;
        double t = round(info.elapsed_time() * 10000) / 10;
        std::string sol_str = "Solution" + std::to_string(info.output->number_of_solutions);
        info.add_to_json(sol_str, "Cost", lower_bound);
        info.add_to_json(sol_str, "Time", t);
        if (!info.output->only_write_at_the_end)
            solution.write(info.output->certificate_path);
    }

    info.unlock();
}

void Output::update_solution(
        const Solution& solution_new,
        const std::stringstream& s,
        Info& info)
{
    if (!solution_new.feasible()
            || solution.profit() >= solution_new.profit())
        return;

    info.lock();

    if (&solution_new.instance() != &solution.instance()) {
        throw std::runtime_error("update_solution");
    }

    if (solution.profit() < solution_new.profit()) {
        solution = solution_new;
    }

    if (lower_bound < solution_new.profit()) {
        lower_bound = solution_new.profit();
        print(info, s);

        info.output->number_of_solutions++;
        double t = round(info.elapsed_time() * 10000) / 10;
        std::string sol_str = "Solution" + std::to_string(info.output->number_of_solutions);
        info.add_to_json(sol_str, "Cost", lower_bound);
        info.add_to_json(sol_str, "Time", t);
        if (!info.output->only_write_at_the_end)
            solution.write(info.output->certificate_path);
    }

    info.unlock();
}

void Output::update_upper_bound(
        Profit upper_bound_new,
        const std::stringstream& s,
        Info& info)
{
    if (upper_bound != -1 && upper_bound <= upper_bound_new)
        return;

    info.lock();

    if (upper_bound == -1 || upper_bound > upper_bound_new) {
        upper_bound = upper_bound_new;
        print(info, s);

        info.output->number_of_bounds++;
        double t = round(info.elapsed_time() * 10000) / 10;
        std::string sol_str = "Bound" + std::to_string(info.output->number_of_bounds);
        info.add_to_json(sol_str, "Cost", upper_bound);
        info.add_to_json(sol_str, "Time", t);
        if (!info.output->only_write_at_the_end)
            solution.write(info.output->certificate_path);
    }

    info.unlock();
}

Output& Output::algorithm_end(Info& info)
{
    double t = round(info.elapsed_time() * 10000) / 10;
    std::string ub_str = (upper_bound == -1)? "inf": std::to_string(upper_bound);
    std::string gap_str = (upper_bound == -1)? "inf": std::to_string(upper_bound - lower_bound);
    std::string sol_str = (solution.feasible() && solution.profit() == lower_bound)? "1": "0";
    double gap = (lower_bound == 0 || upper_bound == -1)?
        std::numeric_limits<double>::infinity():
        (double)(10000 * (upper_bound - lower_bound) / lower_bound) / 100;
    info.add_to_json("Solution", "Value", lower_bound);
    info.add_to_json("Solution", "Time", t);
    info.add_to_json("Bound", "Value", upper_bound);
    info.add_to_json("Bound", "Time", t);
    info.os()
            << std::endl
            << "Final statistics" << std::endl
            << "----------------" << std::endl
            << "Value:                      " << lower_bound << std::endl
            << "Bound:                      " << ub_str << std::endl
            << "Gap:                        " << gap_str << std::endl
            << "Gap (%):                    " << gap << std::endl
            << "Solution:                   " << sol_str << std::endl
            << "Number of items:            " << solution.number_of_items() << " / " << solution.instance().number_of_items() << " (" << (double)solution.number_of_items() / solution.instance().number_of_items() * 100 << "%)" << std::endl
            << "Weight:                     " << solution.weight() << " / " << solution.instance().capacity() << " (" << (double)solution.weight() / solution.instance().capacity() * 100 << "%)" << std::endl
            << "Time (ms):                  " << t << std::endl;

    info.write_json_output();
    solution.write(info.output->certificate_path);
    return *this;
}

Profit knapsacksolver::algorithm_end(Profit upper_bound, Info& info)
{
    double t = round(info.elapsed_time() * 10000) / 10;
    info.add_to_json("Bound", "Value", upper_bound);
    info.add_to_json("Bound", "Time", t);
    info.os()
            << std::endl
            << "Final statistics" << std::endl
            << "----------------" << std::endl
            << "Bound:                      " << upper_bound << std::endl
            << "Time (ms):                  " << t << std::endl;

    info.write_json_output();
    return upper_bound;
}

