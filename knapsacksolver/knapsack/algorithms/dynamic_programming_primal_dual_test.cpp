#include "knapsacksolver/knapsack/tests.hpp"

#include "knapsacksolver/knapsack/algorithms/dynamic_programming_primal_dual.hpp"

#include "knapsacksolver/knapsack/algorithms/dynamic_programming_bellman.hpp"
#include "knapsacksolver/knapsack/generator.hpp"

using namespace knapsacksolver;
using namespace knapsacksolver::knapsack;

INSTANTIATE_TEST_SUITE_P(
        DynamicProgrammingPrimalDual,
        AlgorithmTest,
        testing::ValuesIn(get_test_params(
                {
                    {[](const Instance& instance) { DynamicProgrammingPrimalDualParameters parameters; parameters.partial_solution_size = 1; return dynamic_programming_primal_dual(instance, parameters); }, true, true},
                    {[](const Instance& instance) { DynamicProgrammingPrimalDualParameters parameters; parameters.partial_solution_size = 2; return dynamic_programming_primal_dual(instance, parameters); }, true, true},
                    {[](const Instance& instance) { DynamicProgrammingPrimalDualParameters parameters; parameters.partial_solution_size = 3; return dynamic_programming_primal_dual(instance, parameters); }, true, true},
                    {[](const Instance& instance) { return dynamic_programming_primal_dual(instance); }, true, true},
                },
                get_test_instance_paths())));

