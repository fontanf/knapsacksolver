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

/**
 * See:
 * "A low-space algorithm for the subset-sum problem on GPU" (Curtis and
 * Sanches, 2017)
 * https://doi.org/10.1016/j.cor.2017.02.006
 */
Instance generate_evenodd6(
        ItemPos number_of_items,
        std::mt19937_64& generator);

/**
 * See:
 * "A low-space algorithm for the subset-sum problem on GPU" (Curtis and
 * Sanches, 2017)
 * https://doi.org/10.1016/j.cor.2017.02.006
 */
Instance generate_evenodd8(
        ItemPos number_of_items,
        std::mt19937_64& generator);

/**
 * See:
 * "A low-space algorithm for the subset-sum problem on GPU" (Curtis and
 * Sanches, 2017)
 * https://doi.org/10.1016/j.cor.2017.02.006
 */
Instance generate_tenfive6(
        ItemPos number_of_items,
        std::mt19937_64& generator);

/**
 * See:
 * "A low-space algorithm for the subset-sum problem on GPU" (Curtis and
 * Sanches, 2017)
 * https://doi.org/10.1016/j.cor.2017.02.006
 */
Instance generate_tenfive8(
        ItemPos number_of_items,
        std::mt19937_64& generator);

}
