#include "knapsacksolver/instance_builder.hpp"

#include "knapsacksolver/upper_bound.hpp"
#include "knapsacksolver/algorithms/upper_bound_dantzig.hpp"
#include "knapsacksolver/algorithms/greedy.hpp"
#include "knapsacksolver/algorithms/dynamic_programming_bellman.hpp"
#include "knapsacksolver/algorithms/dynamic_programming_primal_dual.hpp"

#include <boost/program_options.hpp>

using namespace knapsacksolver;

namespace po = boost::program_options;

void read_args(
        Parameters& parameters,
        const po::variables_map& vm)
{
    parameters.timer.set_sigint_handler();
    parameters.messages_to_stdout = true;
    if (vm.count("time-limit"))
        parameters.timer.set_time_limit(vm["time-limit"].as<double>());
    if (vm.count("verbosity-level"))
        parameters.verbosity_level = vm["verbosity-level"].as<int>();
    if (vm.count("log"))
        parameters.log_path = vm["log"].as<std::string>();
    parameters.log_to_stderr = vm.count("log-to-stderr");
    parameters.json_output = (vm.count("output"));
    bool only_write_at_the_end = vm.count("only-write-at-the-end");
    if (!only_write_at_the_end) {
        std::string certificate_path = vm["certificate"].as<std::string>();
        std::string json_output_path = vm["output"].as<std::string>();
        parameters.new_solution_callback = [
            json_output_path,
            certificate_path](
                    const Output& output)
        {
            output.write_json_output(json_output_path);
            output.solution.write(certificate_path);
        };
    }
}

Output run(
        const Instance& instance,
        const po::variables_map& vm)
{
    std::mt19937_64 generator(vm["seed"].as<Seed>());
    Solution solution(instance, vm["initial-solution"].as<std::string>());

    // Run algorithm.
    std::string algorithm = "dynamic-programming-primal-dual";
    if (vm.count("algorithm"))
        algorithm = vm["algorithm"].as<std::string>();

    if (algorithm == "upper-bound-dantzig") {
        UpperBoundDantzigParameters parameters;
        read_args(parameters, vm);
        return upper_bound_dantzig(instance, parameters);

    } else if (algorithm == "greedy") {
        GreedyParameters parameters;
        read_args(parameters, vm);
        return greedy(instance, parameters);

    } else if (algorithm == "dynamic-programming-bellman-rec") {
        Parameters parameters;
        read_args(parameters, vm);
        return dynamic_programming_bellman_rec(instance, parameters);
    } else if (algorithm == "dynamic-programming-bellman-array") {
        Parameters parameters;
        read_args(parameters, vm);
        return dynamic_programming_bellman_array(instance, parameters);
    } else if (algorithm == "dynamic-programming-bellman-array-parallel") {
        Parameters parameters;
        read_args(parameters, vm);
        return dynamic_programming_bellman_array_parallel(instance, parameters);
    } else if (algorithm == "dynamic-programming-bellman-array-all") {
        Parameters parameters;
        read_args(parameters, vm);
        return dynamic_programming_bellman_array_all(instance, parameters);
    } else if (algorithm == "dynamic-programming-bellman-array-one") {
        Parameters parameters;
        read_args(parameters, vm);
        return dynamic_programming_bellman_array_one(instance, parameters);
    } else if (algorithm == "dynamic-programming-bellman-array-part") {
        DynamicProgrammingBellmanArrayPartParameters parameters;
        read_args(parameters, vm);
        if (vm.count("partial-solution-size"))
            parameters.partial_solution_size = vm["partial-solution-size"].as<int>();
        return dynamic_programming_bellman_array_part(instance, parameters);
    } else if (algorithm == "dynamic-programming-bellman-array-rec") {
        Parameters parameters;
        read_args(parameters, vm);
        return dynamic_programming_bellman_array_rec(instance, parameters);
    } else if (algorithm == "dynamic-programming-bellman-list") {
        DynamicProgrammingBellmanListParameters parameters;
        read_args(parameters, vm);
        if (vm.count("sort"))
            parameters.sort = vm["sort"].as<bool>();
        return dynamic_programming_bellman_list(instance, parameters);

    } else if (algorithm == "dynamic-programming-primal-dual"
            || algorithm == "minknap") {
        DynamicProgrammingPrimalDualParameters parameters;
        read_args(parameters, vm);
        if (vm.count("partial-solution-size"))
            parameters.partial_solution_size = vm["partial-solution-size"].as<int>();
        if (vm.count("pairing"))
            parameters.pairing = vm["pairing"].as<bool>();
        return dynamic_programming_primal_dual(instance, parameters);

    } else {
        throw std::invalid_argument(
                "Unknown algorithm \"" + algorithm + "\".");
    }
}

int main(int argc, char *argv[])
{
    // Parse program options
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "produce help message")
        ("algorithm,a", po::value<std::string>(), "set algorithm")
        ("input,i", po::value<std::string>()->required(), "set input file (required)")
        ("format,f", po::value<std::string>()->default_value(""), "set input file format (default: standard)")
        ("output,o", po::value<std::string>()->default_value(""), "set JSON output file")
        ("initial-solution,", po::value<std::string>()->default_value(""), "")
        ("certificate,c", po::value<std::string>()->default_value(""), "set certificate file")
        ("seed,s", po::value<Seed>()->default_value(0), "set seed")
        ("time-limit,t", po::value<double>(), "set time limit in seconds")
        ("verbosity-level,v", po::value<int>(), "set verbosity level")
        ("only-write-at-the-end,e", "only write output and certificate files at the end")
        ("log,l", po::value<std::string>(), "set log file")
        ("log-to-stderr", "write log to stderr")

        ("sort,", po::value<bool>(), "set sort")
        ("partial-solution-size,", po::value<int>(), "set partial solution size")
        ("pairing,", po::value<bool>(), "set pairing")
        ;
    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    if (vm.count("help")) {
        std::cout << desc << std::endl;;
        return 1;
    }
    try {
        po::notify(vm);
    } catch (const po::required_option& e) {
        std::cout << desc << std::endl;;
        return 1;
    }

    // Build instance.
    InstanceBuilder instance_builder;
    instance_builder.read(
            vm["input"].as<std::string>(),
            vm["format"].as<std::string>());
    const Instance instance = instance_builder.build();

    // Run.
    Output output = run(instance, vm);

    // Write outputs.
    std::string certificate_path = vm["certificate"].as<std::string>();
    std::string json_output_path = vm["output"].as<std::string>();
    output.write_json_output(json_output_path);
    output.solution.write(certificate_path);

    return 0;
}
