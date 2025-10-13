#pragma once

#include "knapsacksolver/solution.hpp"

namespace knapsacksolver
{

struct DynamicProgrammingPrimalDualParameters: Parameters
{
    bool greedy = true;

    bool pairing = false;

    ItemId partial_solution_size = 64;


    virtual int format_width() const override { return 37; }

    virtual void format(std::ostream& os) const override
    {
        Parameters::format(os);
        int width = format_width();
        os
            << std::setw(width) << std::left << "Greedy: " << greedy << std::endl
            << std::setw(width) << std::left << "Pairing: " << pairing << std::endl
            << std::setw(width) << std::left << "Partial solution size: " << partial_solution_size << std::endl
            ;
    }

    virtual nlohmann::json to_json() const override
    {
        nlohmann::json json = Parameters::to_json();
        json.merge_patch({
                {"Greedy", greedy},
                {"Pairing", pairing},
                {"PartialSolutionSize", partial_solution_size}});
        return json;
    }
};

struct DynamicProgrammingPrimalDualOutput: Output
{
    DynamicProgrammingPrimalDualOutput(
            const Instance& instance):
        Output(instance) { }


    Counter number_of_recursive_calls = 0;


    virtual void format(std::ostream& os) const override
    {
        Output::format(os);
        int width = format_width();
        os
            << std::setw(width) << std::left << "Number of recursive calls: " << number_of_recursive_calls << std::endl
            ;
    }

    virtual nlohmann::json to_json() const override
    {
        nlohmann::json json = Output::to_json();
        json.merge_patch({
                {"NumberOfRecursiveCalls", number_of_recursive_calls}});
        return json;
    }
};

const DynamicProgrammingPrimalDualOutput dynamic_programming_primal_dual(
        const Instance& instance,
        const DynamicProgrammingPrimalDualParameters& parameters = {});

}
