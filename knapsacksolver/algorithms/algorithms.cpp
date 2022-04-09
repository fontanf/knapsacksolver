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
    } catch (const po::required_option& e) {
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
    } catch (const po::required_option& e) {
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
    } catch (const po::required_option& e) {
        std::cout << desc << std::endl;;
        throw "";
    }
    if (vm.count("greedy")) p.greedy = true;
    if (vm.count("combo-core")) p.combo_core = true;
    return p;
}

Output knapsacksolver::run(
        std::string algorithm, Instance& instance, std::mt19937_64&, Info info)
{
    std::vector<std::string> algorithm_args = po::split_unix(algorithm);
    std::vector<char*> algorithm_argv;
    for(Counter i = 0; i < (Counter)algorithm_args.size(); ++i)
        algorithm_argv.push_back(const_cast<char*>(algorithm_args[i].c_str()));

    if (algorithm.empty() || algorithm_args[0].empty()) {
        throw std::invalid_argument("Missing algorithm.");

        /*
         * Lower bounds
         */
    } else if (algorithm_args[0] == "greedy") {
        instance.sort_partially(FFOT_DBG(info));
        return greedy(instance, info);
    } else if (algorithm_args[0] == "greedynlogn") {
        instance.sort_partially(FFOT_DBG(info));
        return greedynlogn(instance, info);
    } else if (algorithm_args[0] == "greedynlogn_for") {
        instance.sort_partially(FFOT_DBG(info));
        return forwardgreedynlogn(instance, info);
    } else if (algorithm_args[0] == "greedynlogn_back") {
        instance.sort_partially(FFOT_DBG(info));
        return backwardgreedynlogn(instance, info);

        /*
         * Exact argsrithms
         */
    } else if (algorithm_args[0] == "bellman_array") { // Bellman
        return bellman_array(instance, info);
    } else if (algorithm_args[0] == "bellmanpar_array") {
        return bellmanpar_array(instance, info);
    } else if (algorithm_args[0] == "bellman_rec") {
        return bellmanrec(instance, info);
    } else if (algorithm_args[0] == "bellman_array_all") {
        return bellman_array_all(instance, info);
    } else if (algorithm_args[0] == "bellman_array_one") {
        return bellman_array_one(instance, info);
    } else if (algorithm_args[0] == "bellman_array_part") {
        return bellman_array_part(instance, 64, info);
    } else if (algorithm_args[0] == "bellman_array_rec") {
        return bellman_array_rec(instance, info);
    } else if (algorithm_args[0] == "bellman_list") {
        return bellman_list(instance, false, info);
    } else if (algorithm_args[0] == "bellman_list_sort") {
        return bellman_list(instance, true, info);
    } else if (algorithm_args[0] == "bellman_list_rec") {
        return bellman_list_rec(instance, info);
    } else if (algorithm_args[0] == "dpprofits_array") { // DPProfits
        return dpprofits_array(instance, info);
    } else if (algorithm_args[0] == "dpprofits_array_all") {
        return dpprofits_array_all(instance, info);
    } else if (algorithm_args[0] == "branchandbound") { // Branch-and-bound
        return branchandbound(instance, false, info);
    } else if (algorithm_args[0] == "branchandbound_sort") {
        return branchandbound(instance, true, info);
    } else if (algorithm_args[0] == "expknap") { // Expknap
        ExpknapOptionalParameters p = read_expknap_args(algorithm_argv);
        p.info = info;
        return expknap(instance, p);
    } else if (algorithm_args[0] == "expknap_combo") {
        auto p = ExpknapOptionalParameters().set_combo();
        p.info = info;
        return expknap(instance, p);
    } else if (algorithm_args[0] == "balknap") { // Balknap
        BalknapOptionalParameters p = read_balknap_args(algorithm_argv);
        p.info = info;
        return balknap(instance, p);
    } else if (algorithm_args[0] == "balknap_combo") {
        auto p = BalknapOptionalParameters().set_combo();
        p.info = info;
        return balknap(instance, p);
    } else if (algorithm_args[0] == "minknap") { // Minknap
        MinknapOptionalParameters p = read_minknap_args(algorithm_argv);
        p.info = info;
        return minknap(instance, p);
    } else if (algorithm_args[0] == "minknap_combo" || algorithm_args[0] == "combo") {
        auto p = MinknapOptionalParameters().set_combo();
        p.info = info;
        return minknap(instance, p);

        /*
         * Upper bounds
         */
    } else if (algorithm_args[0] == "dantzig") { // Dantzig
        Info info_tmp;
        Output output(instance, info_tmp);
        instance.sort_partially(FFOT_DBG(info_tmp));
        output.upper_bound = ub_dantzig(instance, info);
        return output;
    } else if (algorithm_args[0] == "surrelax") { // Surrogate relaxation
        return surrelax(instance, info);
    } else if (algorithm_args[0] == "surrelax_minknap") { // Surrogate relaxation
        return surrelax_minknap(instance, info);

    } else {
        throw std::invalid_argument(
                "Unknown algorithm \"" + algorithm_args[0] + "\".");
    }
}

