#include "bellman.hpp"

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
		opt_bellman(instance, &pt, verbose);
	} else if (algorithm == "sopt_1") {
		solution = sopt_bellman_1(instance, &pt, verbose);
	} else if (algorithm == "sopt_1it") {
		solution = sopt_bellman_1_it(instance, &pt, verbose);
	} else if (algorithm == "sopt_1rec") {
		solution = sopt_bellman_1_rec(instance, &pt, verbose);
	} else if (algorithm == "sopt_1stack") {
		solution = sopt_bellman_1_stack(instance, &pt, verbose);
	} else if (algorithm == "sopt_1map") {
		solution = sopt_bellman_1_map(instance, &pt, verbose);
	} else if (algorithm == "sopt_2") {
		solution = sopt_bellman_2(instance, &pt, verbose);
	} else if (algorithm == "sopt_rec") {
		solution = sopt_bellman_rec(instance, &pt, verbose);
	} else if (algorithm == "opt_list") {
		opt_bellman_list(instance, &pt, verbose);
	} else if (algorithm == "sopt_list_rec") {
		solution = sopt_bellman_rec_list(instance, &pt, verbose);
	} else if (algorithm == "opt_ub") {
		Instance instance_sorted = Instance::sort_by_efficiency(instance);
		opt_bellman_ub(instance_sorted, &pt, verbose);
	} else if (algorithm == "sopt_ub_rec") {
		Instance instance_sorted = Instance::sort_by_efficiency(instance);
		solution = sopt_bellman_rec_ub(instance_sorted, &pt, verbose).get_orig();
	} else {
		std::cout << "Unknwow algorithm" << std::endl;
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
