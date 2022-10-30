#include "subsetsumsolver/generator.hpp"

#include <cmath>

using namespace subsetsumsolver;

template <typename T>
T gcd(T a, T b)
{
   if (b == 0)
       return a;
   return gcd(b, a % b);
}

Instance subsetsumsolver::generate_pthree(
        ItemPos number_of_items,
        std::mt19937_64& generator)
{
    Instance instance;
    std::uniform_int_distribution<Weight> distribution(1, 1e3);
    instance.set_capacity(number_of_items * 1e3 / 4);
    for (ItemPos pos = 0; pos < number_of_items; ++pos)
        instance.add_item(distribution(generator));
    return instance;
}

Instance subsetsumsolver::generate_psix(
        ItemPos number_of_items,
        std::mt19937_64& generator)
{
    Instance instance;
    std::uniform_int_distribution<Weight> distribution(1, 1e6);
    instance.set_capacity(number_of_items * 1e6 / 4);
    for (ItemPos pos = 0; pos < number_of_items; ++pos)
        instance.add_item(distribution(generator));
    return instance;
}

Instance subsetsumsolver::generate_evenodd(
        ItemPos number_of_items,
        std::mt19937_64& generator)
{
    Instance instance;
    std::uniform_int_distribution<Weight> distribution(1, 1e3 / 2);
    instance.set_capacity(2 * (number_of_items * 1e3 / 8) + 1);
    for (ItemPos pos = 0; pos < number_of_items; ++pos)
        instance.add_item(2 * distribution(generator));
    return instance;
}

Instance subsetsumsolver::generate_avis(
        ItemPos number_of_items)
{
    ItemPos n = number_of_items;
    Instance instance;
    std::uniform_int_distribution<Weight> distribution(1, 1e3);
    instance.set_capacity(n * (n + 1) * ((n - 1) / 2) + n * (n - 1) / 2);
    for (ItemPos pos = 0; pos < number_of_items; ++pos)
        instance.add_item(n * (n + 1) + pos);
    return instance;
}

Instance subsetsumsolver::generate_todd(
        ItemPos number_of_items)
{
    ItemPos n = number_of_items;
    Instance instance;
    std::uniform_int_distribution<Weight> distribution(1, 1e3);
    Weight w_sum = 0;
    for (ItemPos pos = 0; pos < number_of_items; ++pos) {
        int i1 = (int)std::floor(std::log(n)) + n + 1;
        int i2 = (int)std::floor(std::log(n)) + pos;
        Weight wj = (1 << i1) + (1 << i2) + 1;
        instance.add_item(wj);
        w_sum += wj;
    }
    instance.set_capacity(w_sum / 2);
    return instance;
}

Instance subsetsumsolver::generate_somatoth(
        ItemPos number_of_items,
        std::mt19937_64& generator)
{
    Instance instance;

    std::uniform_int_distribution<Weight> distribution(1, number_of_items);
    Weight w1;
    Weight w2;
    Weight c;
    for (;;) {
        w1 = distribution(generator);
        w2 = distribution(generator);
        std::cout << "w1 " << w1 << " w2 " << w2 << " gcd " << gcd(w1, w2) << std::endl;
        if (gcd(w1, w2) != 1)
            continue;

        c = (w1 - 1) * (w2 - 1) - 1;
        std::cout << c / w1 << " " << c / w2 << " " << number_of_items / 2 << std::endl;
        if (c / w1 >= number_of_items / 2)
            continue;
        if (c / w2 >= number_of_items / 2)
            continue;
    }

    instance.set_capacity(c);
    for (ItemPos pos = 0; pos < number_of_items; ++pos) {
        if (pos % 2 == 0) {
            instance.add_item(std::ceil((double)pos / 2) * w1);
        } else {
            instance.add_item(std::ceil((double)pos / 2) * w2);
        }
    }

    return instance;
}

Instance subsetsumsolver::generate_evenodd6(
        ItemPos number_of_items,
        std::mt19937_64& generator)
{
    Instance instance;
    std::uniform_int_distribution<Weight> distribution(1, 1e6 / 2);
    instance.set_capacity((number_of_items * 1e6 / 60) + 1);
    for (ItemPos pos = 0; pos < number_of_items; ++pos)
        instance.add_item(2 * distribution(generator));
    return instance;
}

Instance subsetsumsolver::generate_evenodd8(
        ItemPos number_of_items,
        std::mt19937_64& generator)
{
    Instance instance;
    std::uniform_int_distribution<Weight> distribution(1, 1e8 / 2);
    Weight w_sum = 0;
    for (ItemPos pos = 0; pos < number_of_items; ++pos) {
        Weight wj = 2 * distribution(generator);
        instance.add_item(wj);
        w_sum += wj;
    }
    instance.set_capacity(w_sum / 2 + 1);
    return instance;
}

Instance subsetsumsolver::generate_tenfive6(
        ItemPos number_of_items,
        std::mt19937_64& generator)
{
    Instance instance;
    std::uniform_int_distribution<Weight> distribution(1, 1e5);
    Weight w_sum = 0;
    for (ItemPos pos = 0; pos < number_of_items; ++pos) {
        Weight wj = 10 * distribution(generator);
        instance.add_item(wj);
        w_sum += wj;
    }
    instance.set_capacity((w_sum / 600) * 10 + 5);
    return instance;
}

Instance subsetsumsolver::generate_tenfive8(
        ItemPos number_of_items,
        std::mt19937_64& generator)
{
    Instance instance;
    std::uniform_int_distribution<Weight> distribution(1, 1e7);
    Weight w_sum = 0;
    for (ItemPos pos = 0; pos < number_of_items; ++pos) {
        Weight wj = 10 * distribution(generator);
        instance.add_item(wj);
        w_sum += wj;
    }
    instance.set_capacity((w_sum / 20) * 10 + 5);
    return instance;
}
