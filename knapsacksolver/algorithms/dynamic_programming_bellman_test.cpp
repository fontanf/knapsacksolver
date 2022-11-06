#include "knapsacksolver/tester.hpp"
#include "knapsacksolver/algorithms/dynamic_programming_bellman.hpp"

using namespace knapsacksolver;

Output dynamic_programming_bellman_array_test(Instance& instance)
{
    Info info = Info()
        //.set_verbosity_level(1)
        ;
    return dynamic_programming_bellman_array(instance, info);
}

Output dynamic_programming_bellman_array_parallel_test(Instance& instance)
{
    Info info = Info()
        //.set_verbosity_level(1)
        ;
    return dynamic_programming_bellman_array_parallel(instance, info);
}

Output dynamic_programming_bellman_rec_test(Instance& instance)
{
    Info info = Info()
        //.set_verbosity_level(1)
        ;
    return dynamic_programming_bellman_rec(instance, info);
}

Output dynamic_programming_bellman_array_all_test(Instance& instance)
{
    Info info = Info()
        //.set_verbosity_level(1)
        ;
    return dynamic_programming_bellman_array_all(instance, info);
}

Output dynamic_programming_bellman_array_one_test(Instance& instance)
{
    Info info = Info()
        //.set_verbosity_level(1)
        ;
    return dynamic_programming_bellman_array_one(instance, info);
}

Output dynamic_programming_bellman_array_part1_test(Instance& instance)
{
    Info info = Info()
        //.set_verbosity_level(1)
        ;
    return dynamic_programming_bellman_array_part(instance, 1, info);
}

Output dynamic_programming_bellman_array_part2_test(Instance& instance)
{
    Info info = Info()
        //.set_verbosity_level(1)
        ;
    return dynamic_programming_bellman_array_part(instance, 2, info);
}

Output dynamic_programming_bellman_array_part3_test(Instance& instance)
{
    Info info = Info()
        //.set_verbosity_level(1)
        ;
    return dynamic_programming_bellman_array_part(instance, 3, info);
}

Output dynamic_programming_bellman_array_rec_test(Instance& instance)
{
    Info info = Info()
        //.set_verbosity_level(1)
        ;
    return dynamic_programming_bellman_array_rec(instance, info);
}

Output dynamic_programming_bellman_list_test(Instance& instance)
{
    Info info = Info()
        //.set_verbosity_level(1)
        ;
    return dynamic_programming_bellman_list(instance, false, info);
}

Output dynamic_programming_bellman_list_sort_test(Instance& instance)
{
    Info info = Info()
        //.set_verbosity_level(1)
        ;
    return dynamic_programming_bellman_list(instance, true, info);
}

Output dynamic_programming_bellman_list_rec_test(Instance& instance)
{
    Info info = Info()
        //.set_verbosity_level(1)
        ;
    return dynamic_programming_bellman_list_rec(instance, info);
}

std::vector<Output (*)(Instance&)> f_sopt {
        dynamic_programming_bellman_array_test,
        dynamic_programming_bellman_rec_test,
        dynamic_programming_bellman_array_all_test,
        dynamic_programming_bellman_array_one_test,
        dynamic_programming_bellman_array_part1_test,
        dynamic_programming_bellman_array_part2_test,
        dynamic_programming_bellman_array_part3_test,
        dynamic_programming_bellman_array_rec_test,
        dynamic_programming_bellman_list_rec_test,
};

std::vector<Output (*)(Instance&)> f_opt {
        dynamic_programming_bellman_array_test,
        dynamic_programming_bellman_array_parallel_test,
        dynamic_programming_bellman_list_test,
        dynamic_programming_bellman_list_sort_test,
};

TEST(bellman, TEST_SOPT)  { test(TEST, f_sopt, SOPT); }
TEST(bellman, SMALL_SOPT) { test(SMALL, f_sopt, SOPT); }
TEST(bellman, TEST_OPT)  { test(TEST, f_opt, OPT); }
TEST(bellman, SMALL_OPT) { test(SMALL, f_opt, OPT); }

