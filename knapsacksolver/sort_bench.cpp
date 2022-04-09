#include "knapsacksolver/instance.hpp"

using namespace knapsacksolver;

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    ItemIdx n = 10000000;
    Weight c = 64 * n;
    Instance ins_eff;
    Instance ins_peff;
    ins_eff.set_capacity(c);
    ins_peff.set_capacity(c);

    std::vector<Weight> w(n);
    std::iota(w.begin(), w.end(), 0);
    std::random_shuffle(w.begin(), w.end());
    for (Weight weight: w) {
        ins_eff.add_item(weight, 1);
        ins_peff.add_item(weight, 1);
    }

    Info info_1;
    std::cout << "Conmplete sorting" << std::endl;
    ins_eff.sort(FFOT_DBG(info_1));

    Info info_2;
    std::cout << "Partial sorting" << std::endl;
    ins_eff.sort_partially(FFOT_DBG(info_2));

    return 0;
}

