#include "bellman.hpp"

#include "../lb_greedy/greedy.hpp"

#include <iostream>
#include <chrono>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/program_options.hpp>

int main(int argc, char *argv[])
{
    // Parse program options
    std::string input_data  = "";
    std::string output_file = "";
    std::string cert_file   = "";
    std::string algorithm   = "";
    std::string reduction   = "";
    boost::program_options::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "produce help message")
        ("input-data,i",  boost::program_options::value<std::string>(&input_data)->required(), "set input data (required)")
        ("output-file,o", boost::program_options::value<std::string>(&output_file),            "set output file")
        ("cert-file,c",   boost::program_options::value<std::string>(&cert_file),              "set certificate output file")
        ("algorithm,a",   boost::program_options::value<std::string>(&algorithm),              "set algorithm")
        ("reduction,r",   boost::program_options::value<std::string>(&reduction),              "choose variable reduction")
        ("verbose,v",                                                                          "enable verbosity")
        ;
    boost::program_options::variables_map vm;
    boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
    if (vm.count("help")) {
        std::cout << desc << std::endl;;
        return 1;
    }
    try {
        boost::program_options::notify(vm);
    } catch (boost::program_options::required_option e) {
        std::cout << desc << std::endl;;
        return 1;
    }
    bool verbose = vm.count("verbose");

    Instance instance(input_data);
    Solution sol_best(instance);
    Profit opt;
    Info info;
    info.verbose(verbose);

    // Variable reduction
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

    if (algorithm == "opt") {
        opt = std::max(
                sol_best.profit(),
                opt_bellman(instance, &info));
    } else if (algorithm == "sopt_1") {
        sol_best.update(sopt_bellman_1(instance, &info));
        opt = sol_best.profit();
    } else if (algorithm == "sopt_1it") {
        sol_best.update(sopt_bellman_1_it(instance, &info));
        opt = sol_best.profit();
    } else if (algorithm == "sopt_1rec") {
        sol_best.update(sopt_bellman_1_rec(instance, &info));
        opt = sol_best.profit();
    } else if (algorithm == "sopt_1stack") {
        sol_best.update(sopt_bellman_1_stack(instance, &info));
        opt = sol_best.profit();
    } else if (algorithm == "sopt_1map") {
        sol_best.update(sopt_bellman_1_map(instance, &info));
        opt = sol_best.profit();
    } else if (algorithm == "sopt_2") {
        sol_best.update(sopt_bellman_2(instance, &info));
        opt = sol_best.profit();
    } else if (algorithm == "sopt_rec") {
        sol_best.update(sopt_bellman_rec(instance, &info));
        opt = sol_best.profit();
    } else if (algorithm == "opt_list") {
        opt = std::max(
                sol_best.profit(),
                opt_bellman_list(instance, &info));
    } else if (algorithm == "sopt_list_rec") {
        sol_best.update(sopt_bellman_rec_list(instance, &info));
        opt = sol_best.profit();
    } else if (algorithm == "opt_ub") {
        instance.sort();
        opt = std::max(
                sol_best.profit(),
                opt_bellman_ub(instance, &info));
    } else if (algorithm == "sopt_ub_rec") {
        instance.sort();
        sol_best.update(sopt_bellman_rec_ub(instance, &info));
        opt = sol_best.profit();
    } else {
        std::cerr << "Unknown or missing algorithm" << std::endl;
        assert(false);
        return 1;
    }

    double t = info.elapsed_time();
    info.pt.put("Solution.OPT", opt);
    info.pt.put("Solution.Time", t);
    if (verbose) {
        std::cout << "OPT " << opt << std::endl;
        std::cout << "EXP " << instance.optimum() << std::endl;
        std::cout << "Time " << t << std::endl;
    }

    info.write_ini(output_file); // Write output file
    sol_best.write_cert(cert_file); // Write certificate file
    return 0;
}
