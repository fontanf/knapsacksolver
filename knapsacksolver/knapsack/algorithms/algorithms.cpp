#include "knapsacksolver/knapsack/algorithms/algorithms.hpp"

#include <boost/program_options.hpp>

using namespace knapsacksolver::knapsack;
namespace po = boost::program_options;

BranchAndBoundPrimalDualOptionalParameters read_branch_and_bound_primal_dual_args(std::vector<char*> argv)
{
    BranchAndBoundPrimalDualOptionalParameters p;
    po::options_description desc("Allowed options");
    desc.add_options()
        ("greedy,g", "")
        ("greedy-nlogn,n", po::value<StateIdx>(&p.greedy_nlogn), "")
        ("surrogate-relaxation,s", po::value<StateIdx>(&p.surrogate_relaxation), "")
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
    if (vm.count("greedy"))
        p.greedy = true;
    if (vm.count("combo-core"))
        p.combo_core = true;
    return p;
}

DynamicProgrammingBalancingOptionalParameters read_dynamic_programming_balancing_args(std::vector<char*> argv)
{
    DynamicProgrammingBalancingOptionalParameters p;
    po::options_description desc("Allowed options");
    desc.add_options()
        ("upper-bound,u", po::value<char>(&p.ub), "")
        ("greedy,g", "")
        ("greedy-nlogn,n", po::value<StateIdx>(&p.greedy_nlogn), "")
        ("surrogate-relaxation,s", po::value<StateIdx>(&p.surrogate_relaxation), "")
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
    if (vm.count("greedy"))
        p.greedy = true;
    return p;
}

DynamicProgrammingPrimalDualOptionalParameters read_dynamic_programming_primal_dual_args(std::vector<char*> argv)
{
    DynamicProgrammingPrimalDualOptionalParameters p;
    po::options_description desc("Allowed options");
    desc.add_options()
        ("greedy,g", "")
        ("pairing,p", po::value<StateIdx>(&p.pairing), "")
        ("surrogate_relaxation,s", po::value<StateIdx>(&p.surrogate_relaxation), "")
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
    if (vm.count("greedy"))
        p.greedy = true;
    if (vm.count("combo-core"))
        p.combo_core = true;
    return p;
}

