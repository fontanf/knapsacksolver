#pragma once

#include "knapsacksolver/solution.hpp"
#include "knapsacksolver/sort.hpp"

namespace knapsacksolver
{

Output trivial(
        const Instance& instance,
        const Parameters& parameters = {});


struct GreedyParameters: Parameters
{
    /** Full sort. */
    FullSort* full_sort = nullptr;

    /** Partial sort. */
    PartialSort* partial_sort = nullptr;


    virtual int format_width() const override { return 37; }

    virtual void format(std::ostream& os) const override
    {
        Parameters::format(os);
        int width = format_width();
        os
            << std::setw(width) << std::left << "Has full sort: " << (full_sort != nullptr) << std::endl
            << std::setw(width) << std::left << "Has partial sort: " << (partial_sort != nullptr) << std::endl
            ;
    }

    virtual nlohmann::json to_json() const override
    {
        nlohmann::json json = Parameters::to_json();
        json.merge_patch({
                {"HasFullSort", (full_sort != nullptr)},
                {"HasPartialSort", (partial_sort != nullptr)}});
        return json;
    }
};

Output greedy(
        const Instance& instance,
        const GreedyParameters& parameters = {});

}
