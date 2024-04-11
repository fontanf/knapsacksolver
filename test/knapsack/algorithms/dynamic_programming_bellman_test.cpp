#include "knapsacksolver/knapsack/tests.hpp"

#include "knapsacksolver/knapsack/algorithms/dynamic_programming_bellman.hpp"

using namespace knapsacksolver;
using namespace knapsacksolver::knapsack;

TEST_P(ExactAlgorithmTest, ExactAlgorithm)
{
    TestParams test_params = GetParam();
    const Instance instance = get_instance(test_params.files);
    const Solution solution = get_solution(instance, test_params.files);
    auto output = test_params.algorithm(instance);
    EXPECT_EQ(output.value, solution.profit());
    EXPECT_EQ(output.value, output.solution.profit());
    EXPECT_EQ(output.bound, solution.profit());
}

TEST_P(ExactNoSolutionAlgorithmTest, ExactNoSolutionAlgorithm)
{
    TestParams test_params = GetParam();
    const Instance instance = get_instance(test_params.files);
    const Solution solution = get_solution(instance, test_params.files);
    auto output = test_params.algorithm(instance);
    EXPECT_EQ(output.value, solution.profit());
    EXPECT_EQ(output.bound, solution.profit());
}

INSTANTIATE_TEST_SUITE_P(
        DynamicProgrammingBellmanArray,
        ExactNoSolutionAlgorithmTest,
        testing::ValuesIn(get_test_params(
                {
                    [](const Instance& instance)
                    {
                        return dynamic_programming_bellman_array(instance);
                    },
                },
                {
                    get_test_instance_paths(),
                    get_pisinger_instance_paths("smallcoeff", "knapPI_1_50_1000"),
                })));

INSTANTIATE_TEST_SUITE_P(
        DynamicProgrammingBellmanNoSolution,
        ExactNoSolutionAlgorithmTest,
        testing::ValuesIn(get_test_params(
                {
                    [](const Instance& instance)
                    {
                        return dynamic_programming_bellman_rec(instance);
                    },
                    [](const Instance& instance)
                    {
                        return dynamic_programming_bellman_array(instance);
                    },
                    [](const Instance& instance)
                    {
                        return dynamic_programming_bellman_array_parallel(instance);
                    },
                    [](const Instance& instance)
                    {
                        return dynamic_programming_bellman_list(instance);
                    },
                    [](const Instance& instance)
                    {
                        DynamicProgrammingBellmanListParameters parameters;
                        parameters.sort = true;
                        return dynamic_programming_bellman_list(instance, parameters);
                    },
                },
                {
                    get_test_instance_paths(),
                })));

INSTANTIATE_TEST_SUITE_P(
        DynamicProgrammingBellman,
        ExactAlgorithmTest,
        testing::ValuesIn(get_test_params(
                {
                    [](const Instance& instance)
                    {
                        return dynamic_programming_bellman_array_all(instance);
                    },
                    [](const Instance& instance)
                    {
                        return dynamic_programming_bellman_array_one(instance);
                    },
                    [](const Instance& instance)
                    {
                        DynamicProgrammingBellmanArrayPartParameters parameters;
                        parameters.partial_solution_size = 1;
                        return dynamic_programming_bellman_array_part(instance, parameters);
                    },
                    [](const Instance& instance)
                    {
                        DynamicProgrammingBellmanArrayPartParameters parameters;
                        parameters.partial_solution_size = 2;
                        return dynamic_programming_bellman_array_part(instance, parameters);
                    },
                    [](const Instance& instance)
                    {
                        DynamicProgrammingBellmanArrayPartParameters parameters;
                        parameters.partial_solution_size = 3;
                        return dynamic_programming_bellman_array_part(instance, parameters);
                    },
                    [](const Instance& instance)
                    {
                        return dynamic_programming_bellman_array_part(instance);
                    },
                    [](const Instance& instance)
                    {
                        return dynamic_programming_bellman_array_rec(instance);
                    },
                },
                {
                    get_test_instance_paths(),
                })));
