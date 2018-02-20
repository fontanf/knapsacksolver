#include "bab.hpp"

#include "../lb_greedy/greedy.hpp"

#include <iostream>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/program_options.hpp>

int main(int argc, char *argv[])
{
    namespace po = boost::program_options;

    // Parse program options
    std::string input_data  = "";
    std::string output_file = "";
    std::string cert_file   = "";
    std::string algorithm   = "";
    std::string reduction   = "";
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "produce help message")
        ("input-data,i",  po::value<std::string>(&input_data)->required(), "set input data (required)")
        ("output-file,o", po::value<std::string>(&output_file),            "set output file")
        ("cert-file,c",   po::value<std::string>(&cert_file),              "set certificate output file")
        ("algorithm,a",   po::value<std::string>(&algorithm),              "set algorithm")
        ("reduction,r",   po::value<std::string>(&reduction),              "set reduction")
        ("verbose,v",                                                                          "enable verbosity")
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
    bool verbose = vm.count("verbose");

    Instance instance(input_data);
    Solution sol_best(instance);
    Info info;
    info.verbose(verbose);

    if (reduction == "") {
        instance.sort_partially();
        sol_best = sol_bestgreedy(instance);
    } else if (reduction == "1") {
        instance.sort_partially();
        sol_best = sol_bestgreedy(instance);
        instance.reduce1(sol_best, verbose);
    } else if (reduction == "2") {
        instance.sort();
        sol_best = sol_bestgreedy(instance);
        instance.reduce2(sol_best, verbose);
    }

    BabData data(instance, &info);
    if (algorithm == "") {
        instance.sort();
        sopt_bab(data);
    } else if (algorithm == "rec") {
        instance.sort();
        sopt_bab_rec(data);
    } else if (algorithm == "stack") {
        instance.sort();
        sopt_bab_stack(data);
    } else {
        std::cerr << "Unknown or missing algorithm" << std::endl;
        assert(false);
        return 1;
    }
    sol_best.update(data.sol_best);

    double t = info.elapsed_time();
    info.pt.put("Solution.Time", t);
    info.pt.put("Solution.OPT", sol_best.profit());
    if (verbose) {
        std::cout << "OPT " << sol_best.profit() << std::endl;
        std::cout << "EXP " << instance.optimum() << std::endl;
        std::cout << "TIME " << t << std::endl;
    }

    // Write output file
    if (output_file != "")
        write_ini(output_file, info.pt);

    // Write certificate file
    if (cert_file != "") {
        std::ofstream cert;
        cert.open(cert_file);
        cert << data.sol_best;
        cert.close();
    }

    return 0;
}
