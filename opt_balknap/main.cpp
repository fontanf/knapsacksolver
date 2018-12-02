#include "knapsack/opt_balknap/balknap.hpp"

using namespace knapsack;

int main(int argc, char *argv[])
{
    namespace po = boost::program_options;

    // Parse program options
    std::string output_file = "";
    std::string cert_file = "";
    std::string debug_file = "";
    std::string memory = "array";
    std::string retrieve = "all";
    ItemPos k = 64;
    BalknapParams p;

    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "produce help message")
        ("input-data,i", po::value<std::string>()->required(), "set input data (required)")
        ("output-file,o", po::value<std::string>(&output_file), "set output file")
        ("cert-file,c", po::value<std::string>(&cert_file), "set certificate output file")
        ("upper-bound,u", po::value<char>(&p.ub_type), "set upper bound")
        ("greedynlogn,g", po::value<StateIdx>(&p.cpt_greedynlogn), "")
        ("pairing,p", po::value<StateIdx>(&p.cpt_pairing), "")
        ("surrogate,s", po::value<StateIdx>(&p.cpt_surrogate), "")
        ("solve-sur,k", po::value<StateIdx>(&p.cpt_solve_sur), "")
        ("part-size,x", po::value<ItemPos>(&k), "")
        ("verbose,v",  "enable verbosity")
        ("debug,d", "enable live debugging")
        ("debug-file", po::value<std::string>(&debug_file), "set debug file")
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

    Instance instance(vm["input-data"].as<std::string>());

    Info info;
    info.set_verbose(vm.count("verbose"));
    info.set_debug(debug_file != "");
    info.set_debuglive(vm.count("debug"));

    Solution sopt = sopt_balknap(instance, info, p, k);

    info.write_ini(output_file); // Write output file
    sopt.write_cert(cert_file); // Write certificate file
    info.write_dbg(debug_file); // Write debug file
    return 0;
}
