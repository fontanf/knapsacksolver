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

	KnapsackInstance instance(input_data);

	// Initialize memory table
	Profit** values = new Profit*[instance.itemNumber()+1];
	for (ItemIdx i=0; i<=instance.itemNumber(); ++i) {
		values[i] = new Profit[instance.capacity() + 1];
		for (Weight j=0; j<=instance.capacity(); ++j)
			values[i][j] = -1;
	}

	std::list<Node*> q;

	Node* n = new Node();
	n->i = instance.itemNumber();
	n->w = instance.capacity();
	n->parent = NULL;
	q.push_front(n);

	while (!q.empty()) {
		Node* node = q.front();
		q.pop_front();
		ItemIdx i = node->i;
		Weight  w = node->w;
		if (node->state == 0) {
			if (values[i][w] != -1) {
				node->p = values[i][w];
				continue;
			}

			if (i == 0) {
				values[i][w] = 0;
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

			values[i][w] = (p1 > p0)? p1: p0;
			node->p = values[i][w];
			delete node->child_0;
			if (w >= instance.weight(i))
				delete node->child_1;
		}
	}
	delete n;

	Profit opt = values[instance.itemNumber()][instance.capacity()];

	// Retrieve optimal solution
	std::vector<bool> solution(instance.itemNumber(), false);
	ItemIdx i = instance.itemNumber();
	Weight  w = instance.capacity();
	Profit  v = 0;
	while (v < opt) {
		Profit v0 = values[i-1][w];
		Profit v1 = (w < instance.weight(i))? 0:
			values[i-1][w - instance.weight(i)] + instance.profit(i);
		if (v1 > v0) {
			v += instance.profit(i);
			w -= instance.weight(i);
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

	// Free memory
	for (ItemIdx i=0; i<=instance.itemNumber(); ++i)
		delete[] values[i];
	delete[] values;

	return 0;
}
