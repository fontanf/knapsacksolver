#include "../Lib/Instance.hpp"
#include "../Lib/Solution.hpp"

#include <iostream>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/program_options.hpp>

Profit rec(
		Instance& instance, Profit* values,
		ItemIdx i, Weight w)
{
	//ItemIdx n = instance.item_number();
	//ValIdx  x = w*(n+1) + i;
	Weight c = instance.capacity();
	ValIdx x = i*(c+1) + w;

	if (values[x] != -1)
		return values[x];

	if (i == 0) {
		values[x] = 0;
		return 0;
	}

	Profit v0 = rec(instance, values, i-1, w);
	Profit v1 = (w < instance.weight(i))? 0: rec(
			instance, values, i-1, w - instance.weight(i)) + instance.profit(i);

	if (v1 > v0) {
		values[x] = v1;
		return v1;
	} else {
		values[x] = v0;
		return v0;
	}
}

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

	Instance instance(input_data);

	// Initialize memory table
	// values[i][w] == values[w * (n+1) + i]
	// values[i][w] == values[i * (c+1) + w]
	ItemIdx n = instance.item_number();
	ItemIdx c = instance.capacity();
	ValIdx values_size = (n+1)*(c+1);
	Profit* values = new Profit[values_size];
	for (ItemIdx i=0; i<values_size; ++i)
		values[i] = -1;

	// Compute optimal value
	Profit opt = rec(instance, values, n, c);

	// Retrieve optimal solution
	Solution solution(instance);
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
			solution.set(i, true);
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

	delete[] values; // Free memory

	if (verbose)
		std::cout << "OPT: " << opt << std::endl;

	return 0;
}
