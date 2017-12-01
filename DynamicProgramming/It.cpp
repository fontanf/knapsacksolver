#include "../Lib/Parser.hpp"

#include <algorithm>
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
	boost::program_options::options_description desc("Allowed options");
	desc.add_options()
		("help,h", "produce help message")
		("input-data,i",  boost::program_options::value<std::string>(&input_data)->required(), "set input data (required)")
		("output-file,o", boost::program_options::value<std::string>(&output_file),            "set output file")
		("cert-file,c",   boost::program_options::value<std::string>(&cert_file),              "set certificate output file")
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

	KnapsackInstance instance(input_data);

	// Initialize memory table
	// values[i][w] == values[w * (n+1) + i]
	// values[i][w] == values[i * (c+1) + w]
	ItemIdx n = instance.itemNumber();
	ItemIdx c = instance.capacity();
	ValIdx values_size = (n+1)*(c+1);
	Profit* values = new Profit[values_size];

	// Compute optimal value
	for (Weight w=0; w<=instance.capacity(); ++w) {
		//ValIdx x = w*(n+1);
		ValIdx x = w;
		values[x] = 0;
	}
	for (ItemIdx i=1; i<=instance.itemNumber(); ++i) {
		Weight wi = instance.weight(i);
		for (Weight w=0; w<=instance.capacity(); ++w) {
			//ValIdx x  = w*(n+1) + i;
			//ValIdx x0 = w*(n+1) + (i-1);
			//ValIdx x1 = (w-wi)*(n+1) + (i-1);
			ValIdx x  = i*(c+1) + w;
			ValIdx x0 = (i-1)*(c+1) + w;
			ValIdx x1 = (i-1)*(c+1) + (w-wi);
			Profit v0 = values[x0];
			Profit v1 = (w < wi)? 0: values[x1] + instance.profit(i);
			values[x] = (v1 > v0)? v1: v0;
		}
	}
	Profit opt = values[values_size-1];

	// Retrieve optimal solution
	std::vector<bool> solution(n, false);
	ItemIdx i = n;
	Weight  w = c;
	Profit  v = 0;
	while (v < opt) {
		Weight wi = instance.weight(i);
		Profit pi = instance.profit(i);
		//ValIdx x0 = w * (n+1) + (i-1);
		//ValIdx x1 = (w - wi) * (n+1) + (i-1);
		ValIdx x0 = (i-1) * (c+1) + w;
		ValIdx x1 = (i-1) * (c+1) + (w - wi);

		Profit v0 = values[x0];
		Profit v1 = (w < wi)? 0: values[x1] + pi;
		if (v1 > v0) {
			v += pi;
			w -= wi;
			solution[i-1] = true;
		}
		i--;
	}

	// Write output file
	if (output_file != "") {
		boost::property_tree::ptree pt;
		pt.put("Solution.OPT", opt);
		write_ini(output_file, pt);
	}

	// Write certificate file
	if (cert_file != "") {
		std::ofstream cert;
		cert.open(cert_file);
		cert << solution;
		cert.close();
	}

	if (verbose)
		std::cout << "OPT: " << opt << std::endl;

	delete[] values; // Free memory

	return 0;
}
