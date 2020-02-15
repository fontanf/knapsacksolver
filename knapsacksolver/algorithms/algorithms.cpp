#include "knapsacksolver/algorithms/algorithms.hpp"

#include <boost/program_options.hpp>

using namespace knapsacksolver;
namespace po = boost::program_options;

ExpknapOptionalParameters read_expknap_args(std::vector<char*> argv)
{
    ExpknapOptionalParameters p;
    po::options_description desc("Allowed options");
    desc.add_options()
        ("greedy,g", "")
        ("greedynlogn,n", po::value<StateIdx>(&p.greedynlogn), "")
        ("surrelax,s", po::value<StateIdx>(&p.surrelax), "")
        ("combo-core,c", "")
        ;
    po::variables_map vm;
    po::store(po::parse_command_line((Counter)argv.size(), argv.data(), desc), vm);
    try {
        po::notify(vm);
    } catch (po::required_option e) {
        std::cout << desc << std::endl;;
        throw "";
    }
    if (vm.count("greedy")) p.greedy = true;
    if (vm.count("combo-core")) p.combo_core = true;
    return p;
}

BalknapOptionalParameters read_balknap_args(std::vector<char*> argv)
{
    BalknapOptionalParameters p;
    po::options_description desc("Allowed options");
    desc.add_options()
        ("upper-bound,u", po::value<char>(&p.ub), "")
        ("greedy,g", "")
        ("greedynlogn,n", po::value<StateIdx>(&p.greedynlogn), "")
        ("surrelax,s", po::value<StateIdx>(&p.surrelax), "")
        ("partial-solution-size,k", po::value<ItemIdx>(&p.partial_solution_size), "")
        ;
    po::variables_map vm;
    po::store(po::parse_command_line((Counter)argv.size(), argv.data(), desc), vm);
    try {
        po::notify(vm);
    } catch (po::required_option e) {
        std::cout << desc << std::endl;;
        throw "";
    }
    if (vm.count("greedy")) p.greedy = true;
    return p;
}

MinknapOptionalParameters read_minknap_args(std::vector<char*> argv)
{
    MinknapOptionalParameters p;
    po::options_description desc("Allowed options");
    desc.add_options()
        ("greedy,g", "")
        ("pairing,p", po::value<StateIdx>(&p.pairing), "")
        ("surrelax,s", po::value<StateIdx>(&p.surrelax), "")
        ("combo-core,c", "")
        ("partial-solution-size,k", po::value<ItemIdx>(&p.partial_solution_size), "")
        ;
    po::variables_map vm;
    po::store(po::parse_command_line((Counter)argv.size(), argv.data(), desc), vm);
    try {
        po::notify(vm);
    } catch (po::required_option e) {
        std::cout << desc << std::endl;;
        throw "";
    }
    if (vm.count("greedy")) p.greedy = true;
    if (vm.count("combo-core")) p.combo_core = true;
    return p;
}

