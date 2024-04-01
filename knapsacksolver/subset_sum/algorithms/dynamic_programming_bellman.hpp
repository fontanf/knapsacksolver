#pragma once

#include "knapsacksolver/subset_sum/solution.hpp"

namespace knapsacksolver
{
namespace subset_sum
{

const Output dynamic_programming_bellman_array(
        const Instance& instance,
        const Parameters& parameters = {});

const Output dynamic_programming_bellman_list(
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

