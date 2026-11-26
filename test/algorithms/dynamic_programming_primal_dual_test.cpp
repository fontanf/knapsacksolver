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
    /** Path of the instance, in the 'standard' format. */
    std::string instance_path;

    /** Path of an optimal solution of the instance. */
    std::string certificate_path;

    /** Maximum number of states. */
    StateId maximum_number_of_states;

    /** True iff the maximum number of states is expected to be reached. */
    bool expected_maximum_number_of_states_reached;
};

void PrintTo(const DynamicProgrammingPrimalDualMaximumNumberOfStatesTestParams& params, std::ostream* os)
{
    *os << "instance_path " << params.instance_path << "\n";
    *os << "certificate_path " << params.certificate_path << "\n";
    *os << "maximum_number_of_states " << params.maximum_number_of_states << "\n";
    *os << "expected_maximum_number_of_states_reached " << params.expected_maximum_number_of_states_reached << "\n";
}

class DynamicProgrammingPrimalDualMaximumNumberOfStatesTest: public testing::TestWithParam<DynamicProgrammingPrimalDualMaximumNumberOfStatesTestParams> { };

TEST_P(DynamicProgrammingPrimalDualMaximumNumberOfStatesTest, DynamicProgrammingPrimalDualMaximumNumberOfStates)
{
    DynamicProgrammingPrimalDualMaximumNumberOfStatesTestParams test_params = GetParam();
    PrintTo(test_params, &std::cout);
    InstanceBuilder instance_builder;
    instance_builder.read(test_params.instance_path, "standard");
    const Instance instance = instance_builder.build();
    const Solution solution(instance, test_params.certificate_path, "standard");

    std::mt19937_64 generator(0);
    DynamicProgrammingPrimalDualParameters parameters;
    parameters.verbosity_level = 0;
    parameters.maximum_number_of_states = test_params.maximum_number_of_states;
    auto output = dynamic_programming_primal_dual(instance, generator, parameters);

    ASSERT_TRUE(solution.feasible());
    EXPECT_EQ(
            output.maximum_number_of_states_reached,
            test_params.expected_maximum_number_of_states_reached);
    // The solution is feasible and the bound is valid.
    EXPECT_TRUE(output.solution.feasible());
    EXPECT_EQ(output.value >= output.solution.profit(), true);
    EXPECT_EQ(output.value <= solution.profit(), true);
    EXPECT_EQ(output.bound >= solution.profit(), true);
    if (!test_params.expected_maximum_number_of_states_reached) {
        EXPECT_EQ(output.solution.profit(), solution.profit());
        EXPECT_EQ(output.bound, solution.profit());
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
                get_path({"data", "tests", "handmade", "subset_sum_10_items.txt"}),
                get_path({"data", "tests", "handmade", "subset_sum_10_items.sol.txt"}),
                -1,
                false,
            }, {
                get_path({"data", "tests", "handmade", "subset_sum_10_items.txt"}),
                get_path({"data", "tests", "handmade", "subset_sum_10_items.sol.txt"}),
                4,
                true,
            }, {
                get_path({"data", "tests", "handmade", "knapsack_10_items.txt"}),
                get_path({"data", "tests", "handmade", "knapsack_10_items.sol.txt"}),
                -1,
                false,
            }, {
                get_path({"data", "tests", "handmade", "knapsack_10_items.txt"}),
                get_path({"data", "tests", "handmade", "knapsack_10_items.sol.txt"}),
                1,
                true,
            }}),
        [](const testing::TestParamInfo<DynamicProgrammingPrimalDualMaximumNumberOfStatesTest::ParamType>& info) {
            return std::to_string(info.index);
        });

struct DynamicProgrammingPrimalDualRandomTieBreakTestParams
{
    /** Path of the instance, in the 'standard' format. */
    std::string instance_path;

    /** Path of an optimal solution of the instance. */
    std::string certificate_path;

    /** Seed of the generator. */
    Seed seed;
};

void PrintTo(const DynamicProgrammingPrimalDualRandomTieBreakTestParams& params, std::ostream* os)
{
    *os << "instance_path " << params.instance_path << "\n";
    *os << "certificate_path " << params.certificate_path << "\n";
    *os << "seed " << params.seed << "\n";
}

class DynamicProgrammingPrimalDualRandomTieBreakTest: public testing::TestWithParam<DynamicProgrammingPrimalDualRandomTieBreakTestParams> { };

