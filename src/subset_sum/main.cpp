#include "knapsacksolver/subset_sum/instance_builder.hpp"

#include "knapsacksolver/subset_sum/algorithms/dynamic_programming_bellman.hpp"
#include "knapsacksolver/subset_sum/algorithms/dynamic_programming_balancing.hpp"

#include <boost/program_options.hpp>

using namespace knapsacksolver::subset_sum;

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
    bool only_write_at_the_end = vm.count("only-write-at-the-end");
    if (!only_write_at_the_end) {

        std::string certificate_path;
        if (vm.count("certificate"))
            certificate_path = vm["certificate"].as<std::string>();

        std::string json_output_path;
        if (vm.count("output"))
            json_output_path = vm["output"].as<std::string>();

        parameters.new_solution_callback = [
            json_output_path,
            certificate_path](
                    const Output& output)
        {
            if (!json_output_path.empty())
                output.write_json_output(json_output_path);
            if (!certificate_path.empty())
                output.solution.write(certificate_path);
        };
    }
}

Output run(
        const Instance& instance,
        const po::variables_map& vm)
{
    std::mt19937_64 generator(vm["seed"].as<Seed>());
    Solution solution = (!vm.count("initial-solution"))?
        Solution(instance):
        Solution(instance, vm["initial-solution"].as<std::string>());

    // Run algorithm.
    std::string algorithm = vm["algorithm"].as<std::string>();
    if (algorithm == "dynamic-programming-bellman-array") {
        Parameters parameters;
        read_args(parameters, vm);
        return dynamic_programming_bellman_array(instance, parameters);
    } else if (algorithm == "dynamic-programming-bellman-list") {
        Parameters parameters;
        read_args(parameters, vm);
        return dynamic_programming_bellman_list(instance, parameters);
    } else if (algorithm == "dynamic-programming-bellman-word-ram") {
        Parameters parameters;
        read_args(parameters, vm);
        return dynamic_programming_bellman_word_ram(instance, parameters);
    } else if (algorithm == "dynamic-programming-bellman-word-ram-rec") {
        Parameters parameters;
        read_args(parameters, vm);
        return dynamic_programming_bellman_word_ram_rec(instance, parameters);

    } else if (algorithm == "dynamic-programming-balancing-array") {
        Parameters parameters;
        read_args(parameters, vm);
        return dynamic_programming_balancing_array(instance, parameters);

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
        ("output,o", po::value<std::string>(), "set JSON output file")
        ("certificate,c", po::value<std::string>(), "set certificate file")
        ("seed,s", po::value<Seed>()->default_value(0), "set seed")
        ("time-limit,t", po::value<double>(), "set time limit in seconds")
        ("verbosity-level,v", po::value<int>(), "set verbosity level")
        ("only-write-at-the-end,e", "only write output and certificate files at the end")
        ("log,l", po::value<std::string>(), "set log file")
        ("log-to-stderr", "write log to stderr")
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
        std::cout << desc << std::endl;
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
    if (vm.count("certificate"))
        output.solution.write(vm["certificate"].as<std::string>());
    if (vm.count("output"))
        output.write_json_output(vm["output"].as<std::string>());

    return 0;
}
