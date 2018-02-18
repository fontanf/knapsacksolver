#include "dpprofits.hpp"
#include "../ub_dantzig/dantzig.hpp"
#include "../ub_surrogate/surrogate.hpp"
#include "../lb_greedy/greedy.hpp"

#include <iostream>
#include <chrono>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/program_options.hpp>

//#define DBG(x)
#define DBG(x) x

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
    Profit opt;
    boost::property_tree::ptree pt;

    std::chrono::high_resolution_clock::time_point t1
        = std::chrono::high_resolution_clock::now();

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
    Profit ub = ub_surrogate(instance, sol_best.profit()).ub;
    if (verbose) {
        std::cout << "LB " << sol_best.profit() << " GAP " << instance.optimum() - sol_best.profit() << std::endl;
        std::cout << "UB " << ub << " GAP " << ub - instance.optimum() << std::endl;
    }

    if (sol_best.profit() == ub) {
        opt = sol_best.profit();
    } else if (algorithm == "opt") {
        opt = std::max(
                sol_best.profit(),
                opt_dpprofits(instance, ub, &pt, verbose));
    } else if (algorithm == "sopt") {
        sol_best.update(sopt_dpprofits_1(instance, ub, &pt, verbose));
        opt = sol_best.profit();
    } else {
        std::cerr << "Unknown or missing algorithm" << std::endl;
        assert(false);
        return 1;
    }

    std::chrono::high_resolution_clock::time_point t2
        = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> time_span
        = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);

    pt.put("Solution.Time", time_span.count());
    pt.put("Solution.OPT", opt);
    if (verbose) {
        std::cout << "---" << std::endl;
        std::cout << "OPT " << opt << std::endl;
        std::cout << "EXP " << instance.optimum() << std::endl;
        std::cout << "Time " << time_span.count() << std::endl;
    }

    // Write output file
    if (output_file != "")
        write_ini(output_file, pt);

    // Write certificate file
    if (cert_file != "") {
        std::ofstream cert;
        cert.open(cert_file);
        cert << sol_best;
        cert.close();
    }

    return 0;
}
