#include "knapsacksolver/tests.hpp"

#include "knapsacksolver/algorithms/dynamic_programming_primal_dual.hpp"
#include "knapsacksolver/instance_builder.hpp"

using namespace knapsacksolver;

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

INSTANTIATE_TEST_SUITE_P(
        KnapsackDynamicProgrammingPrimalDualPartialSolutionSize,
        ExactAlgorithmTest,
        testing::ValuesIn(get_test_params(
                {
                    [](const Instance& instance)
                    {
                        DynamicProgrammingPrimalDualParameters parameters;
                        parameters.partial_solution_size = 1;
                        std::mt19937_64 generator(0);
                        return dynamic_programming_primal_dual(instance, generator, parameters);
                    },
                    [](const Instance& instance)
                    {
                        DynamicProgrammingPrimalDualParameters parameters;
                        parameters.partial_solution_size = 2;
                        std::mt19937_64 generator(0);
                        return dynamic_programming_primal_dual(instance, generator, parameters);
                    },
                    [](const Instance& instance)
                    {
                        DynamicProgrammingPrimalDualParameters parameters;
                        parameters.partial_solution_size = 3;
                        std::mt19937_64 generator(0);
                        return dynamic_programming_primal_dual(instance, generator, parameters);
                    },
                },
                {get_test_instance_paths()})));

