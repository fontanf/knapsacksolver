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
    namespace po = boost::program_options;

    // Parse program options
    std::string input_data  = "";
    std::string output_file = "";
    std::string cert_file   = "";
    std::string algorithm   = "sopt_1";
    std::string reduction   = "";
    std::string lower_bound = "none";
    std::string upper_bound = "none";
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "produce help message")
        ("input-data,i",  po::value<std::string>(&input_data)->required(), "set input data (required)")
        ("output-file,o", po::value<std::string>(&output_file),            "set output file")
        ("cert-file,c",   po::value<std::string>(&cert_file),              "set certificate output file")
        ("algorithm,a",   po::value<std::string>(&algorithm),              "set algorithm")
        ("reduction,r",   po::value<std::string>(&reduction),              "choose variable reduction")
        ("lower-bound,l", po::value<std::string>(&lower_bound),            "set lower bound")
        ("upper-bound,u", po::value<std::string>(&upper_bound),            "set upper bound")
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
    bool verbose = vm.count("verbose");

    Instance instance(input_data);
    Solution sol_best(instance);
    Profit opt;
    Info info;
    info.verbose(verbose);

    // Variable reduction
    bool optimal = false;
    if (reduction == "") {
        instance.sort_partially();
        sol_best = sol_bestgreedy(instance);
    } else if (reduction == "1") {
        instance.sort_partially();
        sol_best = sol_bestgreedy(instance);
        optimal = instance.reduce1(sol_best, verbose);
    } else if (reduction == "2") {
        instance.sort();
        sol_best = sol_bestgreedy(instance);
        optimal = instance.reduce2(sol_best, verbose);
    }

    // Bellman
    if (!optimal) {
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
            if (upper_bound != "none")
                instance.sort();
            opt = std::max(
                    sol_best.profit(),
                    opt_bellman_list(instance, sol_best.profit(), lower_bound, upper_bound, &info));
        } else if (algorithm == "sopt_list_rec") {
            if (upper_bound != "none")
                instance.sort();
            sol_best.update(sopt_bellman_rec_list(instance, sol_best, lower_bound, upper_bound, &info));
            opt = sol_best.profit();
        } else {
            std::cerr << "Unknown or missing algorithm" << std::endl;
            assert(false);
            return 1;
        }
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
