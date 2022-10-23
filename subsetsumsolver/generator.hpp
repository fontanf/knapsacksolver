#pragma once

#include "subsetsumsolver/instance.hpp"

#include <random>

namespace subsetsumsolver
{

Instance generate_pthree(
        ItemPos number_of_items,
        std::mt19937_64& generator);

Instance generate_psix(
        ItemPos number_of_items,
        std::mt19937_64& generator);

Instance generate_evenodd(
        ItemPos number_of_items,
        std::mt19937_64& generator);

Instance generate_avis(
        ItemPos number_of_items);

Instance generate_todd(
        ItemPos number_of_items);

Instance generate_somatoth(
        ItemPos number_of_items,
        std::mt19937_64& generator);

}
