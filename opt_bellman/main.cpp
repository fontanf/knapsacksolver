#include "bellman.hpp"

#include <iostream>

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

	if (algorithm == "") {
		opt_bellman(instance, &pt, verbose);
	} else if (algorithm == "1") {
		solution = sopt_bellman_1(instance, &pt, verbose);
	} else if (algorithm == "1it") {
		solution = sopt_bellman_1_it(instance, &pt, verbose);
	} else if (algorithm == "1rec") {
		solution = sopt_bellman_1_rec(instance, &pt, verbose);
	} else if (algorithm == "1stack") {
		solution = sopt_bellman_1_stack(instance, &pt, verbose);
	} else if (algorithm == "1map") {
		solution = sopt_bellman_1_map(instance, &pt, verbose);
	} else if (algorithm == "2") {
		solution = sopt_bellman_2(instance, &pt, verbose);
	} else if (algorithm == "rec") {
		solution = sopt_bellman_rec(instance, &pt, verbose);
	} else if (algorithm == "list") {
		opt_bellman_list(instance, &pt, verbose);
	} else if (algorithm == "reclist") {
		solution = sopt_bellman_rec_list(instance, &pt, verbose);
	} else if (algorithm == "ub") {
		Instance instance_sorted = Instance::sort_by_efficiency(instance);
		opt_bellman_ub(instance_sorted, &pt, verbose);
	} else if (algorithm == "recub") {
		Instance instance_sorted = Instance::sort_by_efficiency(instance);
		solution = sopt_bellman_rec_ub(instance_sorted, &pt, verbose).get_orig();
	} else {
		std::cout << "Unknwow algorithm" << std::endl;
	}

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
