#include "subsetsumsolver/solution.hpp"

using namespace subsetsumsolver;

Solution::Solution(const Instance& instance):
    instance_(&instance),
    contains_(instance.number_of_items(), 0)
{ }

Solution::Solution(const Instance& instance, std::string certificate_path):
    Solution(instance)
{
    if (certificate_path.empty())
        return;
    std::ifstream file(certificate_path);
    if (!file.good()) {
        throw std::runtime_error(
                "Unable to open file \"" + certificate_path + "\".");
    }

    ItemPos n;
    ItemPos j;
    file >> n;
    for (ItemPos pos = 0; pos < n; ++pos) {
        file >> j;
        add(j);
    }
}

void Solution::add(ItemId j)
{
    contains_[j] = 1;
    number_of_items_++;
    weight_ += instance().weight(j);
}

void Solution::write(std::string certificate_path)
{
    if (certificate_path.empty())
        return;
    std::ofstream file(certificate_path);
    if (!file.good()) {
        throw std::runtime_error(
                "Unable to open file \"" + certificate_path + "\".");
    }

    file << number_of_items() << std::endl;
    for (ItemId j = 0; j < instance().number_of_items(); ++j)
        if (contains(j))
            file << j << std::endl;
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////// Output ////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

Output::Output(
        const Instance& instance,
        optimizationtools::Info& info):
    solution(instance),
    upper_bound(instance.capacity())
{
    info.os()
            << std::setw(10) << "T (s)"
            << std::setw(14) << "UB"
            << std::setw(14) << "LB"
            << std::setw(14) << "GAP"
            << std::setw(10) << "GAP (%)"
            << std::setw(24) << "Comment"
            << std::endl
            << std::setw(10) << "-----"
            << std::setw(14) << "--"
            << std::setw(14) << "--"
            << std::setw(14) << "---"
            << std::setw(10) << "-------"
            << std::setw(24) << "-------"
            << std::endl;
    print(info, std::stringstream(""));
    info.reset_time();
}

bool Output::optimal() const
{
    return (lower_bound == upper_bound);
}

double Output::gap() const
{
    return (double)(upper_bound - lower_bound) / upper_bound;
}

void Output::print(
        optimizationtools::Info& info,
        const std::stringstream& s) const
{
    double t = info.elapsed_time();
    std::streamsize precision = std::cout.precision();

    info.os()
            << std::setw(10) << std::fixed << std::setprecision(3) << t << std::defaultfloat << std::setprecision(precision)
            << std::setw(14) << lower_bound
            << std::setw(14) << upper_bound
            << std::setw(14) << upper_bound - lower_bound
            << std::setw(10) << std::fixed << std::setprecision(2) << gap() << std::defaultfloat << std::setprecision(precision)
            << std::setw(24) << s.str() << std::endl;

    if (!info.output->only_write_at_the_end)
        info.write_json_output();
}

void Output::update_solution(
        const Solution& solution_new,
        const std::stringstream& s,
        optimizationtools::Info& info)
{
    if (!solution_new.better(lower_bound))
        return;

    info.lock();

    if (solution_new.better(lower_bound)) {
        solution = solution_new;
        lower_bound = solution_new.weight();
        print(info, s);

        info.output->number_of_solutions++;
        double t = (double)std::round(info.elapsed_time() * 10000) / 10000;
        std::string sol_str = "Solution" + std::to_string(info.output->number_of_solutions);
        info.add_to_json(sol_str, "Value", lower_bound);
        info.add_to_json(sol_str, "Time", t);
        if (!info.output->only_write_at_the_end)
            solution.write(info.output->certificate_path);
    }

    info.unlock();
}

void Output::update_lower_bound(
        Weight lower_bound_new,
        const std::stringstream& s,
        optimizationtools::Info& info)
{
    if (!(lower_bound < lower_bound_new))
        return;

    info.lock();

    if (lower_bound < lower_bound_new) {
        lower_bound = lower_bound_new;
        print(info, s);

        info.output->number_of_bounds++;
        double t = (double)std::round(info.elapsed_time() * 10000) / 10000;
        std::string sol_str = "Bound" + std::to_string(info.output->number_of_bounds);
        info.add_to_json(sol_str, "Value", lower_bound);
        info.add_to_json(sol_str, "Time", t);
        if (!info.output->only_write_at_the_end)
            solution.write(info.output->certificate_path);
    }

    info.unlock();
}

void Output::update_upper_bound(
        Weight upper_bound_new,
        const std::stringstream& s,
        optimizationtools::Info& info)
{
    if (!(upper_bound > upper_bound_new))
        return;

    info.lock();

    if (upper_bound > upper_bound_new) {
        upper_bound = upper_bound_new;
        print(info, s);

        info.output->number_of_bounds++;
        double t = (double)std::round(info.elapsed_time() * 10000) / 10000;
        std::string sol_str = "Bound" + std::to_string(info.output->number_of_bounds);
        info.add_to_json(sol_str, "Value", upper_bound);
        info.add_to_json(sol_str, "Time", t);
        if (!info.output->only_write_at_the_end)
            solution.write(info.output->certificate_path);
    }

    info.unlock();
}

Output& Output::algorithm_end(optimizationtools::Info& info)
{
    time = (double)std::round(info.elapsed_time() * 10000) / 10000;

    info.add_to_json("Solution", "Value", lower_bound);
    info.add_to_json("Bound", "Value", upper_bound);
    info.add_to_json("Solution", "Time", time);
    info.add_to_json("Bound", "Time", time);
    info.os()
            << std::endl
            << "Final statistics" << std::endl
            << "----------------" << std::endl
            << "Value:                    " << lower_bound << " / " << solution.instance().capacity() << " (" << (double)lower_bound / solution.instance().capacity() * 100 << "%)" << std::endl
            << "Has solution:             " << (solution.weight() == lower_bound) << std::endl
            << "Bound:                    " << upper_bound << std::endl
            << "Gap:                      " << upper_bound - lower_bound << std::endl
            << "Gap (%):                  " << gap() << std::endl
            << "Number of items:          " << solution.number_of_items() << " / " << solution.instance().number_of_items() << " (" << (double)solution.number_of_items() / solution.instance().number_of_items() * 100 << "%)" << std::endl
            << "Time (s):                 " << time << std::endl;

    if (info.verbosity_level() >= 2) {
        info.os()
            << std::endl
            << std::setw(12) << "Item"
            << std::setw(12) << "Weight"
            << std::endl
            << std::setw(12) << "----"
            << std::setw(12) << "------"
            << std::endl;
        for (ItemId j = 0; j < solution.instance().number_of_items(); ++j) {
            if (solution.contains(j)) {
                info.os()
                    << std::setw(12) << j
                    << std::setw(12) << solution.instance().weight(j)
                    << std::endl;
            }
        }
    }

    info.write_json_output();
    solution.write(info.output->certificate_path);
    return *this;
}

