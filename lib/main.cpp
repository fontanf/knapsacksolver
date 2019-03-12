#include "knapsack/opt_bellman/bellman.hpp"
#include "knapsack/opt_dpprofits/dpprofits.hpp"
#include "knapsack/opt_bab/bab.hpp"
#include "knapsack/opt_astar/astar.hpp"
#include "knapsack/opt_balknap/balknap.hpp"
#include "knapsack/opt_minknap/minknap.hpp"
#include "knapsack/opt_expknap/expknap.hpp"
#include "knapsack/lb_greedy/greedy.hpp"
#include "knapsack/lb_greedynlogn/greedynlogn.hpp"
#include "knapsack/ub_dantzig/dantzig.hpp"
#include "knapsack/ub_surrogate/surrogate.hpp"

using namespace knapsack;

int main(int argc, char *argv[])
{
    namespace po = boost::program_options;

    // Parse program options
    std::string algorithm = "bellman_array";
    std::string instance_filepath = "";
    std::string output_filepath = "";
    std::string format = "knapsack_standard";
    std::string cert_filepath = "";
    std::string log_filepath = "";
    int log_levelmax = 999;

    ItemPos k = 64;

    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "produce help message")
        ("algorithm,a", po::value<std::string>(&algorithm), "set algorithm")
        ("input,i", po::value<std::string>(&instance_filepath)->required(), "set input file (required)")
        ("format,f", po::value<std::string>(&format), "set input file format (default: knapsack_standard)")
        ("output,o", po::value<std::string>(&output_filepath), "set output file")
        ("cert,c", po::value<std::string>(&cert_filepath), "set certificate file")
        ("part-size,x", po::value<ItemPos>(&k), "")
        ("verbose,v", "")
        ("log,l", po::value<std::string>(&log_filepath), "set log file")
        ("loglevelmax", po::value<int>(&log_levelmax), "set log max level")
        ("log2stderr", "write log in stderr")
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

    if (!boost::filesystem::exists(instance_filepath)) {
        std::cerr << instance_filepath << ": file not found." << std::endl;
        return 1;
    }

    Instance ins(instance_filepath, format);
    Solution sopt(ins);

    Logger logger(log_filepath, vm.count("log2stderr"), log_levelmax);
    Info info(logger, vm.count("verbose"));

    if (algorithm == "bellman_array") { // bellman
        opt_bellman_array(ins, info);
    } else if (algorithm == "bellman_array_all") {
        sopt = sopt_bellman_array_all(ins, info);
    } else if (algorithm == "bellman_array_one") {
        sopt = sopt_bellman_array_one(ins, info);
    } else if (algorithm == "bellman_array_part") {
        sopt = sopt_bellman_array_part(ins, info, 64);
    } else if (algorithm == "bellman_array_rec") {
        sopt = sopt_bellman_array_rec(ins, info);
    } else if (algorithm == "bellman_list") {
        opt_bellman_list(ins, info);
    } else if (algorithm == "bellman_list_rec") {
        sopt = sopt_bellman_list_rec(ins, info);
    } else if (algorithm == "dpprofits_array") { // dpprofits
        opt_dpprofits_array(ins, info);
    } else if (algorithm == "dpprofits_array_all") {
        sopt = sopt_dpprofits_array_all(ins, info);
    } else if (algorithm == "bab") { // bab
        sopt = sopt_bab(ins, info);
    } else if (algorithm == "astar") { // astar
        sopt = sopt_astar(ins, info);
    } else if (algorithm == "expknap") { // expknap
        ExpknapParams p;
        sopt = sopt_expknap(ins, info, p);
    } else if (algorithm == "balknap") { // balknap
        BalknapParams p;
        sopt = sopt_balknap(ins, info, p, k);
    } else if (algorithm == "minknap") { // minknap
        MinknapParams p;
        p.k = k;
        sopt = Minknap(ins, p).run(info);
    } else if (algorithm == "greedy") { // greedy
        ins.sort_partially(info);
        sopt = sol_greedy(ins, info);
    } else if (algorithm == "greedynlogn") { // greedynlogn
        sopt = sol_greedynlogn(ins, info);
    } else if (algorithm == "greedynlogn_for") {
        sopt = sol_forwardgreedynlogn(ins, info);
    } else if (algorithm == "greedynlogn_back") {
        sopt = sol_backwardgreedynlogn(ins, info);
    } else if (algorithm == "dantzig") { // dantzig
        ub_dantzig(ins, info);
    } else if (algorithm == "surrelax") { // surrelax
        ins.sort_partially(info);
        Info info_tmp(info.logger);
        Solution sol = sol_greedynlogn(ins, info_tmp);
        ub_surrogate(ins, sol.profit(), info);
    }

    info.write_ini(output_filepath); // Write output file
    sopt.write_cert(cert_filepath); // Write certificate file
    return 0;
}

