#pragma once

#include "subsetsumsolver/solution.hpp"

namespace subsetsumsolver
{

Output dynamic_programming_bellman_array(
        const Instance& instance,
        Info info = Info());

Output dynamic_programming_bellman_list(
        const Instance& instance,
        Info info = Info());

Output dynamic_programming_bellman_word_ram(
        const Instance& instance,
        Info info = Info());

Output dynamic_programming_bellman_word_ram_rec(
        const Instance& instance,
        Info info = Info());

}

