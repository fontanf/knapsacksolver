#pragma once

#include "knapsacksolver/solution.hpp"

#include <random>

namespace knapsacksolver
{

struct DynamicProgrammingPrimalDualParameters: Parameters
{
    bool greedy = true;

    bool pairing = false;

    ItemId partial_solution_size = 64;

    /**
     * Maximum number of states.
     *
     * It is checked after each item. If it is exceeded, the algorithm stops
     * as when the time limit is reached: the solution is feasible and the
     * bound is valid, but they may not be optimal.
     *
     * If negative, the number of states is not limited.
     */
    StateId maximum_number_of_states = -1;


    virtual int format_width() const override { return 37; }

    virtual void format(std::ostream& os) const override
    {
        Parameters::format(os);
        int width = format_width();
        os
            << std::setw(width) << std::left << "Greedy: " << greedy << std::endl
            << std::setw(width) << std::left << "Pairing: " << pairing << std::endl
            << std::setw(width) << std::left << "Partial solution size: " << partial_solution_size << std::endl
            << std::setw(width) << std::left << "Maximum number of states: " << maximum_number_of_states << std::endl
            ;
    }

    virtual nlohmann::json to_json() const override
    {
        nlohmann::json json = Parameters::to_json();
        json.merge_patch({
                {"Greedy", greedy},
                {"Pairing", pairing},
                {"PartialSolutionSize", partial_solution_size},
                {"MaximumNumberOfStates", maximum_number_of_states}});
        return json;
    }
};

struct DynamicProgrammingPrimalDualOutput: Output
{
    DynamicProgrammingPrimalDualOutput(
            const Instance& instance):
        Output(instance) { }


    Counter number_of_recursive_calls = 0;

    /** True iff the algorithm stopped because of the maximum number of states. */
    bool maximum_number_of_states_reached = false;


    virtual void format(std::ostream& os) const override
    {
        Output::format(os);
        int width = format_width();
        os
            << std::setw(width) << std::left << "Number of recursive calls: " << number_of_recursive_calls << std::endl
            << std::setw(width) << std::left << "Max. states reached: " << maximum_number_of_states_reached << std::endl
            ;
    }

    virtual nlohmann::json to_json() const override
    {
        nlohmann::json json = Output::to_json();
        json.merge_patch({
                {"NumberOfRecursiveCalls", number_of_recursive_calls},
                {"MaximumNumberOfStatesReached", maximum_number_of_states_reached}});
        return json;
    }
};

/**
 * Primal-dual dynamic programming ("minknap").
 *
 * The items with the same efficiency are ordered randomly with 'generator'.
 * On instances where many items have the same efficiency, such as subset sum
 * instances, their order determines the states explored before the maximum
 * number of states is reached.
 */
const DynamicProgrammingPrimalDualOutput dynamic_programming_primal_dual(
        const Instance& instance,
        std::mt19937_64& generator,
        const DynamicProgrammingPrimalDualParameters& parameters = {});

}
