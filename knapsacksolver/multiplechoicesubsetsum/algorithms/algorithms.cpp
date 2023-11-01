#include "knapsacksolver/multiplechoicesubsetsum/algorithms/algorithms.hpp"

#include <boost/program_options.hpp>

using namespace knapsacksolver::multiplechoicesubsetsum;
namespace po = boost::program_options;

Output knapsacksolver::multiplechoicesubsetsum::run(
        std::string algorithm,
        Instance& instance,
        std::mt19937_64&,
        optimizationtools::Info info)
{
    std::vector<std::string> algorithm_args = po::split_unix(algorithm);
    std::vector<char*> algorithm_argv;
    for(Counter i = 0; i < (Counter)algorithm_args.size(); ++i)
        algorithm_argv.push_back(const_cast<char*>(algorithm_args[i].c_str()));

    if (algorithm.empty() || algorithm_args[0].empty()) {
        throw std::invalid_argument("Missing algorithm.");

    } else if (algorithm_args[0] == "dynamic-programming-bellman-array") {
        return dynamic_programming_bellman_array(instance, info);
    } else if (algorithm_args[0] == "dynamic-programming-bellman-word-ram") {
        return dynamic_programming_bellman_word_ram(instance, info);
    } else if (algorithm_args[0] == "dynamic-programming-bellman-word-ram-rec") {
        return dynamic_programming_bellman_word_ram_rec(instance, info);

    } else {
        throw std::invalid_argument(
                "Unknown algorithm \"" + algorithm_args[0] + "\".");
    }
}