INSTANTIATE_TEST_SUITE_P(
        KnapsackDynamicProgrammingPrimalDual,
        ExactAlgorithmTest,
        testing::ValuesIn(get_test_params(
                {
                    [](const Instance& instance)
                    {
                        DynamicProgrammingPrimalDualParameters parameters;
                        parameters.verbosity_level = 0;
                        std::mt19937_64 generator(0);
                        return dynamic_programming_primal_dual(instance, generator, parameters);
                    },
                },
                {
                    get_test_instance_paths(),
                    get_pisinger_instance_paths("smallcoeff", "knapPI_1_50_1000"),
                    get_pisinger_instance_paths("smallcoeff", "knapPI_1_50_10000"),
                    get_pisinger_instance_paths("smallcoeff", "knapPI_1_100_1000"),
                    get_pisinger_instance_paths("smallcoeff", "knapPI_1_100_10000"),
                    get_pisinger_instance_paths("smallcoeff", "knapPI_1_200_1000"),
                    get_pisinger_instance_paths("smallcoeff", "knapPI_1_200_10000"),
                    get_pisinger_instance_paths("smallcoeff", "knapPI_1_500_1000"),
                    get_pisinger_instance_paths("smallcoeff", "knapPI_1_500_10000"),
                    get_pisinger_instance_paths("smallcoeff", "knapPI_1_1000_1000"),
                    get_pisinger_instance_paths("smallcoeff", "knapPI_1_1000_10000"),
                    get_pisinger_instance_paths("smallcoeff", "knapPI_1_2000_1000"),
                    get_pisinger_instance_paths("smallcoeff", "knapPI_1_2000_10000"),
                    get_pisinger_instance_paths("smallcoeff", "knapPI_1_5000_1000"),
                    get_pisinger_instance_paths("smallcoeff", "knapPI_1_5000_10000"),
                    get_pisinger_instance_paths("smallcoeff", "knapPI_1_10000_1000"),
                    get_pisinger_instance_paths("smallcoeff", "knapPI_1_10000_10000"),
                    get_pisinger_instance_paths("smallcoeff", "knapPI_2_50_1000"),
                    get_pisinger_instance_paths("smallcoeff", "knapPI_2_50_10000"),
                    get_pisinger_instance_paths("smallcoeff", "knapPI_2_100_1000"),
                    get_pisinger_instance_paths("smallcoeff", "knapPI_2_100_10000"),
                    get_pisinger_instance_paths("smallcoeff", "knapPI_2_200_1000"),
                    get_pisinger_instance_paths("smallcoeff", "knapPI_2_200_10000"),
                    get_pisinger_instance_paths("smallcoeff", "knapPI_2_500_1000"),
                    get_pisinger_instance_paths("smallcoeff", "knapPI_2_500_10000"),
                    get_pisinger_instance_paths("smallcoeff", "knapPI_2_1000_1000"),
                    get_pisinger_instance_paths("smallcoeff", "knapPI_2_1000_10000"),
                    get_pisinger_instance_paths("smallcoeff", "knapPI_2_2000_1000"),
                    get_pisinger_instance_paths("smallcoeff", "knapPI_2_2000_10000"),
                    get_pisinger_instance_paths("smallcoeff", "knapPI_2_5000_1000"),
                    get_pisinger_instance_paths("smallcoeff", "knapPI_2_5000_10000"),
                    get_pisinger_instance_paths("smallcoeff", "knapPI_2_10000_1000"),
                    get_pisinger_instance_paths("smallcoeff", "knapPI_2_10000_10000"),
                    get_pisinger_instance_paths("smallcoeff", "knapPI_3_50_1000"),
                    get_pisinger_instance_paths("smallcoeff", "knapPI_3_50_10000"),
                    get_pisinger_instance_paths("smallcoeff", "knapPI_3_100_1000"),
                    get_pisinger_instance_paths("smallcoeff", "knapPI_3_100_10000"),
                    get_pisinger_instance_paths("smallcoeff", "knapPI_4_50_1000"),
                    get_pisinger_instance_paths("smallcoeff", "knapPI_4_50_10000"),
                    get_pisinger_instance_paths("smallcoeff", "knapPI_4_100_1000"),
                    get_pisinger_instance_paths("smallcoeff", "knapPI_4_100_10000"),
                    get_pisinger_instance_paths("smallcoeff", "knapPI_5_50_1000"),
                    get_pisinger_instance_paths("smallcoeff", "knapPI_5_50_10000"),
                    get_pisinger_instance_paths("smallcoeff", "knapPI_5_100_1000"),
                    get_pisinger_instance_paths("smallcoeff", "knapPI_5_100_10000"),
                    get_pisinger_instance_paths("smallcoeff", "knapPI_6_50_1000"),
                    get_pisinger_instance_paths("smallcoeff", "knapPI_6_50_10000"),
                    get_pisinger_instance_paths("smallcoeff", "knapPI_6_100_1000"),
                    get_pisinger_instance_paths("smallcoeff", "knapPI_6_100_10000"),
                    get_pisinger_instance_paths("smallcoeff", "knapPI_9_50_1000"),
                    get_pisinger_instance_paths("smallcoeff", "knapPI_9_100_1000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_1_50_100000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_1_50_1000000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_1_50_10000000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_1_100_100000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_1_100_1000000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_1_100_10000000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_1_200_100000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_1_200_1000000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_1_200_10000000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_1_500_100000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_1_500_1000000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_1_500_10000000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_1_1000_100000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_1_1000_1000000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_1_1000_10000000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_1_2000_100000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_1_2000_1000000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_1_2000_10000000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_1_5000_100000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_1_5000_1000000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_1_5000_10000000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_1_10000_100000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_1_10000_1000000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_1_10000_10000000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_2_50_100000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_2_50_1000000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_2_50_10000000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_2_100_100000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_2_100_1000000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_2_100_10000000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_2_200_100000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_2_200_1000000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_2_200_10000000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_2_500_100000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_2_500_1000000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_2_500_10000000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_2_1000_100000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_2_1000_1000000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_2_1000_10000000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_2_2000_100000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_2_2000_1000000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_2_2000_10000000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_2_5000_100000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_2_5000_1000000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_2_5000_10000000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_2_10000_100000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_2_10000_1000000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_2_10000_10000000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_3_50_100000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_3_50_1000000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_3_50_10000000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_3_100_100000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_4_50_100000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_4_50_1000000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_4_50_10000000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_4_100_100000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_5_50_100000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_5_50_1000000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_5_50_10000000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_5_100_100000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_5_100_1000000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_5_100_10000000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_6_50_100000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_6_100_100000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_9_50_100000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_9_50_1000000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_9_100_100000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_9_100_1000000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_9_200_100000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_9_200_1000000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_9_500_100000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_9_500_1000000"),
                    get_pisinger_instance_paths("hardinstances", "knapPI_11_20_1000"),
                    get_pisinger_instance_paths("hardinstances", "knapPI_11_50_1000"),
                    get_pisinger_instance_paths("hardinstances", "knapPI_11_100_1000"),
                    get_pisinger_instance_paths("hardinstances", "knapPI_12_20_1000"),
                    get_pisinger_instance_paths("hardinstances", "knapPI_12_50_1000"),
                    get_pisinger_instance_paths("hardinstances", "knapPI_12_100_1000"),
                    get_pisinger_instance_paths("hardinstances", "knapPI_13_20_1000"),
                    get_pisinger_instance_paths("hardinstances", "knapPI_13_50_1000"),
                    get_pisinger_instance_paths("hardinstances", "knapPI_13_100_1000"),
                    get_pisinger_instance_paths("hardinstances", "knapPI_14_20_1000"),
                    get_pisinger_instance_paths("hardinstances", "knapPI_14_50_1000"),
                    get_pisinger_instance_paths("hardinstances", "knapPI_14_100_1000"),
                    get_pisinger_instance_paths("hardinstances", "knapPI_15_20_1000"),
                    get_pisinger_instance_paths("hardinstances", "knapPI_15_50_1000"),
                    get_pisinger_instance_paths("hardinstances", "knapPI_15_100_1000"),
                    get_pisinger_instance_paths("hardinstances", "knapPI_16_20_1000"),
                    get_pisinger_instance_paths("hardinstances", "knapPI_16_50_1000"),
                    get_pisinger_instance_paths("hardinstances", "knapPI_16_100_1000"),
                })));

