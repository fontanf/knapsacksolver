#include "babprimaldual.hpp"

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
	boost::property_tree::ptree pt;
	Solution sol_orig(instance);

	if (algorithm == "partsorted") {
		Instance instance_sorted = Instance::sort_partially_by_efficiency(instance);
		BabPDData data(instance_sorted, &pt, verbose);
		sopt_babprimaldual(data);
		sol_orig = data.sol_best.get_orig();
	} else if (algorithm == "sorted") {
		Instance instance_sorted = Instance::sort_by_efficiency(instance);
		BabPDData data(instance_sorted, &pt, verbose);
		sopt_babprimaldual(data);
		sol_orig = data.sol_best.get_orig();
	} else {
		assert(false);
		std::cout << "Unknwow algorithm" << std::endl;
	}

	// Write output file
	if (output_file != "")
		write_ini(output_file, pt);

	// Write certificate file
	if (cert_file != "") {
		std::ofstream cert;
		cert.open(cert_file);
		cert << sol_orig;
		cert.close();
	}

	return 0;
}