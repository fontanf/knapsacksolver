#include "knapsacksolver/subset_sum/tests.hpp"

#include "knapsacksolver/subset_sum/algorithms/dynamic_programming_bellman.hpp"

using namespace knapsacksolver;
using namespace knapsacksolver::subset_sum;

TEST_P(ExactAlgorithmTest, ExactAlgorithm)
{
    TestParams test_params = GetParam();
    const Instance instance = get_instance(test_params.files);
    const Solution solution = get_solution(instance, test_params.files);
    auto output = test_params.algorithm(instance);
    EXPECT_EQ(output.value, solution.weight());
    EXPECT_EQ(output.value, output.solution.weight());
    EXPECT_EQ(output.bound, solution.weight());
}

TEST_P(ExactNoSolutionAlgorithmTest, ExactNoSolutionAlgorithm)
{
    TestParams test_params = GetParam();
    const Instance instance = get_instance(test_params.files);
    const Solution solution = get_solution(instance, test_params.files);
    auto output = test_params.algorithm(instance);
    EXPECT_EQ(output.value, solution.weight());
    EXPECT_EQ(output.bound, solution.weight());
}

INSTANTIATE_TEST_SUITE_P(
        SubsetSumDynamicProgrammingBellmanArray,
        ExactAlgorithmTest,
        testing::ValuesIn(get_test_params(
                {
                    [](const Instance& instance)
                    {
                        return dynamic_programming_bellman_array(instance);
                    },
                },
                {
                    get_pthree_instance_paths(10),
                    get_pthree_instance_paths(30),
                    get_pthree_instance_paths(100),
                    get_pthree_instance_paths(300),
                    get_pthree_instance_paths(1000),
                    get_psix_instance_paths(10),
                })));

INSTANTIATE_TEST_SUITE_P(
        SubsetSumDynamicProgrammingBellmanList,
        ExactNoSolutionAlgorithmTest,
        testing::ValuesIn(get_test_params(
                {
                    [](const Instance& instance)
                    {
                        return dynamic_programming_bellman_list(instance);
                    },
                },
                {
                    get_pthree_instance_paths(10),
                    get_pthree_instance_paths(30),
                    get_pthree_instance_paths(100),
                    get_pthree_instance_paths(300),
                    get_pthree_instance_paths(1000),
                    get_psix_instance_paths(10),
                })));

INSTANTIATE_TEST_SUITE_P(
        SubsetSumDynamicProgrammingBellmanWordRam,
        ExactNoSolutionAlgorithmTest,
        testing::ValuesIn(get_test_params(
                {
                    [](const Instance& instance)
                    {
                        return dynamic_programming_bellman_word_ram(instance);
                    },
                },
                {
                    get_pthree_instance_paths(10),
                    get_pthree_instance_paths(30),
                    get_pthree_instance_paths(100),
                    get_pthree_instance_paths(300),
                    get_pthree_instance_paths(1000),
                    get_psix_instance_paths(10),
                })));

INSTANTIATE_TEST_SUITE_P(
        SubsetSumDynamicProgrammingBellmanWordRamRec,
        ExactAlgorithmTest,
        testing::ValuesIn(get_test_params(
                {
                    [](const Instance& instance)
                    {
                        return dynamic_programming_bellman_word_ram_rec(instance);
                    },
                },
                {
                    get_pthree_instance_paths(10),
                    get_pthree_instance_paths(30),
                    get_pthree_instance_paths(100),
                    get_pthree_instance_paths(300),
                    get_pthree_instance_paths(1000),
                    get_psix_instance_paths(10),
                })));
