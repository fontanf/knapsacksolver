#include "knapsack/lib/tester.hpp"
#include "knapsack/opt_minknap/minknap.hpp"
#include "knapsack/opt_expknap/expknap.hpp"

using namespace knapsack;

knapsack::Output sopt_minknap_test(Instance& ins)
{
    Info info = Info()
        //.set_verbose(true)
        ;
    MinknapOptionalParameters p;
    p.info = info;
    return sopt_minknap(ins, p);
}

knapsack::Output sopt_expknap_test(Instance& ins)
{
    Info info = Info()
        //.set_verbose(true)
        //.set_log2stderr(true)
        ;
    ExpknapOptionalParameters p;
    p.info = info;
    return sopt_expknap(ins, p);
}

knapsack::Output sopt_expknap_combocore_test(Instance& ins)
{
    Info info = Info()
        //.set_verbose(true)
        //.set_log2stderr(true)
        ;
    ExpknapOptionalParameters p;
    p.info = info;
    p.combo_core = true;
    return sopt_expknap(ins, p);
}

std::vector<knapsack::Output (*)(Instance&)> f = {
        sopt_minknap_test,
        sopt_expknap_test,
        sopt_expknap_combocore_test,
};

TEST(expknap, TEST)  { test(TEST, f, SOPT); }
TEST(expknap, SMALL) { test(SMALL, f, SOPT); }

