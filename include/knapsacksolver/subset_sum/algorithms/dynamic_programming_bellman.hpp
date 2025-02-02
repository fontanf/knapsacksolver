#pragma once

#include "knapsacksolver/subset_sum/solution.hpp"

namespace knapsacksolver
{
namespace subset_sum
{

struct DynamicProgrammingBellmanArrayOutput: public Output
{
    /** Constructor. */
    DynamicProgrammingBellmanArrayOutput(const Instance& instance):
        Output(instance)
    { }

    std::vector<ItemId> values;

    bool is_reachable(Weight weight) const
    {
        return values[weight] != -1;
    }
};

DynamicProgrammingBellmanArrayOutput dynamic_programming_bellman_array(
        const Instance& instance,
        const Parameters& parameters = {});

Output dynamic_programming_bellman_list(
        const Instance& instance,
        const Parameters& parameters = {});

struct DynamicProgrammingBellmanWordRamOutput: public Output
{
    /** Constructor. */
    DynamicProgrammingBellmanWordRamOutput(const Instance& instance):
        Output(instance)
    { }

    std::vector<uint64_t> values;

    bool is_reachable(Weight weight) const
    {
        Weight word = weight / 64;
        int bit = weight % 64;
        return (values[word] >> bit);
    }
};

DynamicProgrammingBellmanWordRamOutput dynamic_programming_bellman_word_ram(
        const Instance& instance,
        const Parameters& parameters = {});

Output dynamic_programming_bellman_word_ram_rec(
        const Instance& instance,
        const Parameters& parameters = {});

}
}
