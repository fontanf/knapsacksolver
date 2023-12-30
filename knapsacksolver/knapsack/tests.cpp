#include "knapsacksolver/knapsack/tests.hpp"

using namespace knapsacksolver::knapsack;

TEST_P(AlgorithmTest, AlgorithmOutput)
{
    TestParams test_params = GetParam();

    // Run algorithm.
    auto output = test_params.algorithm.algorithm(test_params.instance);

    // Check output.
    EXPECT_LE(output.bound, test_params.solution.profit());
    if (test_params.algorithm.has_solution) {
        EXPECT_EQ(output.value, output.solution.profit());
    }
    if (test_params.algorithm.exact) {
        EXPECT_EQ(output.value, test_params.solution.profit());
        EXPECT_EQ(output.bound, test_params.solution.profit());
    }
}
