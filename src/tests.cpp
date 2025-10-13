#include "knapsacksolver/tests.hpp"

#include "knapsacksolver/instance_builder.hpp"

#include <boost/filesystem.hpp>

using namespace knapsacksolver;

namespace fs = boost::filesystem;

std::string knapsacksolver::get_path(
        const std::vector<std::string>& path)
{
    if (path.empty())
        return "";
    fs::path p(path[0]);
    for (size_t i = 1; i < path.size(); ++i)
        p /= path[i];
    return p.string();
}

std::vector<TestInstancePath> knapsacksolver::get_test_instance_paths()
{
    return {
        {
            get_path({"tests", "instance_0_item.txt"}), "standard",
            get_path({"tests", "instance_0_item.txt"}), "standard"
        }, {
            get_path({"tests", "instance_1_item.txt"}), "standard",
            get_path({"tests", "instance_1_item.sol.txt"}), "standard"
        }, {
            get_path({"tests", "instance_1_item_tight_capacity.txt"}), "standard",
            get_path({"tests", "instance_1_item_tight_capacity.sol.txt"}), "standard"
        }, {
            get_path({"tests", "instance_2_items.txt"}), "standard",
            get_path({"tests", "instance_2_items.sol.txt"}), "standard"
        }, {
            get_path({"tests", "instance_2_fitting_items.txt"}), "standard",
            get_path({"tests", "instance_2_fitting_items.sol.txt"}), "standard"
        }, {
            get_path({"tests", "instance_3_items_1.txt"}), "standard",
            get_path({"tests", "instance_3_items_1.sol.txt"}), "standard"
        }, {
            get_path({"tests", "instance_3_items_2.txt"}), "standard",
            get_path({"tests", "instance_3_items_2.sol.txt"}), "standard"
        }, {
            get_path({"tests", "instance_3_items_3.txt"}), "standard",
            get_path({"tests", "instance_3_items_3.sol.txt"}), "standard"},
        {
            get_path({"tests", "instance_4_items.txt"}), "standard",
            get_path({"tests", "instance_4_items.sol.txt"}), "standard"
        }, {
            get_path({"tests", "instance_5_items.txt"}), "standard", 
            get_path({"tests", "instance_5_items.sol.txt"}), "standard"
        }, {
            get_path({"tests", "instance_7_items.txt"}), "standard",
            get_path({"tests", "instance_7_items.sol.txt"}), "standard"
        }, {
            get_path({"tests", "instance_9_items.txt"}), "standard",
            get_path({"tests", "instance_9_items.sol.txt"}), "standard"
        }, {
            get_path({"tests", "instance_debug.txt"}), "standard",
            get_path({"tests", "instance_debug.sol.txt"}), "standard"
        },
    };
}

std::vector<TestInstancePath> knapsacksolver::get_pisinger_instance_paths(
        const std::string& s1,
        const std::string& s2)
{
    std::vector<TestInstancePath> instance_paths;
    for (int i = 1; i <= 100; ++i) {
        instance_paths.push_back({
                get_path({s1, s2, s2 + "_" + std::to_string(i) + ".csv"}), "pisinger",
                get_path({s1, s2, s2 + "_" + std::to_string(i) + ".csv"}), "pisinger"});
    }
    return instance_paths;
}

std::vector<TestParams> knapsacksolver::get_test_params(
        const std::vector<Algorithm>& algorithms,
        const std::vector<std::vector<TestInstancePath>>& instance_paths)
{
    std::vector<TestParams> res;
    for (const Algorithm& algorithm: algorithms) {
        for (const auto& v: instance_paths) {
            for (const TestInstancePath& files: v) {
                TestParams test_params;
                test_params.algorithm = algorithm;
                test_params.files = files;
                res.push_back(test_params);
            }
        }
    }
    return res;
}

const Instance knapsacksolver::get_instance(
            const TestInstancePath& files)
{
    InstanceBuilder instance_builder;
    std::string instance_path = get_path({
            std::getenv("KNAPSACK_DATA"),
            files.instance_path});
    std::cout << "Instance path:  " << instance_path << std::endl;
    instance_builder.read(
            instance_path,
            files.instance_format);
    return instance_builder.build();
}

const Solution knapsacksolver::get_solution(
        const Instance& instance,
        const TestInstancePath& files)
{
    std::string certificate_path = get_path({
            std::getenv("KNAPSACK_DATA"),
            files.certificate_path});
    return Solution(
            instance,
            certificate_path,
            files.certificate_format);
}
