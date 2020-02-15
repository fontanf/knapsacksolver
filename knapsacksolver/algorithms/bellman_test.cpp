#include "knapsacksolver/tester.hpp"
#include "knapsacksolver/algorithms/bellman.hpp"

using namespace knapsacksolver;

Output bellman_array_test(Instance& ins)
{
    Info info = Info()
        //.set_verbose(true)
        ;
    return bellman_array(ins, info);
}

Output bellmanpar_array_test(Instance& ins)
{
    Info info = Info()
        //.set_verbose(true)
        ;
    return bellmanpar_array(ins, info);
}

Output bellmanrec_test(Instance& ins)
{
    Info info = Info()
        //.set_verbose(true)
        ;
    return bellmanrec(ins, info);
}

Output bellman_array_all_test(Instance& ins)
{
    Info info = Info()
        //.set_verbose(true)
        ;
    return bellman_array_all(ins, info);
}

Output bellman_array_one_test(Instance& ins)
{
    Info info = Info()
        //.set_verbose(true)
        ;
    return bellman_array_one(ins, info);
}

Output bellman_array_part1_test(Instance& ins)
{
    Info info = Info()
        //.set_verbose(true)
        ;
    return bellman_array_part(ins, 1, info);
}

Output bellman_array_part2_test(Instance& ins)
{
    Info info = Info()
        //.set_verbose(true)
        ;
    return bellman_array_part(ins, 2, info);
}

Output bellman_array_part3_test(Instance& ins)
{
    Info info = Info()
        //.set_verbose(true)
        ;
    return bellman_array_part(ins, 3, info);
}

Output bellman_array_rec_test(Instance& ins)
{
    Info info = Info()
        //.set_verbose(true)
        ;
    return bellman_array_rec(ins, info);
}

Output bellman_list_test(Instance& ins)
{
    Info info = Info()
        //.set_verbose(true)
        ;
    return bellman_list(ins, false, info);
}

Output bellman_list_sort_test(Instance& ins)
{
    Info info = Info()
        //.set_verbose(true)
        ;
    return bellman_list(ins, true, info);
}

Output bellman_list_rec_test(Instance& ins)
{
    Info info = Info()
        //.set_verbose(true)
        ;
    return bellman_list_rec(ins, info);
}

std::vector<Output (*)(Instance&)> f_sopt {
        bellman_array_test,
        bellmanrec_test,
        bellman_array_all_test,
        bellman_array_one_test,
        bellman_array_part1_test,
        bellman_array_part2_test,
        bellman_array_part3_test,
        bellman_array_rec_test,
        bellman_list_rec_test,
};

std::vector<Output (*)(Instance&)> f_opt {
        bellman_array_test,
        bellmanpar_array_test,
        bellman_list_test,
        bellman_list_sort_test,
};

TEST(bellman, TEST_SOPT)  { test(TEST, f_sopt, SOPT); }
TEST(bellman, SMALL_SOPT) { test(SMALL, f_sopt, SOPT); }
TEST(bellman, TEST_OPT)  { test(TEST, f_opt, OPT); }
TEST(bellman, SMALL_OPT) { test(SMALL, f_opt, OPT); }

