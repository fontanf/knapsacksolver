#pragma once

#include "knapsacksolver/solution.hpp"

namespace knapsacksolver
{

Output dynamic_programming_bellman_rec(
        const Instance& instance,
        const Parameters& parameters = {});


Output dynamic_programming_bellman_array(
        const Instance& instance,
        const Parameters& parameters = {});


Output dynamic_programming_bellman_array_parallel(
        const Instance& instance,
        const Parameters& parameters = {});


Output dynamic_programming_bellman_array_all(
        const Instance& instance,
        const Parameters& parameters = {});


struct DynamicProgrammingBellmanArrayOneOutput: Output
{
    DynamicProgrammingBellmanArrayOneOutput(
            const Instance& instance):
        Output(instance) { }


    /** Number of iterations. */
    Counter number_of_iterations = 0;


    virtual void format(std::ostream& os) const override
    {
        Output::format(os);
        int width = format_width();
        os
            << std::setw(width) << std::left << "Number of iterations: " << number_of_iterations << std::endl
            ;
    }

    virtual nlohmann::json to_json() const override
    {
        nlohmann::json json = Output::to_json();
        json.merge_patch({
                {"NumberOfIterations", number_of_iterations}});
        return json;
    }
};

const DynamicProgrammingBellmanArrayOneOutput dynamic_programming_bellman_array_one(
        const Instance& instance,
        const Parameters& parameters = {});


struct DynamicProgrammingBellmanArrayPartParameters: Parameters
{
    /** Size of the partial solutions. */
    Counter partial_solution_size = 64;


    virtual int format_width() const override { return 37; }

    virtual void format(std::ostream& os) const override
    {
        Parameters::format(os);
        int width = format_width();
        os
            << std::setw(width) << std::left << "Partial solution size: " << partial_solution_size << std::endl
            ;
    }

    virtual nlohmann::json to_json() const override
    {
        nlohmann::json json = Parameters::to_json();
        json.merge_patch({
                {"PartialSolutionSize", partial_solution_size}});
        return json;
    }
};

struct DynamicProgrammingBellmanArrayPartOutput: Output
{
    DynamicProgrammingBellmanArrayPartOutput(
            const Instance& instance):
        Output(instance) { }


    /** Number of iterations. */
    Counter number_of_iterations = 0;


    virtual void format(std::ostream& os) const override
    {
        Output::format(os);
        int width = format_width();
        os
            << std::setw(width) << std::left << "Number of iterations: " << number_of_iterations << std::endl
            ;
    }

    virtual nlohmann::json to_json() const override
    {
        nlohmann::json json = Output::to_json();
        json.merge_patch({
                {"NumberOfIterations", number_of_iterations}});
        return json;
    }
};

const DynamicProgrammingBellmanArrayPartOutput dynamic_programming_bellman_array_part(
        const Instance& instance,
        const DynamicProgrammingBellmanArrayPartParameters& parameters = {});


Output dynamic_programming_bellman_array_rec(
        const Instance& instance,
        const Parameters& parameters = {});


struct DynamicProgrammingBellmanListParameters: Parameters
{
    /** Sort the items. */
    bool sort = false;


    virtual int format_width() const override { return 37; }

    virtual void format(std::ostream& os) const override
    {
        Parameters::format(os);
        int width = format_width();
        os
            << std::setw(width) << std::left << "Sort: " << sort << std::endl
            ;
    }

    virtual nlohmann::json to_json() const override
    {
        nlohmann::json json = Parameters::to_json();
        json.merge_patch({
                {"Sort", sort}});
        return json;
    }
};

Output dynamic_programming_bellman_list(
        const Instance& instance,
        const DynamicProgrammingBellmanListParameters& parameters = {});

}
