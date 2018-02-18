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

    if (algorithm == "opt") {
        opt = std::max(
                sol_best.profit(),
                opt_bellman(instance, &pt, verbose));
    } else if (algorithm == "sopt_1") {
        sol_best.update(sopt_bellman_1(instance, &pt, verbose));
        opt = sol_best.profit();
    } else if (algorithm == "sopt_1it") {
        sol_best.update(sopt_bellman_1_it(instance, &pt, verbose));
        opt = sol_best.profit();
    } else if (algorithm == "sopt_1rec") {
        sol_best.update(sopt_bellman_1_rec(instance, &pt, verbose));
        opt = sol_best.profit();
    } else if (algorithm == "sopt_1stack") {
        sol_best.update(sopt_bellman_1_stack(instance, &pt, verbose));
        opt = sol_best.profit();
    } else if (algorithm == "sopt_1map") {
        sol_best.update(sopt_bellman_1_map(instance, &pt, verbose));
        opt = sol_best.profit();
    } else if (algorithm == "sopt_2") {
        sol_best.update(sopt_bellman_2(instance, &pt, verbose));
        opt = sol_best.profit();
    } else if (algorithm == "sopt_rec") {
        sol_best.update(sopt_bellman_rec(instance, &pt, verbose));
        opt = sol_best.profit();
    } else if (algorithm == "opt_list") {
        opt = std::max(
                sol_best.profit(),
                opt_bellman_list(instance, &pt, verbose));
    } else if (algorithm == "sopt_list_rec") {
        sol_best.update(sopt_bellman_rec_list(instance, &pt, verbose));
        opt = sol_best.profit();
    } else if (algorithm == "opt_ub") {
        instance.sort();
        opt = std::max(
                sol_best.profit(),
                opt_bellman_ub(instance, &pt, verbose));
    } else if (algorithm == "sopt_ub_rec") {
        instance.sort();
        sol_best.update(sopt_bellman_rec_ub(instance, &pt, verbose));
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

    pt.put("Solution.OPT", opt);
    pt.put("Solution.Time", time_span.count());
    if (verbose) {
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