TEST_P(DynamicProgrammingPrimalDualRandomTieBreakTest, DynamicProgrammingPrimalDualRandomTieBreak)
{
    DynamicProgrammingPrimalDualRandomTieBreakTestParams test_params = GetParam();
    PrintTo(test_params, &std::cout);
    InstanceBuilder instance_builder;
    instance_builder.read(test_params.instance_path, "standard");
    const Instance instance = instance_builder.build();
    const Solution solution(instance, test_params.certificate_path, "standard");

    // Whatever the random order of the items with the same efficiency, the
    // algorithm is exact.
    std::mt19937_64 generator(test_params.seed);
    DynamicProgrammingPrimalDualParameters parameters;
    parameters.verbosity_level = 0;
    auto output = dynamic_programming_primal_dual(instance, generator, parameters);

    ASSERT_TRUE(solution.feasible());
    EXPECT_TRUE(output.solution.feasible());
    EXPECT_EQ(output.solution.profit(), solution.profit());
    EXPECT_EQ(output.value, solution.profit());
    EXPECT_EQ(output.bound, solution.profit());
}

INSTANTIATE_TEST_SUITE_P(
        KnapsackDynamicProgrammingPrimalDualRandomTieBreak,
        DynamicProgrammingPrimalDualRandomTieBreakTest,
        testing::ValuesIn(std::vector<DynamicProgrammingPrimalDualRandomTieBreakTestParams>{
            {
                // Subset sum instance: all the items have the same efficiency.
                get_path({"data", "tests", "handmade", "subset_sum_10_items.txt"}),
                get_path({"data", "tests", "handmade", "subset_sum_10_items.sol.txt"}),
                0,
            }, {
                get_path({"data", "tests", "handmade", "subset_sum_10_items.txt"}),
                get_path({"data", "tests", "handmade", "subset_sum_10_items.sol.txt"}),
                1,
            }, {
                get_path({"data", "tests", "handmade", "subset_sum_10_items.txt"}),
                get_path({"data", "tests", "handmade", "subset_sum_10_items.sol.txt"}),
                2,
            }, {
                get_path({"data", "tests", "handmade", "subset_sum_10_items.txt"}),
                get_path({"data", "tests", "handmade", "subset_sum_10_items.sol.txt"}),
                3,
            }, {
                get_path({"data", "tests", "handmade", "subset_sum_10_items.txt"}),
                get_path({"data", "tests", "handmade", "subset_sum_10_items.sol.txt"}),
                4,
            }, {
                get_path({"data", "tests", "handmade", "subset_sum_10_items.txt"}),
                get_path({"data", "tests", "handmade", "subset_sum_10_items.sol.txt"}),
                5,
            }, {
                get_path({"data", "tests", "handmade", "subset_sum_10_items.txt"}),
                get_path({"data", "tests", "handmade", "subset_sum_10_items.sol.txt"}),
                6,
            }, {
                get_path({"data", "tests", "handmade", "subset_sum_10_items.txt"}),
                get_path({"data", "tests", "handmade", "subset_sum_10_items.sol.txt"}),
                7,
            }, {
                // Knapsack instance with groups of items with the same efficiency.
                get_path({"data", "tests", "handmade", "knapsack_12_items_same_efficiency.txt"}),
                get_path({"data", "tests", "handmade", "knapsack_12_items_same_efficiency.sol.txt"}),
                0,
            }, {
                get_path({"data", "tests", "handmade", "knapsack_12_items_same_efficiency.txt"}),
                get_path({"data", "tests", "handmade", "knapsack_12_items_same_efficiency.sol.txt"}),
                1,
            }, {
                get_path({"data", "tests", "handmade", "knapsack_12_items_same_efficiency.txt"}),
                get_path({"data", "tests", "handmade", "knapsack_12_items_same_efficiency.sol.txt"}),
                2,
            }, {
                get_path({"data", "tests", "handmade", "knapsack_12_items_same_efficiency.txt"}),
                get_path({"data", "tests", "handmade", "knapsack_12_items_same_efficiency.sol.txt"}),
                3,
            }, {
                get_path({"data", "tests", "handmade", "knapsack_12_items_same_efficiency.txt"}),
                get_path({"data", "tests", "handmade", "knapsack_12_items_same_efficiency.sol.txt"}),
                4,
            }, {
                get_path({"data", "tests", "handmade", "knapsack_12_items_same_efficiency.txt"}),
                get_path({"data", "tests", "handmade", "knapsack_12_items_same_efficiency.sol.txt"}),
                5,
            }, {
                get_path({"data", "tests", "handmade", "knapsack_12_items_same_efficiency.txt"}),
                get_path({"data", "tests", "handmade", "knapsack_12_items_same_efficiency.sol.txt"}),
                6,
            }, {
                get_path({"data", "tests", "handmade", "knapsack_12_items_same_efficiency.txt"}),
                get_path({"data", "tests", "handmade", "knapsack_12_items_same_efficiency.sol.txt"}),
                7,
            }}),
        [](const testing::TestParamInfo<DynamicProgrammingPrimalDualRandomTieBreakTest::ParamType>& info) {
            return std::to_string(info.index);
        });
