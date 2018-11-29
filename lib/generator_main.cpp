#include "knapsack/lib/generator.hpp"

using namespace knapsack;

int main(int argc, char *argv[])
{
    namespace po = boost::program_options;

    // Parse program options
    GenerateData data;
    std::string output_file = "";
    std::string plot_file = "";
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "produce help message")
        (",t", po::value<std::string>(&data.type)->required(), "set instance type")
        (",n", po::value<ItemIdx>(&data.n)->required(), "set item number")
        (",r", po::value<Profit>(&data.r), "set R")
        (",h", po::value<int>(&data.h), "set h")
        (",s", po::value<Seed>(&data.seed), "set seed")
        ("spanner,sp", "set spanner")
        (",m", po::value<Profit>(&data.m), "set m (for spanner instances)")
        (",v", po::value<Profit>(&data.v), "set v (for spanner instances)")
        (",o", po::value<std::string>(&output_file)->required(), "set output file")
        (",p", po::value<std::string>(&plot_file)->required(), "set output file")
        ;
    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    if (vm.count("help")) {
        std::cout << desc << std::endl;;
        return 1;
    }
    try {
        po::notify(vm);
    } catch (po::required_option e) {
        std::cout << desc << std::endl;;
        return 1;
    }
    data.spanner = vm.count("spanner");

    Instance ins = generate(data);

    if (plot_file != "")
        ins.plot(plot_file);
    if (output_file != "")
        ins.write(output_file);

    return 0;
}

