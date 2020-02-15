#include "knapsacksolver/tester.hpp"
#include "knapsacksolver/algorithms/minknap.hpp"
#include "knapsacksolver/algorithms/branchandbound.hpp"

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

Output branchandbound_test(Instance& ins)
{
    Info info = Info()
        //.set_verbose(true)
        ;
    return branchandbound(ins, false, info);
}

Output branchandbound_sort_test(Instance& ins)
{
    Info info = Info()
        //.set_verbose(true)
        ;
    return branchandbound(ins, true, info);
}

std::vector<Output (*)(Instance&)> f = {
        minknap_test,
        branchandbound_test,
        branchandbound_sort_test,
};

TEST(bab, TEST)  { test(TEST, f); }
TEST(bab, SMALL) { test(SMALL, f); }