// The pairing strategy makes the algorithm able to solve all the random subset
// sum instances.
INSTANTIATE_TEST_SUITE_P(
        DynamicProgrammingPrimalDualPairing,
        ExactAlgorithmTest,
        testing::ValuesIn(get_test_params(
                {
                    [](const Instance& instance)
                    {
                        DynamicProgrammingPrimalDualParameters parameters;
                        parameters.pairing = true;
                        parameters.verbosity_level = 0;
                        std::mt19937_64 generator(0);
                        return dynamic_programming_primal_dual(instance, generator, parameters);
                    },
                },
                {
                    get_test_instance_paths(),
                    get_pisinger_instance_paths("smallcoeff", "knapPI_1_50_1000"),
                    get_pisinger_instance_paths("smallcoeff", "knapPI_1_50_10000"),
                    get_pisinger_instance_paths("smallcoeff", "knapPI_1_100_1000"),
                    get_pisinger_instance_paths("smallcoeff", "knapPI_1_100_10000"),
                    get_pisinger_instance_paths("smallcoeff", "knapPI_1_200_1000"),
                    get_pisinger_instance_paths("smallcoeff", "knapPI_1_200_10000"),
                    get_pisinger_instance_paths("smallcoeff", "knapPI_1_500_1000"),
                    get_pisinger_instance_paths("smallcoeff", "knapPI_1_500_10000"),
                    get_pisinger_instance_paths("smallcoeff", "knapPI_1_1000_1000"),
                    get_pisinger_instance_paths("smallcoeff", "knapPI_1_1000_10000"),
                    get_pisinger_instance_paths("smallcoeff", "knapPI_1_2000_1000"),
                    get_pisinger_instance_paths("smallcoeff", "knapPI_1_2000_10000"),
                    get_pisinger_instance_paths("smallcoeff", "knapPI_1_5000_1000"),
                    get_pisinger_instance_paths("smallcoeff", "knapPI_1_5000_10000"),
                    get_pisinger_instance_paths("smallcoeff", "knapPI_1_10000_1000"),
                    get_pisinger_instance_paths("smallcoeff", "knapPI_1_10000_10000"),
                    get_pisinger_instance_paths("smallcoeff", "knapPI_2_50_1000"),
                    get_pisinger_instance_paths("smallcoeff", "knapPI_2_50_10000"),
                    get_pisinger_instance_paths("smallcoeff", "knapPI_2_100_1000"),
                    get_pisinger_instance_paths("smallcoeff", "knapPI_2_100_10000"),
                    get_pisinger_instance_paths("smallcoeff", "knapPI_2_200_1000"),
                    get_pisinger_instance_paths("smallcoeff", "knapPI_2_200_10000"),
                    get_pisinger_instance_paths("smallcoeff", "knapPI_2_500_1000"),
                    get_pisinger_instance_paths("smallcoeff", "knapPI_2_500_10000"),
                    get_pisinger_instance_paths("smallcoeff", "knapPI_2_1000_1000"),
                    get_pisinger_instance_paths("smallcoeff", "knapPI_2_1000_10000"),
                    get_pisinger_instance_paths("smallcoeff", "knapPI_2_2000_1000"),
                    get_pisinger_instance_paths("smallcoeff", "knapPI_2_2000_10000"),
                    get_pisinger_instance_paths("smallcoeff", "knapPI_2_5000_1000"),
                    get_pisinger_instance_paths("smallcoeff", "knapPI_2_5000_10000"),
                    get_pisinger_instance_paths("smallcoeff", "knapPI_2_10000_1000"),
                    get_pisinger_instance_paths("smallcoeff", "knapPI_2_10000_10000"),
                    get_pisinger_instance_paths("smallcoeff", "knapPI_3_50_1000"),
                    get_pisinger_instance_paths("smallcoeff", "knapPI_3_50_10000"),
                    get_pisinger_instance_paths("smallcoeff", "knapPI_3_100_1000"),
                    get_pisinger_instance_paths("smallcoeff", "knapPI_3_100_10000"),
                    get_pisinger_instance_paths("smallcoeff", "knapPI_4_50_1000"),
                    get_pisinger_instance_paths("smallcoeff", "knapPI_4_50_10000"),
                    get_pisinger_instance_paths("smallcoeff", "knapPI_4_100_1000"),
                    get_pisinger_instance_paths("smallcoeff", "knapPI_4_100_10000"),
                    get_pisinger_instance_paths("smallcoeff", "knapPI_5_50_1000"),
                    get_pisinger_instance_paths("smallcoeff", "knapPI_5_50_10000"),
                    get_pisinger_instance_paths("smallcoeff", "knapPI_5_100_1000"),
                    get_pisinger_instance_paths("smallcoeff", "knapPI_5_100_10000"),
                    get_pisinger_instance_paths("smallcoeff", "knapPI_6_50_1000"),
                    get_pisinger_instance_paths("smallcoeff", "knapPI_6_50_10000"),
                    get_pisinger_instance_paths("smallcoeff", "knapPI_6_100_1000"),
                    get_pisinger_instance_paths("smallcoeff", "knapPI_6_100_10000"),
                    get_pisinger_instance_paths("smallcoeff", "knapPI_6_200_1000"),
                    get_pisinger_instance_paths("smallcoeff", "knapPI_6_200_10000"),
                    get_pisinger_instance_paths("smallcoeff", "knapPI_6_500_1000"),
                    get_pisinger_instance_paths("smallcoeff", "knapPI_6_500_10000"),
                    get_pisinger_instance_paths("smallcoeff", "knapPI_6_1000_1000"),
                    get_pisinger_instance_paths("smallcoeff", "knapPI_6_1000_10000"),
                    get_pisinger_instance_paths("smallcoeff", "knapPI_6_2000_1000"),
                    get_pisinger_instance_paths("smallcoeff", "knapPI_6_2000_10000"),
                    get_pisinger_instance_paths("smallcoeff", "knapPI_6_5000_1000"),
                    get_pisinger_instance_paths("smallcoeff", "knapPI_6_5000_10000"),
                    get_pisinger_instance_paths("smallcoeff", "knapPI_6_10000_1000"),
                    get_pisinger_instance_paths("smallcoeff", "knapPI_6_10000_10000"),
                    get_pisinger_instance_paths("smallcoeff", "knapPI_9_50_1000"),
                    get_pisinger_instance_paths("smallcoeff", "knapPI_9_100_1000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_1_50_100000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_1_50_1000000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_1_50_10000000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_1_100_100000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_1_100_1000000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_1_100_10000000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_1_200_100000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_1_200_1000000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_1_200_10000000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_1_500_100000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_1_500_1000000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_1_500_10000000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_1_1000_100000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_1_1000_1000000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_1_1000_10000000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_1_2000_100000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_1_2000_1000000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_1_2000_10000000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_1_5000_100000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_1_5000_1000000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_1_5000_10000000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_1_10000_100000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_1_10000_1000000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_1_10000_10000000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_2_50_100000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_2_50_1000000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_2_50_10000000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_2_100_100000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_2_100_1000000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_2_100_10000000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_2_200_100000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_2_200_1000000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_2_200_10000000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_2_500_100000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_2_500_1000000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_2_500_10000000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_2_1000_100000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_2_1000_1000000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_2_1000_10000000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_2_2000_100000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_2_2000_1000000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_2_2000_10000000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_2_5000_100000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_2_5000_1000000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_2_5000_10000000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_2_10000_100000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_2_10000_1000000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_2_10000_10000000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_3_50_100000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_3_50_1000000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_3_50_10000000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_3_100_100000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_4_50_100000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_4_50_1000000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_4_50_10000000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_4_100_100000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_5_50_100000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_5_50_1000000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_5_50_10000000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_5_100_100000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_5_100_1000000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_5_100_10000000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_6_50_100000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_6_50_1000000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_6_50_10000000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_6_100_100000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_6_100_1000000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_6_100_10000000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_6_200_100000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_6_200_1000000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_6_200_10000000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_6_500_100000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_6_500_1000000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_6_500_10000000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_6_1000_100000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_6_1000_1000000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_6_1000_10000000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_6_2000_100000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_6_2000_1000000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_6_2000_10000000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_6_5000_100000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_6_5000_1000000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_6_5000_10000000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_6_10000_100000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_6_10000_1000000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_6_10000_10000000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_9_50_100000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_9_50_1000000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_9_100_100000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_9_100_1000000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_9_200_100000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_9_200_1000000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_9_500_100000"),
                    get_pisinger_instance_paths("largecoeff", "knapPI_9_500_1000000"),
                    get_pisinger_instance_paths("hardinstances", "knapPI_11_20_1000"),
                    get_pisinger_instance_paths("hardinstances", "knapPI_11_50_1000"),
                    get_pisinger_instance_paths("hardinstances", "knapPI_11_100_1000"),
                    get_pisinger_instance_paths("hardinstances", "knapPI_12_20_1000"),
                    get_pisinger_instance_paths("hardinstances", "knapPI_12_50_1000"),
                    get_pisinger_instance_paths("hardinstances", "knapPI_12_100_1000"),
                    get_pisinger_instance_paths("hardinstances", "knapPI_13_20_1000"),
                    get_pisinger_instance_paths("hardinstances", "knapPI_13_50_1000"),
                    get_pisinger_instance_paths("hardinstances", "knapPI_13_100_1000"),
                    get_pisinger_instance_paths("hardinstances", "knapPI_14_20_1000"),
                    get_pisinger_instance_paths("hardinstances", "knapPI_14_50_1000"),
                    get_pisinger_instance_paths("hardinstances", "knapPI_14_100_1000"),
                    get_pisinger_instance_paths("hardinstances", "knapPI_15_20_1000"),
                    get_pisinger_instance_paths("hardinstances", "knapPI_15_50_1000"),
                    get_pisinger_instance_paths("hardinstances", "knapPI_15_100_1000"),
                    get_pisinger_instance_paths("hardinstances", "knapPI_16_20_1000"),
                    get_pisinger_instance_paths("hardinstances", "knapPI_16_50_1000"),
                    get_pisinger_instance_paths("hardinstances", "knapPI_16_100_1000"),
                })));

