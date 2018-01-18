#include "dpprofits.hpp"
#include "../ub_dantzig/dantzig.hpp"
#include "../ub_surrogate/surrogate.hpp"
#include "../lb_greedy/greedy.hpp"
#include "../lb_ls/ls.hpp"

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
	bool verbose = vm.count("verbose");

	Instance instance(input_data);
	Solution solution(instance);
	boost::property_tree::ptree pt;

	std::chrono::high_resolution_clock::time_point t1
		= std::chrono::high_resolution_clock::now();

	if (algorithm == "opt") {
		Instance instance_sorted = Instance::sort_partially_by_efficiency(instance);
		Profit lb = lb_extgreedy(instance_sorted);
		Profit ub = ub_surrogate(instance_sorted, lb).ub;
		opt_dpprofits(instance_sorted, ub, &pt, verbose);
	} else if (algorithm == "sopt") {
		Instance instance_sorted = Instance::sort_partially_by_efficiency(instance);
		Profit lb = lb_extgreedy(instance_sorted);
		Profit ub = ub_surrogate(instance_sorted, lb).ub;
		solution = sopt_dpprofits_1(instance_sorted, ub, &pt, verbose).get_orig();
	}

	std::chrono::high_resolution_clock::time_point t2
		= std::chrono::high_resolution_clock::now();

	std::chrono::duration<double> time_span
		= std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);

	pt.put("Solution.Time", time_span.count());
	if (verbose)
		std::cout << "Time " << time_span.count() << std::endl;

	// Write output file
	if (output_file != "")
		write_ini(output_file, pt);

	// Write certificate file
	if (cert_file != "") {
		std::ofstream cert;
		cert.open(cert_file);
		cert << solution;
		cert.close();
	}

	return 0;
}
