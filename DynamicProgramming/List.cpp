#include "../Lib/Parser.hpp"

#include <queue>
#include <iostream>
#include <list>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/program_options.hpp>

struct Node
{
	ItemIdx i;
	Weight  w;
	Profit  p;
	uint_fast64_t state = 0;
	Node* child_0;
	Node* child_1;
	Node* parent;
};

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


	std::list<Node*> q;

	Node* node = new Node();
	node->i = instance.item_number();
	node->w = instance.capacity();
	node->parent = NULL;
	q.push_front(node);

	while (!q.empty()) {
		Node* node = q.front();
		q.pop_front();
		ItemIdx i = node->i;
		Weight  w = node->w;
		//ValIdx  x = w*(n+1) + i;
		ValIdx x = i*(c+1) + w;
		if (node->state == 0) {
			if (values[x] != -1) {
				node->p = values[x];
				continue;
			}

			if (i == 0) {
				values[x] = 0;
				node->p      = 0;
				continue;
			}

			node->state = 1;
			q.push_front(node);

			Node* n0 = new Node();
			node->child_0 = n0;
			n0->i = i-1;
			n0->w = w;
			n0->parent = node;
			q.push_front(n0);

			if (w >= instance.weight(i)) {
				Node* n1 = new Node();
				node->child_1 = n1;
				n1->i = i-1;
				n1->w = w - instance.weight(i);
				n1->parent = node;
				q.push_front(n1);
			}
		} else if (node->state == 1) {
			Profit p0 = node->child_0->p;
			Profit p1 = (w < instance.weight(i))? 0: node->child_1->p + instance.profit(i);

			values[x] = (p1 > p0)? p1: p0;
			node->p = values[x];
			delete node->child_0;
			if (w >= instance.weight(i))
				delete node->child_1;
		}
	}
	delete node;

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