struct DynamicProgrammingPrimalDualMaximumNumberOfStatesTestParams
{
    /** Name of the test. */
    std::string name;

    /** Profits of the items. */
    std::vector<Profit> profits;

    /** Weights of the items. */
    std::vector<Weight> weights;

    /** Capacity of the knapsack. */
    Weight capacity;

    /** Maximum number of states. */
    StateId maximum_number_of_states;

    /** True iff the maximum number of states is expected to be reached. */
    bool expected_maximum_number_of_states_reached;
};

inline std::ostream& operator<<(std::ostream& os, const DynamicProgrammingPrimalDualMaximumNumberOfStatesTestParams& test_params)
{
    os << test_params.name << " " << test_params.maximum_number_of_states;
    return os;
}

class DynamicProgrammingPrimalDualMaximumNumberOfStatesTest: public testing::TestWithParam<DynamicProgrammingPrimalDualMaximumNumberOfStatesTestParams> { };

TEST_P(DynamicProgrammingPrimalDualMaximumNumberOfStatesTest, DynamicProgrammingPrimalDualMaximumNumberOfStates)
{
    DynamicProgrammingPrimalDualMaximumNumberOfStatesTestParams test_params = GetParam();
    InstanceBuilder instance_builder;
    instance_builder.set_capacity(test_params.capacity);
    for (ItemId item_id = 0;
            item_id < (ItemId)test_params.profits.size();
            ++item_id) {
        instance_builder.add_item(
                test_params.profits[item_id],
                test_params.weights[item_id]);
    }
    const Instance instance = instance_builder.build();

    std::mt19937_64 reference_generator(0);
    DynamicProgrammingPrimalDualParameters reference_parameters;
    reference_parameters.verbosity_level = 0;
    auto reference_output = dynamic_programming_primal_dual(instance, reference_generator, reference_parameters);

    std::mt19937_64 generator(0);
    DynamicProgrammingPrimalDualParameters parameters;
    parameters.verbosity_level = 0;
    parameters.maximum_number_of_states = test_params.maximum_number_of_states;
    auto output = dynamic_programming_primal_dual(instance, generator, parameters);

    EXPECT_EQ(
            output.maximum_number_of_states_reached,
            test_params.expected_maximum_number_of_states_reached);
    // The solution is feasible and the bound is valid.
    EXPECT_TRUE(output.solution.feasible());
    EXPECT_EQ(output.value >= output.solution.profit(), true);
    EXPECT_EQ(output.value <= reference_output.value, true);
    EXPECT_EQ(output.bound >= reference_output.value, true);
    if (!test_params.expected_maximum_number_of_states_reached) {
        EXPECT_EQ(output.solution.profit(), reference_output.value);
        EXPECT_EQ(output.bound, reference_output.value);
    }
}

