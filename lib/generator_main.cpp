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
        (",t", po::value<std::string>(&data.t)->required(), "set instance type (u, wc, sc, isc, asc, ss, sw, mstr, pceil, circle)")
        (",n", po::value<ItemIdx>(&data.n)->required(), "set item number")
        (",r", po::value<Profit>(&data.r), "set R")
        ("ka,", po::value<Profit>(&data.k1), "set k1 (for mstr instances)")
        ("kb,", po::value<Profit>(&data.k2), "set k2 (for mstr instances)")
        (",d", po::value<double>(&data.d), "set d (for mstr (6), pceil (3) and circle (3/2) instances)")
        (",H", po::value<int>(&data.h), "set h")
        (",s", po::value<Seed>(&data.s), "set seed")
        ("spanner,sp", "set spanner")
        (",m", po::value<Profit>(&data.m), "set m (for spanner instances)")
        (",v", po::value<Profit>(&data.v), "set v (for spanner instances)")
        (",o", po::value<std::string>(&output_file), "set output file")
        (",p", po::value<std::string>(&plot_file), "set output file")
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

    std::cout << data << std::endl;
    Instance ins = generate(data);

    if (plot_file != "")
        ins.plot(plot_file);
    if (output_file != "")
        ins.write(output_file);

    return 0;
}