Output knapsacksolver::knapsack::run(
        std::string algorithm,
        Instance& instance,
        std::mt19937_64&,
        Info info)
{
    std::vector<std::string> algorithm_args = po::split_unix(algorithm);
    std::vector<char*> algorithm_argv;
    for(Counter i = 0; i < (Counter)algorithm_args.size(); ++i)
        algorithm_argv.push_back(const_cast<char*>(algorithm_args[i].c_str()));

    if (algorithm.empty() || algorithm_args[0].empty()) {
        throw std::invalid_argument("Missing algorithm.");

    } else if (algorithm_args[0] == "greedy") {
        instance.sort_partially(FFOT_DBG(info));
        return greedy(instance, info);
    } else if (algorithm_args[0] == "greedy-nlogn") {
        instance.sort_partially(FFOT_DBG(info));
        return greedy_nlogn(instance, info);
    } else if (algorithm_args[0] == "greedy-nlogn-forward") {
        instance.sort_partially(FFOT_DBG(info));
        return greedy_nlogn_forward(instance, info);
    } else if (algorithm_args[0] == "greedy-nlogn-backward") {
        instance.sort_partially(FFOT_DBG(info));
        return greedy_nlogn_backward(instance, info);

    } else if (algorithm_args[0] == "dynamic-programming-bellman-array") { // DP Bellman
        return dynamic_programming_bellman_array(instance, info);
    } else if (algorithm_args[0] == "dynamic-programming-bellman-array-parallel") {
        return dynamic_programming_bellman_array_parallel(instance, info);
    } else if (algorithm_args[0] == "dynamic-programming-bellman-rec") {
        return dynamic_programming_bellman_rec(instance, info);
    } else if (algorithm_args[0] == "dynamic-programming-bellman-array-all") {
        return dynamic_programming_bellman_array_all(instance, info);
    } else if (algorithm_args[0] == "dynamic-programming-bellman-array-one") {
        return dynamic_programming_bellman_array_one(instance, info);
    } else if (algorithm_args[0] == "dynamic-programming-bellman-array-part") {
        return dynamic_programming_bellman_array_part(instance, 64, info);
    } else if (algorithm_args[0] == "dynamic-programming-bellman-array-rec") {
        return dynamic_programming_bellman_array_rec(instance, info);
    } else if (algorithm_args[0] == "dynamic-programming-bellman-list") {
        return dynamic_programming_bellman_list(instance, false, info);
    } else if (algorithm_args[0] == "dynamic-programming-bellman-list-sort") {
        return dynamic_programming_bellman_list(instance, true, info);
    } else if (algorithm_args[0] == "dynamic-programming-bellman-list-rec") {
        return dynamic_programming_bellman_list_rec(instance, info);

    } else if (algorithm_args[0] == "dynamic-programming-profits-array") { // DP by profits
        return dynamic_programming_profits_array(instance, info);
    } else if (algorithm_args[0] == "dynamic-programming-profits-array-all") {
        return dynamic_programming_profits_array_all(instance, info);

    } else if (algorithm_args[0] == "dynamic-programming-balancing"
            || algorithm_args[0] == "balknap") { // DP balancing
        DynamicProgrammingBalancingOptionalParameters p
            = read_dynamic_programming_balancing_args(algorithm_argv);
        p.info = info;
        return dynamic_programming_balancing(instance, p);
    } else if (algorithm_args[0] == "dynamic-programming-balancing-combo"
            || algorithm_args[0] == "balknap-combo") {
        auto p = DynamicProgrammingBalancingOptionalParameters().set_combo();
        p.info = info;
        return dynamic_programming_balancing(instance, p);

    } else if (algorithm_args[0] == "dynamic-programming-primal-dual"
            || algorithm_args[0] == "minknap") { // DP primal-dual
        DynamicProgrammingPrimalDualOptionalParameters p
            = read_dynamic_programming_primal_dual_args(algorithm_argv);
        p.info = info;
        return dynamic_programming_primal_dual(instance, p);
    } else if (algorithm_args[0] == "dynamic-programming-primal-dual-combo"
            || algorithm_args[0] == "combo") {
        auto p = DynamicProgrammingPrimalDualOptionalParameters().set_combo();
        p.info = info;
        return dynamic_programming_primal_dual(instance, p);

    } else if (algorithm_args[0] == "branch-and-bound") { // Branch-and-bound
        return branch_and_bound(instance, false, info);
    } else if (algorithm_args[0] == "branch-and-bound-sort") {
        return branch_and_bound(instance, true, info);

    } else if (algorithm_args[0] == "branch-and-bound-primal-dual") { // BranchAndBoundPrimalDual
        BranchAndBoundPrimalDualOptionalParameters p
            = read_branch_and_bound_primal_dual_args(algorithm_argv);
        p.info = info;
        return branch_and_bound_primal_dual(instance, p);
    } else if (algorithm_args[0] == "branch-and-bound-primal-dual-combo") {
        auto p = BranchAndBoundPrimalDualOptionalParameters().set_combo();
        p.info = info;
        return branch_and_bound_primal_dual(instance, p);

    } else if (algorithm_args[0] == "dantzig") { // Dantzig
        Info info_tmp;
        Output output(instance, info_tmp);
        instance.sort_partially(FFOT_DBG(info_tmp));
        output.upper_bound = upper_bound_dantzig(instance, info);
        return output;
    } else if (algorithm_args[0] == "surrogate-relaxation") { // Surrogate relaxation
        return surrogate_relaxation(instance, info);
    } else if (algorithm_args[0] == "solve-surrogate-instance") {
        return solve_surrogate_instance(instance, info);

    } else {
        throw std::invalid_argument(
                "Unknown algorithm \"" + algorithm_args[0] + "\".");
    }
}