INSTANTIATE_TEST_SUITE_P(
        KnapsackDynamicProgrammingPrimalDualMaximumNumberOfStates,
        DynamicProgrammingPrimalDualMaximumNumberOfStatesTest,
        testing::ValuesIn(std::vector<DynamicProgrammingPrimalDualMaximumNumberOfStatesTestParams>{
            {
                // Subset sum instance (profits equal to weights): the upper
                // bound of each state is the capacity, so that states are only
                // pruned once a solution fills the knapsack exactly.
                "subset_sum",
                {1001, 2003, 4007, 8009, 16001, 32003, 64007, 128021, 256019, 512009},
                {1001, 2003, 4007, 8009, 16001, 32003, 64007, 128021, 256019, 512009},
                700001,
                -1,
                false,
            }, {
                "subset_sum",
                {1001, 2003, 4007, 8009, 16001, 32003, 64007, 128021, 256019, 512009},
                {1001, 2003, 4007, 8009, 16001, 32003, 64007, 128021, 256019, 512009},
                700001,
                4,
                true,
            }, {
                "knapsack",
                {12, 23, 31, 47, 52, 68, 71, 89, 94, 103},
                {10, 21, 29, 43, 50, 61, 70, 80, 91, 99},
                250,
                -1,
                false,
            }, {
                "knapsack",
                {12, 23, 31, 47, 52, 68, 71, 89, 94, 103},
                {10, 21, 29, 43, 50, 61, 70, 80, 91, 99},
                250,
                1,
                true,
            }}));

