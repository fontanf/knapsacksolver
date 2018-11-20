#include "knapsack/opt_dpprofits/dpprofits_array.hpp"
#include "knapsack/opt_dpprofits/dpprofits_list.hpp"

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
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "produce help message")
        ("input-data,i", po::value<std::string>()->required(), "set input data (required)")
        ("output-file,o", po::value<std::string>(&output_file), "set output file")
        ("cert-file,c", po::value<std::string>(&cert_file), "set certificate output file")
        ("memory,m", po::value<std::string>(&memory), "set algorithm")
        ("retrieve,r", po::value<std::string>(&retrieve), "set algorithm")
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
    Solution sopt(instance);

    Info info;
    info.set_verbose(vm.count("verbose"));
    info.set_debug(debug_file != "");
    info.set_debuglive(vm.count("debug"));

    // DPProfits
    if (memory == "array") {
        if (retrieve == "none") {
            opt_dpprofits_array(instance, info);
        } else if (retrieve == "all") {
            sopt = sopt_dpprofits_array_all(instance, info);
        } else if (retrieve == "one") {
            sopt = sopt_dpprofits_array_one(instance, info);
        } else if (retrieve == "part") {
            sopt = sopt_dpprofits_array_part(instance, 64, info);
        } else if (retrieve == "rec") {
            sopt = sopt_dpprofits_array_rec(instance, info);
        } else {
            assert(false);
            return 1;
        }
    } else if (memory == "list") {
        if (retrieve == "none") {
            opt_dpprofits_list(instance, info);
        } else if (retrieve == "full") {
            sopt = sopt_dpprofits_list_all(instance, info);
        } else if (retrieve == "one") {
            sopt = sopt_dpprofits_list_one(instance, info);
        } else if (retrieve == "part") {
            sopt = sopt_dpprofits_list_part(instance, info);
        } else if (retrieve == "rec") {
            sopt = sopt_dpprofits_list_rec(instance, info);
        } else {
            assert(false);
            return 1;
        }
    } else {
        assert(false);
        return 1;
    }

    info.write_ini(output_file); // Write output file
    sopt.write_cert(cert_file); // Write certificate file
    info.write_dbg(debug_file); // Write debug file
    return 0;
}
