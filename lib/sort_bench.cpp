#include "knapsack/lib/instance.hpp"

using namespace knapsack;

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    ItemIdx n = 10000000;
    Weight c = 64 * n;
    Instance ins_eff(n, c);
    Instance ins_peff(n, c);
    std::vector<std::pair<Weight, Profit>> wp;
    for (ItemPos j=0; j<n; ++j)
        wp.push_back({j,1});
    std::random_shuffle(wp.begin(), wp.end());

    ins_eff.add_items(wp);
    ins_peff.add_items(wp);

    std::cout << "Conmplete sorting" << std::endl;
    ins_eff.sort();

    std::cout << "Partial sorting" << std::endl;
    ins_eff.sort_partially();

    return 0;
}
