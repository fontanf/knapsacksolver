#include "knapsacksolver/knapsack/tests.hpp"

#include "knapsacksolver/knapsack/algorithms/dynamic_programming_bellman.hpp"

#include <gtest/gtest.h>

using namespace knapsacksolver;
using namespace knapsacksolver::knapsack;

INSTANTIATE_TEST_SUITE_P(
        DynamicProgrammingBellman,
        AlgorithmTest,
        testing::ValuesIn(get_test_params(
                {
                    {[](const Instance& instance) { return dynamic_programming_bellman_rec(instance); }, false, true},
                    {[](const Instance& instance) { return dynamic_programming_bellman_array(instance); }, false, true},
                    {[](const Instance& instance) { return dynamic_programming_bellman_array_parallel(instance); }, false, true},
                    {[](const Instance& instance) { return dynamic_programming_bellman_array_all(instance); }, true, true},
                    {[](const Instance& instance) { return dynamic_programming_bellman_array_one(instance); }, true, true},
                    {[](const Instance& instance) { DynamicProgrammingBellmanArrayPartParameters parameters; parameters.partial_solution_size = 1; return dynamic_programming_bellman_array_part(instance, parameters); }, true, true},
                    {[](const Instance& instance) { DynamicProgrammingBellmanArrayPartParameters parameters; parameters.partial_solution_size = 2; return dynamic_programming_bellman_array_part(instance, parameters); }, true, true},
                    {[](const Instance& instance) { DynamicProgrammingBellmanArrayPartParameters parameters; parameters.partial_solution_size = 3; return dynamic_programming_bellman_array_part(instance, parameters); }, true, true},
                    {[](const Instance& instance) { return dynamic_programming_bellman_array_part(instance); }, true, true},
                    {[](const Instance& instance) { return dynamic_programming_bellman_array_rec(instance); }, true, true},
                    {[](const Instance& instance) { return dynamic_programming_bellman_list(instance); }, false, true},
                    {[](const Instance& instance) { DynamicProgrammingBellmanListParameters parameters; parameters.sort = true; return dynamic_programming_bellman_list(instance, parameters); }, false, true},
                },
                get_test_instance_paths())));
