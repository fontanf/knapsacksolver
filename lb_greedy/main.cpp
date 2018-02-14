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
	bool verbose = vm.count("verbose");

	Instance instance(input_data);
	Solution sol_best(instance);
	boost::property_tree::ptree pt;

	std::chrono::high_resolution_clock::time_point t1
		= std::chrono::high_resolution_clock::now();

	instance.sort_partially();

	if (algorithm == "") {
		sol_best = sol_greedy(instance, &pt, verbose);
	} else if (algorithm == "max") {
		sol_best = sol_greedymax(instance, &pt, verbose);
	} else if (algorithm == "for") {
		sol_best = sol_forwardgreedy(instance, &pt, verbose);
	} else if (algorithm == "back") {
		sol_best = sol_backwardgreedy(instance, &pt, verbose);
	} else if (algorithm == "best") {
		sol_best = sol_bestgreedy(instance, &pt, verbose);
	}

	std::chrono::high_resolution_clock::time_point t2
		= std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> time_span
		= std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);

	pt.put("Solution.Time", time_span.count());
	pt.put("Solution.OPT", sol_best.profit());
	if (verbose) {
		std::cout << "OPT " << sol_best.profit() << std::endl;
		std::cout << "GAP " << instance.optimum() - sol_best.profit() << std::endl;
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
