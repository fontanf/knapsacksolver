#include "knapsack/opt_minknap/minknap.hpp"

using namespace knapsack;

int main(int argc, char *argv[])
{
    namespace po = boost::program_options;

    // Parse program options
    std::string output_file = "";
    std::string cert_file = "";
    std::string retrieve = "part";
    ItemPos k = 64;
    MinknapParams p;

    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "produce help message")
        ("input-data,i", po::value<std::string>()->required(), "set input data (required)")
        ("output-file,o", po::value<std::string>(&output_file), "set output file")
        ("cert-file,c", po::value<std::string>(&cert_file)->implicit_value("//"), "set certificate output file")
        ("retrieve,r", po::value<std::string>(&retrieve), "set algorithm")
        ("greedynlogn,g", po::value<StateIdx>(&p.lb_greedynlogn), "")
        ("pairing,p", po::value<StateIdx>(&p.lb_pairing), "")
        ("surrogate,s", po::value<StateIdx>(&p.ub_surrogate), "")
        ("solve-sur,k", po::value<StateIdx>(&p.solve_sur), "")
        ("part-size,x", po::value<ItemPos>(&k), "")
        ("verbose,v",  "enable verbosity")
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
    if (cert_file == "//")
        cert_file = vm["input-data"].as<std::string>() + ".sol";

    Instance instance(vm["input-data"].as<std::string>());
    Solution sopt(instance);

    Info info;
    if (vm.count("verbose"))
        info.set_verbose();

    if (retrieve == "none") {
        opt_minknap_list(instance, info, p);
    } else if (retrieve == "part") {
        sopt = sopt_minknap_list_part(instance, info, p, k);
    } else {
        assert(false);
        return 1;
    }

    info.write_ini(output_file); // Write output file
    sopt.write_cert(cert_file); // Write certificate file
    return 0;
}
