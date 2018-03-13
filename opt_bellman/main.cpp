#include "bellman_array.hpp"
#include "bellman_list.hpp"

int main(int argc, char *argv[])
{
    namespace po = boost::program_options;

    std::string output_file = "";
    std::string cert_file = "";
    std::string memory = "array";
    std::string retrieve = "all";

    // Parse program options
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "produce help message")
        ("input-data,i", po::value<std::string>()->required(), "set input data (required)")
        ("output-file,o", po::value<std::string>(&output_file), "set output file")
        ("cert-file,c", po::value<std::string>(&cert_file), "set certificate output file")
        ("memory,m", po::value<std::string>(&memory), "set algorithm")
        ("retrieve,r", po::value<std::string>(&retrieve), "set algorithm")
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

    Instance instance(vm["input-data"].as<std::string>());
    Solution sopt(instance);
    Profit opt = -1;
    Info info;
    info.verbose(vm.count("verbose"));

    if (memory == "array") {
        if (retrieve == "none") {
            opt = opt_bellman_array(instance, &info);
        } else if (retrieve == "all") {
            sopt = sopt_bellman_array_all(instance, &info);
        } else if (retrieve == "one") {
            sopt = sopt_bellman_array_one(instance, &info);
        } else if (retrieve == "part") {
            sopt = sopt_bellman_array_part(instance, 64, &info);
        } else if (retrieve == "rec") {
            sopt = sopt_bellman_array_rec(instance, &info);
        } else {
            assert(false);
            return 1;
        }
    } else if (memory == "list") {
        if (retrieve == "none") {
            opt = opt_bellman_list(instance, &info);
        } else if (retrieve == "full") {
            sopt = sopt_bellman_list_all(instance, &info);
        } else if (retrieve == "one") {
            sopt = sopt_bellman_list_one(instance, &info);
        } else if (retrieve == "part") {
            sopt = sopt_bellman_list_part(instance, 64, &info);
        } else if (retrieve == "rec") {
            sopt = sopt_bellman_list_rec(instance, &info);
        } else {
            assert(false);
            return 1;
        }
    } else {
        assert(false);
        return 1;
    }

    double t = info.elapsed_time();
    opt = std::max(opt, sopt.profit());
    info.pt.put("Solution.OPT", opt);
    info.pt.put("Solution.Time", t);
    if (Info::verbose(&info)) {
        std::cout << "---" << std::endl;
        std::cout << instance.print_opt(sopt.profit()) << std::endl;
        std::cout << "TIME " << t << std::endl;
    }

    info.write_ini(output_file); // Write output file
    sopt.write_cert(cert_file); // Write certificate file
    return 0;
}
