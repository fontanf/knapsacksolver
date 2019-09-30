#include "knapsack/lib/algorithms.hpp"

#include <boost/program_options.hpp>

using namespace knapsack;

int main(int argc, char *argv[])
{
    namespace po = boost::program_options;

    // Parse program options
    std::string algorithm = "bellman_array";
    std::string instancefile = "";
    std::string outputfile = "";
    std::string format = "knapsack_standard";
    std::string certfile = "";
    std::string logfile = "";
    int loglevelmax = 999;
    int seed = 0;
    double time_limit = std::numeric_limits<double>::infinity();

    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "produce help message")
        ("algorithm,a", po::value<std::string>(&algorithm), "set algorithm")
        ("input,i", po::value<std::string>(&instancefile)->required(), "set input file (required)")
        ("format,f", po::value<std::string>(&format), "set input file format (default: knapsack_standard)")
        ("output,o", po::value<std::string>(&outputfile), "set output file")
        ("cert,c", po::value<std::string>(&certfile), "set certificate file")
        ("time-limit,t", po::value<double>(&time_limit), "Time limit in seconds\n  ex: 3600")
        ("seed,s", po::value<int>(&seed), "seed")
        ("verbose,v", "set verbosity")
        ("log,l", po::value<std::string>(&logfile), "set log file")
        ("loglevelmax", po::value<int>(&loglevelmax), "set log max level")
        ("log2stderr", "write log in stderr")
        ;
    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    if (vm.count("help")) {
        std::cout << desc << std::endl;;
        return 1;
    }
    try {
        po::notify(vm);
    } catch (po::required_option e) {
        std::cout << desc << std::endl;;
        return 1;
    }

    std::mt19937_64 gen(seed);
    auto func = get_algorithm(algorithm);
    Instance ins(instancefile, format);

    Info info = Info()
        .set_verbose(vm.count("verbose"))
        .set_timelimit(time_limit)
        .set_certfile(certfile)
        .set_outputfile(outputfile)
        .set_onlywriteattheend(true)
        .set_logfile(logfile)
        .set_log2stderr(vm.count("log2stderr"))
        .set_loglevelmax(loglevelmax)
        ;

    knapsack::Output output = func(ins, gen, info);

    info.write_ini(outputfile);
    output.solution.write_cert(certfile);
    return 0;
}

