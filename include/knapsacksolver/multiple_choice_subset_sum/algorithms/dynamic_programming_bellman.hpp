#pragma once

#include "knapsacksolver/multiple_choice_subset_sum/solution.hpp"

namespace knapsacksolver
{
namespace multiple_choice_subset_sum
{

const Output dynamic_programming_bellman_array(
        const Instance& instance,
        const Parameters& parameters = {});

const Output dynamic_programming_bellman_word_ram(
        const Instance& instance,
        const Parameters& parameters = {});

const Output dynamic_programming_bellman_word_ram_rec(
        const Instance& instance,
        const Parameters& parameters = {});

}
}

