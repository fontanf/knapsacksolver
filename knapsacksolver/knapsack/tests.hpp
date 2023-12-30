#pragma once

#include "knapsacksolver/knapsack/instance_builder.hpp"
#include "knapsacksolver/knapsack/solution.hpp"

#include <gtest/gtest.h>

namespace knapsacksolver
{
namespace knapsack
{

struct TestInstancePath
{
    std::string instance_path;
    std::string instance_format;
    std::string certificate_path;
    std::string certificate_format;
};

inline std::vector<TestInstancePath> get_test_instance_paths()
{
    return {
        {"data/knapsack/tests//instance_0_item.txt", "standard",
            "data/knapsack/tests//instance_0_item.txt", "standard"},
        {"data/knapsack/tests/instance_1_item.txt", "standard",
            "data/knapsack/tests/instance_1_item.sol.txt", "standard"},
        {"data/knapsack/tests/instance_1_item_tight_capacity.txt", "standard",
            "data/knapsack/tests/instance_1_item_tight_capacity.sol.txt", "standard"},
        {"data/knapsack/tests/instance_2_items.txt", "standard",
            "data/knapsack/tests/instance_2_items.sol.txt", "standard"},
        {"data/knapsack/tests/instance_2_fitting_items.txt", "standard",
            "data/knapsack/tests/instance_2_fitting_items.sol.txt", "standard"},
        {"data/knapsack/tests/instance_3_items_1.txt", "standard",
            "data/knapsack/tests/instance_3_items_1.sol.txt", "standard"},
        {"data/knapsack/tests/instance_3_items_2.txt", "standard",
            "data/knapsack/tests/instance_3_items_2.sol.txt", "standard"},
        {"data/knapsack/tests/instance_3_items_3.txt", "standard",
            "data/knapsack/tests/instance_3_items_3.sol.txt", "standard"},
        {"data/knapsack/tests/instance_4_items.txt", "standard",
            "data/knapsack/tests/instance_4_items.sol.txt", "standard"},
        {"data/knapsack/tests/instance_5_items.txt", "standard",
            "data/knapsack/tests/instance_5_items.sol.txt", "standard"},
        {"data/knapsack/tests/instance_7_items.txt", "standard",
            "data/knapsack/tests/instance_7_items.sol.txt", "standard"},
        {"data/knapsack/tests/instance_9_items.txt", "standard",
            "data/knapsack/tests/instance_9_items.sol.txt", "standard"},
    };
}

inline std::vector<TestInstancePath> get_pisinger_instance_paths(
        const std::vector<std::pair<std::string, std::string>>& s)
{
    std::vector<TestInstancePath> instance_paths;
    for (auto p: s) {
        for (int i = 1; i <= 100; ++i) {
            instance_paths.push_back({
                    "data/knapsack/" + p.first + "/" + p.second + "/" + p.second + "_" + std::to_string(i) + ".csv", "pisinger",
                    "data/knapsack/" + p.first + "/" + p.second + "/" + p.second + "_" + std::to_string(i) + ".csv", "pisinger"});
        }
    }
    return instance_paths;
}

enum class AlgorithmType { Exact, Heuristic, Bound };

using Algorithm = std::function<const Output(const Instance&)>;

struct TestAlgorithm
{
    Algorithm algorithm;
    bool has_solution;
    bool exact;
};

struct TestParams
{
    TestParams(const Instance& instance):
        instance(instance),
        solution(instance) { }

    TestAlgorithm algorithm;
    const Instance instance;
    Solution solution;
};

std::vector<TestParams> get_test_params(
        const std::vector<TestAlgorithm>& algorithms,
        const std::vector<TestInstancePath>& instance_paths)
{
    std::vector<TestParams> res;
    for (const TestAlgorithm& algorithm: algorithms) {
        for (const TestInstancePath& t: instance_paths) {
            InstanceBuilder instance_builder;
            instance_builder.read(t.instance_path, t.instance_format);
            TestParams test_params(instance_builder.build());
            test_params.algorithm = algorithm;
            test_params.solution = Solution(test_params.instance, t.certificate_path, t.certificate_format);;
            res.push_back(test_params);
        }
    }
    return res;
}

class AlgorithmTest: public testing::TestWithParam<TestParams> { };

}
}
