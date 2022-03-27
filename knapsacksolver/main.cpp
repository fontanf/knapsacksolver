#include "knapsacksolver/algorithms/algorithms.hpp"

#include <boost/program_options.hpp>

using namespace knapsacksolver;

int main(int argc, char *argv[])
{
    namespace po = boost::program_options;

    // Parse program options

    std::string algorithm = "bellman_array";
    std::string instance_path = "";
    std::string output_path = "";
    std::string certificate_path = "";
    std::string log_path = "";
    std::string format = "standard";
    int loglevelmax = 999;
    int seed = 0;
    double time_limit = std::numeric_limits<double>::infinity();

    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "produce help message")
        ("algorithm,a", po::value<std::string>(&algorithm), "set algorithm")
        ("input,i", po::value<std::string>(&instance_path)->required(), "set input path (required)")
        ("format,f", po::value<std::string>(&format), "set input file format (default: standard)")
        ("output,o", po::value<std::string>(&output_path), "set JSON output path")
        ("certificate,c", po::value<std::string>(&certificate_path), "set certificate path")
        ("time-limit,t", po::value<double>(&time_limit), "set time limit (in s)")
        ("seed,s", po::value<int>(&seed), "set seed")
        ("verbose,v", "enable verbosity")
        ("log,l", po::value<std::string>(&log_path), "set log path")
        ("loglevelmax", po::value<int>(&loglevelmax), "set log max level")
        ("log2stderr", "write log to stderr")
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

    // Run algorithm

    std::mt19937_64 gen(seed);
    Instance instance(instance_path, format);

    Info info = Info()
        .set_verbose(vm.count("verbose"))
        .set_time_limit(time_limit)
        .set_certificate_path(certificate_path)
        .set_json_output_path(output_path)
        .set_only_write_at_the_end(true)
        .set_log_path(log_path)
        .set_log2stderr(vm.count("log2stderr"))
        .set_maximum_log_level(loglevelmax)
        ;

    auto output = run(algorithm, instance, gen, info);

    if (instance.optimal_solution() != NULL) {
        if (output.solution.feasible() && output.solution.profit() > instance.optimum()) {
            std::cerr << "\033[31m" << "ERROR, computed solution strictly better than provided optimum." << "\033[0m" << std::endl;
            return 1;
        }
        if (output.upper_bound != -1 && output.upper_bound < instance.optimum()) {
            std::cerr << "\033[31m" << "ERROR, computed upper bound strictly lower than provided optimum." << "\033[0m" << std::endl;
            return 1;
        }
    }

    return 0;
}