INSTANTIATE_TEST_SUITE_P(
        DynamicProgrammingPrimalDualPisinger,
        AlgorithmTest,
        testing::ValuesIn(get_test_params(
                {
                    {[](const Instance& instance)
                    {
                        DynamicProgrammingPrimalDualParameters parameters;
                        parameters.verbosity_level = 0;
                        return dynamic_programming_primal_dual(instance, parameters);
                    },
                    true,
                    true},
                },
                get_pisinger_instance_paths({
                    {"smallcoeff", "knapPI_1_50_1000"},
                    {"smallcoeff", "knapPI_1_50_10000"},
                    {"smallcoeff", "knapPI_1_100_1000"},
                    {"smallcoeff", "knapPI_1_100_10000"},
                    {"smallcoeff", "knapPI_1_200_1000"},
                    {"smallcoeff", "knapPI_1_200_10000"},
                    {"smallcoeff", "knapPI_1_500_1000"},
                    {"smallcoeff", "knapPI_1_500_10000"},
                    {"smallcoeff", "knapPI_1_1000_1000"},
                    {"smallcoeff", "knapPI_1_1000_10000"},
                    {"smallcoeff", "knapPI_1_2000_1000"},
                    {"smallcoeff", "knapPI_1_2000_10000"},
                    {"smallcoeff", "knapPI_1_5000_1000"},
                    {"smallcoeff", "knapPI_1_5000_10000"},
                    {"smallcoeff", "knapPI_1_10000_1000"},
                    {"smallcoeff", "knapPI_1_10000_10000"},
                    {"smallcoeff", "knapPI_2_50_1000"},
                    {"smallcoeff", "knapPI_2_50_10000"},
                    {"smallcoeff", "knapPI_2_100_1000"},
                    {"smallcoeff", "knapPI_2_100_10000"},
                    {"smallcoeff", "knapPI_2_200_1000"},
                    {"smallcoeff", "knapPI_2_200_10000"},
                    {"smallcoeff", "knapPI_2_500_1000"},
                    {"smallcoeff", "knapPI_2_500_10000"},
                    {"smallcoeff", "knapPI_2_1000_1000"},
                    {"smallcoeff", "knapPI_2_1000_10000"},
                    {"smallcoeff", "knapPI_2_2000_1000"},
                    {"smallcoeff", "knapPI_2_2000_10000"},
                    {"smallcoeff", "knapPI_2_5000_1000"},
                    {"smallcoeff", "knapPI_2_5000_10000"},
                    {"smallcoeff", "knapPI_2_10000_1000"},
                    {"smallcoeff", "knapPI_2_10000_10000"},
                    {"smallcoeff", "knapPI_3_50_1000"},
                    {"smallcoeff", "knapPI_3_50_10000"},
                    {"smallcoeff", "knapPI_3_100_1000"},
                    {"smallcoeff", "knapPI_3_100_10000"},
                    {"smallcoeff", "knapPI_4_50_1000"},
                    {"smallcoeff", "knapPI_4_50_10000"},
                    {"smallcoeff", "knapPI_4_100_1000"},
                    {"smallcoeff", "knapPI_4_100_10000"},
                    {"smallcoeff", "knapPI_5_50_1000"},
                    {"smallcoeff", "knapPI_5_50_10000"},
                    {"smallcoeff", "knapPI_5_100_1000"},
                    {"smallcoeff", "knapPI_5_100_10000"},
                    {"smallcoeff", "knapPI_6_50_1000"},
                    {"smallcoeff", "knapPI_6_50_10000"},
                    {"smallcoeff", "knapPI_6_100_1000"},
                    {"smallcoeff", "knapPI_6_100_10000"},
                    {"smallcoeff", "knapPI_9_50_1000"},
                    {"smallcoeff", "knapPI_9_100_1000"},
                    {"largecoeff", "knapPI_1_50_100000"},
                    {"largecoeff", "knapPI_1_50_1000000"},
                    {"largecoeff", "knapPI_1_50_10000000"},
                    {"largecoeff", "knapPI_1_100_100000"},
                    {"largecoeff", "knapPI_1_100_1000000"},
                    {"largecoeff", "knapPI_1_100_10000000"},
                    {"largecoeff", "knapPI_1_200_100000"},
                    {"largecoeff", "knapPI_1_200_1000000"},
                    {"largecoeff", "knapPI_1_200_10000000"},
                    {"largecoeff", "knapPI_1_500_100000"},
                    {"largecoeff", "knapPI_1_500_1000000"},
                    {"largecoeff", "knapPI_1_500_10000000"},
                    {"largecoeff", "knapPI_1_1000_100000"},
                    {"largecoeff", "knapPI_1_1000_1000000"},
                    {"largecoeff", "knapPI_1_1000_10000000"},
                    {"largecoeff", "knapPI_1_2000_100000"},
                    {"largecoeff", "knapPI_1_2000_1000000"},
                    {"largecoeff", "knapPI_1_2000_10000000"},
                    {"largecoeff", "knapPI_1_5000_100000"},
                    {"largecoeff", "knapPI_1_5000_1000000"},
                    {"largecoeff", "knapPI_1_5000_10000000"},
                    {"largecoeff", "knapPI_1_10000_100000"},
                    {"largecoeff", "knapPI_1_10000_1000000"},
                    {"largecoeff", "knapPI_1_10000_10000000"},
                    {"largecoeff", "knapPI_2_50_100000"},
                    {"largecoeff", "knapPI_2_50_1000000"},
                    {"largecoeff", "knapPI_2_50_10000000"},
                    {"largecoeff", "knapPI_2_100_100000"},
                    {"largecoeff", "knapPI_2_100_1000000"},
                    {"largecoeff", "knapPI_2_100_10000000"},
                    {"largecoeff", "knapPI_2_200_100000"},
                    {"largecoeff", "knapPI_2_200_1000000"},
                    {"largecoeff", "knapPI_2_200_10000000"},
                    {"largecoeff", "knapPI_2_500_100000"},
                    {"largecoeff", "knapPI_2_500_1000000"},
                    {"largecoeff", "knapPI_2_500_10000000"},
                    {"largecoeff", "knapPI_2_1000_100000"},
                    {"largecoeff", "knapPI_2_1000_1000000"},
                    {"largecoeff", "knapPI_2_1000_10000000"},
                    {"largecoeff", "knapPI_2_2000_100000"},
                    {"largecoeff", "knapPI_2_2000_1000000"},
                    {"largecoeff", "knapPI_2_2000_10000000"},
                    {"largecoeff", "knapPI_2_5000_100000"},
                    {"largecoeff", "knapPI_2_5000_1000000"},
                    {"largecoeff", "knapPI_2_5000_10000000"},
                    {"largecoeff", "knapPI_2_10000_100000"},
                    {"largecoeff", "knapPI_2_10000_1000000"},
                    {"largecoeff", "knapPI_2_10000_10000000"},
                    {"largecoeff", "knapPI_3_50_100000"},
                    {"largecoeff", "knapPI_3_50_1000000"},
                    {"largecoeff", "knapPI_3_50_10000000"},
                    {"largecoeff", "knapPI_3_100_100000"},
                    {"largecoeff", "knapPI_4_50_100000"},
                    {"largecoeff", "knapPI_4_50_1000000"},
                    {"largecoeff", "knapPI_4_50_10000000"},
                    {"largecoeff", "knapPI_4_100_100000"},
                    {"largecoeff", "knapPI_5_50_100000"},
                    {"largecoeff", "knapPI_5_50_1000000"},
                    {"largecoeff", "knapPI_5_50_10000000"},
                    {"largecoeff", "knapPI_5_100_100000"},
                    {"largecoeff", "knapPI_5_100_1000000"},
                    {"largecoeff", "knapPI_5_100_10000000"},
                    {"largecoeff", "knapPI_6_50_100000"},
                    {"largecoeff", "knapPI_6_100_100000"},
                    {"largecoeff", "knapPI_9_50_100000"},
                    {"largecoeff", "knapPI_9_50_1000000"},
                    {"largecoeff", "knapPI_9_100_100000"},
                    {"largecoeff", "knapPI_9_100_1000000"},
                    {"largecoeff", "knapPI_9_200_100000"},
                    {"largecoeff", "knapPI_9_200_1000000"},
                    {"largecoeff", "knapPI_9_500_100000"},
                    {"largecoeff", "knapPI_9_500_1000000"},
                    {"hardinstances", "knapPI_11_20_1000"},
                    {"hardinstances", "knapPI_11_50_1000"},
                    {"hardinstances", "knapPI_11_100_1000"},
                    {"hardinstances", "knapPI_12_20_1000"},
                    {"hardinstances", "knapPI_12_50_1000"},
                    {"hardinstances", "knapPI_12_100_1000"},
                    {"hardinstances", "knapPI_13_20_1000"},
                    {"hardinstances", "knapPI_13_50_1000"},
                    {"hardinstances", "knapPI_13_100_1000"},
                    {"hardinstances", "knapPI_14_20_1000"},
                    {"hardinstances", "knapPI_14_50_1000"},
                    {"hardinstances", "knapPI_14_100_1000"},
                    {"hardinstances", "knapPI_15_20_1000"},
                    {"hardinstances", "knapPI_15_50_1000"},
                    {"hardinstances", "knapPI_15_100_1000"},
                    {"hardinstances", "knapPI_16_20_1000"},
                    {"hardinstances", "knapPI_16_50_1000"},
                    {"hardinstances", "knapPI_16_100_1000"},
                }))));

