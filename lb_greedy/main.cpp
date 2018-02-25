#include "greedy.hpp"

#include <iostream>
#include "chrono"

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
    boost::program_options::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "produce help message")
        ("input-data,i",  boost::program_options::value<std::string>(&input_data)->required(), "set input data (required)")
        ("output-file,o", boost::program_options::value<std::string>(&output_file),            "set output file")
        ("cert-file,c",   boost::program_options::value<std::string>(&cert_file),              "set certificate output file")
        ("algorithm,a",   boost::program_options::value<std::string>(&algorithm),              "set algorithm")
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

    Instance instance(input_data);
    Solution sol_best(instance);
    Info info;
    info.verbose(vm.count("verbose"));

    instance.sort_partially();

    if (algorithm == "") {
        sol_best = sol_greedy(instance, &info);
    } else if (algorithm == "max") {
        sol_best = sol_greedymax(instance, &info);
    } else if (algorithm == "for") {
        sol_best = sol_forwardgreedy(instance, &info);
    } else if (algorithm == "back") {
        sol_best = sol_backwardgreedy(instance, &info);
    } else if (algorithm == "best") {
        sol_best = sol_bestgreedy(instance, &info);
    }

    double t = info.elapsed_time();
    info.pt.put("Solution.Time", t);
    info.pt.put("Solution.OPT", sol_best.profit());
    if (Info::verbose(&info)) {
        std::cout << "OPT " << sol_best.profit() << std::endl;
        std::cout << "GAP " << instance.optimum() - sol_best.profit() << std::endl;
        std::cout << "TIME " << t << std::endl;
    }

    info.write_ini(output_file); // Write output file
    sol_best.write_cert(cert_file); // Write certificate file
    return 0;
}
