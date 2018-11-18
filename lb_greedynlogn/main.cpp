#include "knapsack/lb_greedynlogn/greedynlogn.hpp"

using namespace knapsack;

int main(int argc, char *argv[])
{
    namespace po = boost::program_options;

    // Parse program options
    std::string input_data  = "";
    std::string output_file = "";
    std::string cert_file   = "";
    std::string algorithm   = "for";
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "produce help message")
        ("input-data,i",  po::value<std::string>(&input_data)->required(), "set input data (required)")
        ("output-file,o", po::value<std::string>(&output_file),            "set output file")
        ("cert-file,c",   po::value<std::string>(&cert_file),              "set certificate output file")
        ("algorithm,a",   po::value<std::string>(&algorithm),              "set algorithm")
        ("verbose,v",                                                      "enable verbosity")
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

    Instance instance(input_data);
    Solution sol_best(instance);
    Info info(vm.count("verbose"));

    instance.sort_partially();

    if (algorithm == "for") {
        sol_best = sol_forwardgreedynlogn(instance, info);
    } else if (algorithm == "back") {
        sol_best = sol_backwardgreedynlogn(instance, info);
    } else if (algorithm == "best") {
        sol_best = sol_bestgreedynlogn(instance, info);
    }

    info.write_ini(output_file); // Write output file
    sol_best.write_cert(cert_file); // Write certificate file
    return 0;
}
