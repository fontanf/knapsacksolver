#include "knapsacksolver/tester.hpp"
#include "knapsacksolver/algorithms/minknap.hpp"
#include "knapsacksolver/algorithms/expknap.hpp"

using namespace knapsacksolver;

Output minknap_test(Instance& ins)
{
    Info info = Info()
        //.set_verbose(true)
        ;
    MinknapOptionalParameters p;
    p.info = info;
    return minknap(ins, p);
}

Output expknap_test(Instance& ins)
{
    Info info = Info()
        //.set_verbose(true)
        //.set_log2stderr(true)
        ;
    ExpknapOptionalParameters p;
    p.info = info;
    return expknap(ins, p);
}

Output expknap_combocore_test(Instance& ins)
{
    Info info = Info()
        //.set_verbose(true)
        //.set_log2stderr(true)
        ;
    ExpknapOptionalParameters p;
    p.info = info;
    p.combo_core = true;
    return expknap(ins, p);
}

std::vector<Output (*)(Instance&)> f = {
        minknap_test,
        expknap_test,
        expknap_combocore_test,
};

TEST(expknap, TEST)  { test(TEST, f, SOPT); }
TEST(expknap, SMALL) { test(SMALL, f, SOPT); }

