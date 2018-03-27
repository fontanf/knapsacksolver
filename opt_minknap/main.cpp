#include "minknap.hpp"

int main(int argc, char *argv[])
{
    namespace po = boost::program_options;

    // Parse program options
    std::string output_file = "";
    std::string cert_file = "";
    std::string retrieve = "part";
    MinknapParams p;

    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "produce help message")
        ("input-data,i", po::value<std::string>()->required(), "set input data (required)")
        ("output-file,o", po::value<std::string>(&output_file), "set output file")
        ("cert-file,c", po::value<std::string>(&cert_file), "set certificate output file")
        ("retrieve,r", po::value<std::string>(&retrieve), "set algorithm")
        ("greedynlogn,g", po::value<StateIdx>(&p.lb_greedynlogn), "")
        ("pairing,p", po::value<StateIdx>(&p.lb_pairing), "")
        ("surrogate,s", po::value<StateIdx>(&p.ub_surrogate), "")
        ("solve-sur,k", po::value<StateIdx>(&p.solve_sur), "")
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

    if (retrieve == "none") {
        opt = opt_minknap_list(instance, p, &info);
    } else if (retrieve == "part") {
        sopt = sopt_minknap_list_part(instance, p, 64, &info);
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
        std::cout << instance.print_opt(opt) << std::endl;
        std::cout << "TIME " << t << std::endl;
    }

    info.write_ini(output_file); // Write output file
    sopt.write_cert(cert_file); // Write certificate file
    return 0;
}