func knapsacksolver::get_algorithm(std::string algorithm)
{
    std::vector<std::string> algorithm_args = po::split_unix(algorithm);
    std::vector<char*> algorithm_argv;
    for(Counter i = 0; i < (Counter)algorithm_args.size(); ++i)
        algorithm_argv.push_back(const_cast<char*>(algorithm_args[i].c_str()));

    if (algorithm_args[0] == "") {
        std::cerr << "\033[32m" << "ERROR, missing argsrithm." << "\033[0m" << std::endl;
        return [](Instance& ins, std::mt19937_64&, Info info) { return Output(ins, info); };

    /*
     * Lower bounds
     */
    } else if (algorithm_args[0] == "greedy") {
        return [](Instance& ins, std::mt19937_64&, Info info) {
            ins.sort_partially(info);
            return greedy(ins, info);
        };
    } else if (algorithm_args[0] == "greedynlogn") {
        return [](Instance& ins, std::mt19937_64&, Info info) {
            ins.sort_partially(info);
            return greedynlogn(ins, info);
        };
    } else if (algorithm_args[0] == "greedynlogn_for") {
        return [](Instance& ins, std::mt19937_64&, Info info) {
            ins.sort_partially(info);
            return forwardgreedynlogn(ins, info);
        };
    } else if (algorithm_args[0] == "greedynlogn_back") {
        return [](Instance& ins, std::mt19937_64&, Info info) {
            ins.sort_partially(info);
            return backwardgreedynlogn(ins, info);
        };

    /*
     * Exact argsrithms
     */
    } else if (algorithm_args[0] == "bellman_array") { // Bellman
        return [](Instance& ins, std::mt19937_64&, Info info) {
            return bellman_array(ins, info);
        };
    } else if (algorithm_args[0] == "bellmanpar_array") {
        return [](Instance& ins, std::mt19937_64&, Info info) {
            return bellmanpar_array(ins, info);
        };
    } else if (algorithm_args[0] == "bellman_rec") {
        return [](Instance& ins, std::mt19937_64&, Info info) {
            return bellmanrec(ins, info);
        };
    } else if (algorithm_args[0] == "bellman_array_all") {
        return [](Instance& ins, std::mt19937_64&, Info info) {
            return bellman_array_all(ins, info);
        };
    } else if (algorithm_args[0] == "bellman_array_one") {
        return [](Instance& ins, std::mt19937_64&, Info info) {
            return bellman_array_one(ins, info);
        };
    } else if (algorithm_args[0] == "bellman_array_part") {
        return [](Instance& ins, std::mt19937_64&, Info info) {
            return bellman_array_part(ins, 64, info);
        };
    } else if (algorithm_args[0] == "bellman_array_rec") {
        return [](Instance& ins, std::mt19937_64&, Info info) {
            return bellman_array_rec(ins, info);
        };
    } else if (algorithm_args[0] == "bellman_list") {
        return [](Instance& ins, std::mt19937_64&, Info info) {
            return bellman_list(ins, false, info);
        };
    } else if (algorithm_args[0] == "bellman_list_sort") {
        return [](Instance& ins, std::mt19937_64&, Info info) {
            return bellman_list(ins, true, info);
        };
    } else if (algorithm_args[0] == "bellman_list_rec") {
        return [](Instance& ins, std::mt19937_64&, Info info) {
            return bellman_list_rec(ins, info);
        };
    } else if (algorithm_args[0] == "dpprofits_array") { // DPProfits
        return [](Instance& ins, std::mt19937_64&, Info info) {
            return dpprofits_array(ins, info);
        };
    } else if (algorithm_args[0] == "dpprofits_array_all") {
        return [](Instance& ins, std::mt19937_64&, Info info) {
            return dpprofits_array_all(ins, info);
        };
    } else if (algorithm_args[0] == "branchandbound") { // Branch-and-bound
        return [](Instance& ins, std::mt19937_64&, Info info) {
            return branchandbound(ins, false, info);
        };
    } else if (algorithm_args[0] == "branchandbound_sort") {
        return [](Instance& ins, std::mt19937_64&, Info info) {
            return branchandbound(ins, true, info);
        };
    } else if (algorithm_args[0] == "expknap") { // Expknap
        return [algorithm_argv](Instance& ins, std::mt19937_64&, Info info) {
            ExpknapOptionalParameters p = read_expknap_args(algorithm_argv);
            p.info = info;
            return expknap(ins, p);
        };
    } else if (algorithm_args[0] == "expknap_combo") {
        return [](Instance& ins, std::mt19937_64&, Info info) {
            auto p = ExpknapOptionalParameters().set_combo();
            p.info = info;
            return expknap(ins, p);
        };
    } else if (algorithm_args[0] == "balknap") { // Balknap
        return [algorithm_argv](Instance& ins, std::mt19937_64&, Info info) {
            BalknapOptionalParameters p = read_balknap_args(algorithm_argv);
            p.info = info;
            return balknap(ins, p);
        };
    } else if (algorithm_args[0] == "balknap_combo") {
        return [](Instance& ins, std::mt19937_64&, Info info) {
            auto p = BalknapOptionalParameters().set_combo();
            p.info = info;
            return balknap(ins, p);
        };
    } else if (algorithm_args[0] == "minknap") { // Minknap
        return [algorithm_argv](Instance& ins, std::mt19937_64&, Info info) {
            MinknapOptionalParameters p = read_minknap_args(algorithm_argv);
            p.info = info;
            return minknap(ins, p);
        };
    } else if (algorithm_args[0] == "minknap_combo" || algorithm_args[0] == "combo") {
        return [](Instance& ins, std::mt19937_64&, Info info) {
            auto p = MinknapOptionalParameters().set_combo();
            p.info = info;
            return minknap(ins, p);
        };

    /*
     * Upper bounds
     */
    } else if (algorithm_args[0] == "dantzig") { // Dantzig
        return [](Instance& ins, std::mt19937_64&, Info info) {
            Output output(ins, info);
            output.upper_bound = ub_dantzig(ins, info);
            return output;
        };
    } else if (algorithm_args[0] == "surrelax") { // Surrogate relaxation
        return [](Instance& ins, std::mt19937_64&, Info info) {
            return surrelax(ins, info);
        };
    } else if (algorithm_args[0] == "surrelax_minknap") { // Surrogate relaxation
        return [](Instance& ins, std::mt19937_64&, Info info) {
            return surrelax_minknap(ins, info);
        };


    } else {
        std::cerr << "\033[31m" << "ERROR, unknown algorithm: " << algorithm_args[0] << "\033[0m" << std::endl;
        assert(false);
        return [](Instance& ins, std::mt19937_64&, Info info) { return Output(ins, info); };
    }
}

