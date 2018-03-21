#include "instance.hpp"

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    ItemIdx n = 10000000;
    Weight c = 64 * n;
    std::vector<Item> items(n);
    for (ItemPos i=0; i<n; ++i)
        items[i] = {i, i, 1};
    std::random_shuffle(items.begin(), items.end());

    Instance instance_eff(items, c);
    Instance instance_peff(items, c);

    std::cout << "Conmplete sorting" << std::endl;
    instance_eff.sort();

    std::cout << "Partial sorting" << std::endl;
    instance_eff.sort_partially();

    return 0;
}