// The pairing strategy makes the algorithm able to solve all the random subset
// sum instances.
INSTANTIATE_TEST_SUITE_P(
        DynamicProgrammingPrimalDualPairingPisinger,
        AlgorithmTest,
        testing::ValuesIn(get_test_params(
                {
                    {[](const Instance& instance)
                    {
                        DynamicProgrammingPrimalDualParameters parameters;
                        parameters.pairing = true;
                        parameters.verbosity_level = 0;
                        return dynamic_programming_primal_dual(instance, parameters); },
                    true,
                    true},
                },
                get_pisinger_instance_paths({
                    {"smallcoeff", "knapPI_1_50_1000"},
                    {"smallcoeff", "knapPI_1_50_10000"},
                    {"smallcoeff", "knapPI_1_100_1000"},
                    {"smallcoeff", "knapPI_1_100_10000"},
                    {"smallcoeff", "knapPI_1_200_1000"},
                    {"smallcoeff", "knapPI_1_200_10000"},
                    {"smallcoeff", "knapPI_1_500_1000"},
                    {"smallcoeff", "knapPI_1_500_10000"},
                    {"smallcoeff", "knapPI_1_1000_1000"},
                    {"smallcoeff", "knapPI_1_1000_10000"},
                    {"smallcoeff", "knapPI_1_2000_1000"},
                    {"smallcoeff", "knapPI_1_2000_10000"},
                    {"smallcoeff", "knapPI_1_5000_1000"},
                    {"smallcoeff", "knapPI_1_5000_10000"},
                    {"smallcoeff", "knapPI_1_10000_1000"},
                    {"smallcoeff", "knapPI_1_10000_10000"},
                    {"smallcoeff", "knapPI_2_50_1000"},
                    {"smallcoeff", "knapPI_2_50_10000"},
                    {"smallcoeff", "knapPI_2_100_1000"},
                    {"smallcoeff", "knapPI_2_100_10000"},
                    {"smallcoeff", "knapPI_2_200_1000"},
                    {"smallcoeff", "knapPI_2_200_10000"},
                    {"smallcoeff", "knapPI_2_500_1000"},
                    {"smallcoeff", "knapPI_2_500_10000"},
                    {"smallcoeff", "knapPI_2_1000_1000"},
                    {"smallcoeff", "knapPI_2_1000_10000"},
                    {"smallcoeff", "knapPI_2_2000_1000"},
                    {"smallcoeff", "knapPI_2_2000_10000"},
                    {"smallcoeff", "knapPI_2_5000_1000"},
                    {"smallcoeff", "knapPI_2_5000_10000"},
                    {"smallcoeff", "knapPI_2_10000_1000"},
                    {"smallcoeff", "knapPI_2_10000_10000"},
                    {"smallcoeff", "knapPI_3_50_1000"},
                    {"smallcoeff", "knapPI_3_50_10000"},
                    {"smallcoeff", "knapPI_3_100_1000"},
                    {"smallcoeff", "knapPI_3_100_10000"},
                    {"smallcoeff", "knapPI_4_50_1000"},
                    {"smallcoeff", "knapPI_4_50_10000"},
                    {"smallcoeff", "knapPI_4_100_1000"},
                    {"smallcoeff", "knapPI_4_100_10000"},
                    {"smallcoeff", "knapPI_5_50_1000"},
                    {"smallcoeff", "knapPI_5_50_10000"},
                    {"smallcoeff", "knapPI_5_100_1000"},
                    {"smallcoeff", "knapPI_5_100_10000"},
                    {"smallcoeff", "knapPI_6_50_1000"},
                    {"smallcoeff", "knapPI_6_50_10000"},
                    {"smallcoeff", "knapPI_6_100_1000"},
                    {"smallcoeff", "knapPI_6_100_10000"},
                    {"smallcoeff", "knapPI_6_200_1000"},
                    {"smallcoeff", "knapPI_6_200_10000"},
                    {"smallcoeff", "knapPI_6_500_1000"},
                    {"smallcoeff", "knapPI_6_500_10000"},
                    {"smallcoeff", "knapPI_6_1000_1000"},
                    {"smallcoeff", "knapPI_6_1000_10000"},
                    {"smallcoeff", "knapPI_6_2000_1000"},
                    {"smallcoeff", "knapPI_6_2000_10000"},
                    {"smallcoeff", "knapPI_6_5000_1000"},
                    {"smallcoeff", "knapPI_6_5000_10000"},
                    {"smallcoeff", "knapPI_6_10000_1000"},
                    {"smallcoeff", "knapPI_6_10000_10000"},
                    {"smallcoeff", "knapPI_9_50_1000"},
                    {"smallcoeff", "knapPI_9_100_1000"},
                    {"largecoeff", "knapPI_1_50_100000"},
                    {"largecoeff", "knapPI_1_50_1000000"},
                    {"largecoeff", "knapPI_1_50_10000000"},
                    {"largecoeff", "knapPI_1_100_100000"},
                    {"largecoeff", "knapPI_1_100_1000000"},
                    {"largecoeff", "knapPI_1_100_10000000"},
                    {"largecoeff", "knapPI_1_200_100000"},
                    {"largecoeff", "knapPI_1_200_1000000"},
                    {"largecoeff", "knapPI_1_200_10000000"},
                    {"largecoeff", "knapPI_1_500_100000"},
                    {"largecoeff", "knapPI_1_500_1000000"},
                    {"largecoeff", "knapPI_1_500_10000000"},
                    {"largecoeff", "knapPI_1_1000_100000"},
                    {"largecoeff", "knapPI_1_1000_1000000"},
                    {"largecoeff", "knapPI_1_1000_10000000"},
                    {"largecoeff", "knapPI_1_2000_100000"},
                    {"largecoeff", "knapPI_1_2000_1000000"},
                    {"largecoeff", "knapPI_1_2000_10000000"},
                    {"largecoeff", "knapPI_1_5000_100000"},
                    {"largecoeff", "knapPI_1_5000_1000000"},
                    {"largecoeff", "knapPI_1_5000_10000000"},
                    {"largecoeff", "knapPI_1_10000_100000"},
                    {"largecoeff", "knapPI_1_10000_1000000"},
                    {"largecoeff", "knapPI_1_10000_10000000"},
                    {"largecoeff", "knapPI_2_50_100000"},
                    {"largecoeff", "knapPI_2_50_1000000"},
                    {"largecoeff", "knapPI_2_50_10000000"},
                    {"largecoeff", "knapPI_2_100_100000"},
                    {"largecoeff", "knapPI_2_100_1000000"},
                    {"largecoeff", "knapPI_2_100_10000000"},
                    {"largecoeff", "knapPI_2_200_100000"},
                    {"largecoeff", "knapPI_2_200_1000000"},
                    {"largecoeff", "knapPI_2_200_10000000"},
                    {"largecoeff", "knapPI_2_500_100000"},
                    {"largecoeff", "knapPI_2_500_1000000"},
                    {"largecoeff", "knapPI_2_500_10000000"},
                    {"largecoeff", "knapPI_2_1000_100000"},
                    {"largecoeff", "knapPI_2_1000_1000000"},
                    {"largecoeff", "knapPI_2_1000_10000000"},
                    {"largecoeff", "knapPI_2_2000_100000"},
                    {"largecoeff", "knapPI_2_2000_1000000"},
                    {"largecoeff", "knapPI_2_2000_10000000"},
                    {"largecoeff", "knapPI_2_5000_100000"},
                    {"largecoeff", "knapPI_2_5000_1000000"},
                    {"largecoeff", "knapPI_2_5000_10000000"},
                    {"largecoeff", "knapPI_2_10000_100000"},
                    {"largecoeff", "knapPI_2_10000_1000000"},
                    {"largecoeff", "knapPI_2_10000_10000000"},
                    {"largecoeff", "knapPI_3_50_100000"},
                    {"largecoeff", "knapPI_3_50_1000000"},
                    {"largecoeff", "knapPI_3_50_10000000"},
                    {"largecoeff", "knapPI_3_100_100000"},
                    {"largecoeff", "knapPI_4_50_100000"},
                    {"largecoeff", "knapPI_4_50_1000000"},
                    {"largecoeff", "knapPI_4_50_10000000"},
                    {"largecoeff", "knapPI_4_100_100000"},
                    {"largecoeff", "knapPI_5_50_100000"},
                    {"largecoeff", "knapPI_5_50_1000000"},
                    {"largecoeff", "knapPI_5_50_10000000"},
                    {"largecoeff", "knapPI_5_100_100000"},
                    {"largecoeff", "knapPI_5_100_1000000"},
                    {"largecoeff", "knapPI_5_100_10000000"},
                    {"largecoeff", "knapPI_6_50_100000"},
                    {"largecoeff", "knapPI_6_50_1000000"},
                    {"largecoeff", "knapPI_6_50_10000000"},
                    {"largecoeff", "knapPI_6_100_100000"},
                    {"largecoeff", "knapPI_6_100_1000000"},
                    {"largecoeff", "knapPI_6_100_10000000"},
                    {"largecoeff", "knapPI_6_200_100000"},
                    {"largecoeff", "knapPI_6_200_1000000"},
                    {"largecoeff", "knapPI_6_200_10000000"},
                    {"largecoeff", "knapPI_6_500_100000"},
                    {"largecoeff", "knapPI_6_500_1000000"},
                    {"largecoeff", "knapPI_6_500_10000000"},
                    {"largecoeff", "knapPI_6_1000_100000"},
                    {"largecoeff", "knapPI_6_1000_1000000"},
                    {"largecoeff", "knapPI_6_1000_10000000"},
                    {"largecoeff", "knapPI_6_2000_100000"},
                    {"largecoeff", "knapPI_6_2000_1000000"},
                    {"largecoeff", "knapPI_6_2000_10000000"},
                    {"largecoeff", "knapPI_6_5000_100000"},
                    {"largecoeff", "knapPI_6_5000_1000000"},
                    {"largecoeff", "knapPI_6_5000_10000000"},
                    {"largecoeff", "knapPI_6_10000_100000"},
                    {"largecoeff", "knapPI_6_10000_1000000"},
                    {"largecoeff", "knapPI_6_10000_10000000"},
                    {"largecoeff", "knapPI_9_50_100000"},
                    {"largecoeff", "knapPI_9_50_1000000"},
                    {"largecoeff", "knapPI_9_100_100000"},
                    {"largecoeff", "knapPI_9_100_1000000"},
                    {"largecoeff", "knapPI_9_200_100000"},
                    {"largecoeff", "knapPI_9_200_1000000"},
                    {"largecoeff", "knapPI_9_500_100000"},
                    {"largecoeff", "knapPI_9_500_1000000"},
                    {"hardinstances", "knapPI_11_20_1000"},
                    {"hardinstances", "knapPI_11_50_1000"},
                    {"hardinstances", "knapPI_11_100_1000"},
                    {"hardinstances", "knapPI_12_20_1000"},
                    {"hardinstances", "knapPI_12_50_1000"},
                    {"hardinstances", "knapPI_12_100_1000"},
                    {"hardinstances", "knapPI_13_20_1000"},
                    {"hardinstances", "knapPI_13_50_1000"},
                    {"hardinstances", "knapPI_13_100_1000"},
                    {"hardinstances", "knapPI_14_20_1000"},
                    {"hardinstances", "knapPI_14_50_1000"},
                    {"hardinstances", "knapPI_14_100_1000"},
                    {"hardinstances", "knapPI_15_20_1000"},
                    {"hardinstances", "knapPI_15_50_1000"},
                    {"hardinstances", "knapPI_15_100_1000"},
                    {"hardinstances", "knapPI_16_20_1000"},
                    {"hardinstances", "knapPI_16_50_1000"},
                    {"hardinstances", "knapPI_16_100_1000"},
                }))));
