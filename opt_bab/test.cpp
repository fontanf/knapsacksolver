#include "knapsack/lib/tester.hpp"
#include "knapsack/opt_minknap/minknap.hpp"
#include "knapsack/opt_bab/bab.hpp"

using namespace knapsack;

knapsack::Output sopt_minknap_test(Instance& ins)
{
    return sopt_minknap(ins);
}

knapsack::Output sopt_bab_test(Instance& ins)
{
    Info info = Info()
        .set_verbose(false)
        ;
    return sopt_bab(ins, false, info);
}

knapsack::Output sopt_bab_sort_test(Instance& ins)
{
    Info info = Info()
        .set_verbose(false)
        ;
    return sopt_bab(ins, true, info);
}

std::vector<knapsack::Output (*)(Instance&)> f = {
        sopt_minknap_test,
        sopt_bab_test,
        sopt_bab_sort_test,
};

TEST(bab, TEST)  { test(TEST, f); }
TEST(bab, SMALL) { test(SMALL, f); }

