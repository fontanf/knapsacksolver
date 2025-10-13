#include "knapsacksolver/algorithm_formatter.hpp"

#include "optimizationtools/utils/utils.hpp"

#include <iomanip>

using namespace knapsacksolver;

void AlgorithmFormatter::start(
        const std::string& algorithm_name)
{
    if (parameters_.json_output)
        output_.json["Parameters"] = parameters_.to_json();

    if (parameters_.verbosity_level == 0)
        return;
    *os_
        << "====================================" << std::endl
        << "           KnapsackSolver           " << std::endl
        << "====================================" << std::endl
        << std::endl
        << "Instance" << std::endl
        << "--------" << std::endl;
    output_.solution.instance().format(*os_, parameters_.verbosity_level);
    *os_
        << std::endl
        << "Algorithm" << std::endl
        << "---------" << std::endl
        << algorithm_name << std::endl
        << std::endl
        << "Parameters" << std::endl
        << "----------" << std::endl;
    parameters_.format(*os_);
}

void AlgorithmFormatter::print_header()
{
    if (parameters_.verbosity_level == 0)
        return;
    *os_
        << std::right
        << std::endl
        << std::setw(12) << "Time (s)"
        << std::setw(6) << "Sol."
        << std::setw(24) << "Value"
        << std::setw(24) << "Bound"
        << std::setw(16) << "Gap"
        << std::setw(8) << "Gap (%)"
        << std::setw(32) << "Comment"
        << std::endl
        << std::setw(12) << "--------"
        << std::setw(6) << "----"
        << std::setw(24) << "-----"
        << std::setw(24) << "-----"
        << std::setw(16) << "---"
        << std::setw(8) << "-------"
        << std::setw(32) << "-------"
        << std::endl;
    print("");
}

void AlgorithmFormatter::print(
        const std::string& s)
{
    if (parameters_.verbosity_level == 0)
        return;
    std::streamsize precision = std::cout.precision();
    *os_
        << std::setw(12) << std::fixed << std::setprecision(3) << output_.time << std::defaultfloat << std::setprecision(precision)
        << std::setw(6) << output_.has_solution()
        << std::setw(24) << output_.value
        << std::setw(24) << output_.bound
        << std::setw(16) << output_.absolute_optimality_gap()
        << std::setw(8) << std::fixed << std::setprecision(2) << output_.relative_optimality_gap() * 100 << std::defaultfloat << std::setprecision(precision)
        << std::setw(32) << s << std::endl;
}

void AlgorithmFormatter::update_solution(
        const Solution& solution_new,
        const std::string& s)
{
    if ((output_.has_solution() && optimizationtools::is_solution_strictly_better(
                objective_direction(),
                output_.value,
                solution_new.feasible(),
                solution_new.objective_value()))
            || (!output_.has_solution() && optimizationtools::is_solution_better_or_equal(
                objective_direction(),
                output_.value,
                solution_new.feasible(),
                solution_new.objective_value()))) {
        output_.time = parameters_.timer.elapsed_time();
        output_.solution = solution_new;
        output_.value = output_.solution.objective_value();
        print(s);
        if (parameters_.json_output)
            output_.json["IntermediaryOutputs"].push_back(output_.to_json());
        parameters_.new_solution_callback(output_);
    }
}

void AlgorithmFormatter::update_value(
        Weight value_new,
        const std::string& s)
{
    if (optimizationtools::is_value_strictly_better(
                objective_direction(),
                output_.value,
                value_new)) {
        output_.time = parameters_.timer.elapsed_time();
        output_.value = value_new;
        print(s);
        if (parameters_.json_output)
            output_.json["IntermediaryOutputs"].push_back(output_.to_json());
        parameters_.new_solution_callback(output_);
    }
}

void AlgorithmFormatter::update_bound(
        Weight bound_new,
        const std::string& s)
{
    if (optimizationtools::is_bound_strictly_better(
            objective_direction(),
            output_.bound,
            bound_new)) {
        output_.time = parameters_.timer.elapsed_time();
        output_.bound = bound_new;
        print(s);
        if (parameters_.json_output)
            output_.json["IntermediaryOutputs"].push_back(output_.to_json());
        parameters_.new_solution_callback(output_);
    }
}

void AlgorithmFormatter::end()
{
    output_.time = parameters_.timer.elapsed_time();
    if (parameters_.json_output)
        output_.json["Output"] = output_.to_json();

    if (parameters_.verbosity_level == 0)
        return;
    *os_
        << std::endl
        << "Final statistics" << std::endl
        << "----------------" << std::endl;
    output_.format(*os_);
    *os_
        << std::endl
        << "Solution" << std::endl
        << "--------" << std::endl;
    output_.solution.format(*os_, parameters_.verbosity_level);
}