struct DynamicProgrammingPrimalDualRandomTieBreakTestParams
{
    /** Name of the test. */
    std::string name;

    /** Profits of the items. */
    std::vector<Profit> profits;

    /** Weights of the items. */
    std::vector<Weight> weights;

    /** Capacity of the knapsack. */
    Weight capacity;

    /** Seed of the generator. */
    Seed seed;
};

inline std::ostream& operator<<(std::ostream& os, const DynamicProgrammingPrimalDualRandomTieBreakTestParams& test_params)
{
    os << test_params.name << " " << test_params.seed;
    return os;
}

class DynamicProgrammingPrimalDualRandomTieBreakTest: public testing::TestWithParam<DynamicProgrammingPrimalDualRandomTieBreakTestParams> { };

TEST_P(DynamicProgrammingPrimalDualRandomTieBreakTest, DynamicProgrammingPrimalDualRandomTieBreak)
{
    DynamicProgrammingPrimalDualRandomTieBreakTestParams test_params = GetParam();
    InstanceBuilder instance_builder;
    instance_builder.set_capacity(test_params.capacity);
    for (ItemId item_id = 0;
            item_id < (ItemId)test_params.profits.size();
            ++item_id) {
        instance_builder.add_item(
                test_params.profits[item_id],
                test_params.weights[item_id]);
    }
    const Instance instance = instance_builder.build();

    // Reference value, computed with the default seed.
    std::mt19937_64 reference_generator(0);
    DynamicProgrammingPrimalDualParameters reference_parameters;
    reference_parameters.verbosity_level = 0;
    auto reference_output = dynamic_programming_primal_dual(instance, reference_generator, reference_parameters);

    // Whatever the random order of the items with the same efficiency, the
    // algorithm is exact.
    std::mt19937_64 generator(test_params.seed);
    DynamicProgrammingPrimalDualParameters parameters;
    parameters.verbosity_level = 0;
    auto output = dynamic_programming_primal_dual(instance, generator, parameters);

    EXPECT_TRUE(output.solution.feasible());
    EXPECT_EQ(output.solution.profit(), reference_output.value);
    EXPECT_EQ(output.value, reference_output.value);
    EXPECT_EQ(output.bound, reference_output.value);
}

INSTANTIATE_TEST_SUITE_P(
        KnapsackDynamicProgrammingPrimalDualRandomTieBreak,
        DynamicProgrammingPrimalDualRandomTieBreakTest,
        testing::ValuesIn([]()
        {
            // Subset sum instance: all the items have the same efficiency.
            DynamicProgrammingPrimalDualRandomTieBreakTestParams subset_sum{
                "subset_sum",
                {1001, 2003, 4007, 8009, 16001, 32003, 64007, 128021, 256019, 512009},
                {1001, 2003, 4007, 8009, 16001, 32003, 64007, 128021, 256019, 512009},
                700001,
                0};
            // Knapsack instance with groups of items with the same efficiency.
            DynamicProgrammingPrimalDualRandomTieBreakTestParams knapsack{
                "knapsack",
                {20, 30, 40, 50, 33, 66, 99, 132, 17, 34, 51, 68},
                {10, 15, 20, 25, 30, 60, 90, 120, 25, 50, 75, 100},
                250,
                0};
            std::vector<DynamicProgrammingPrimalDualRandomTieBreakTestParams> test_params;
            for (Seed seed = 0; seed < 8; ++seed) {
                subset_sum.seed = seed;
                test_params.push_back(subset_sum);
                knapsack.seed = seed;
                test_params.push_back(knapsack);
            }
            return test_params;
        }()));
