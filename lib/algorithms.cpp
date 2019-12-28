#include "knapsack/lib/algorithms.hpp"

#include <map>

using namespace knapsack;

func knapsack::get_algorithm(std::string argsrithm)
{
    std::stringstream ss(argsrithm);
    std::istream_iterator<std::string> begin(ss);
    std::istream_iterator<std::string> end;
    std::vector<std::string> argv(begin, end);

    if (argv[0] == "") {
        std::cerr << "\033[32m" << "ERROR, missing argsrithm." << "\033[0m" << std::endl;
        return [](Instance& ins, std::mt19937_64&, Info info) { return Output(ins, info); };

    /*
     * Lower bounds
     */
    } else if (argv[0] == "greedy") {
        return [](Instance& ins, std::mt19937_64&, Info info) {
            ins.sort_partially(info);
            return sol_greedy(ins, info);
        };
    } else if (argv[0] == "greedynlogn") {
        return [](Instance& ins, std::mt19937_64&, Info info) {
            ins.sort_partially(info);
            return sol_greedynlogn(ins, info);
        };
    } else if (argv[0] == "greedynlogn_for") {
        return [](Instance& ins, std::mt19937_64&, Info info) {
            ins.sort_partially(info);
            return sol_forwardgreedynlogn(ins, info);
        };
    } else if (argv[0] == "greedynlogn_back") {
        return [](Instance& ins, std::mt19937_64&, Info info) {
            ins.sort_partially(info);
            return sol_backwardgreedynlogn(ins, info);
        };

    /*
     * Exact argsrithms
     */
    } else if (argv[0] == "bellman_array") { // Bellman
        return [](Instance& ins, std::mt19937_64&, Info info) {
            return opt_bellman_array(ins, info);
        };
    } else if (argv[0] == "bellmanpar_array") {
        return [](Instance& ins, std::mt19937_64&, Info info) {
            return opt_bellmanpar_array(ins, info);
        };
    } else if (argv[0] == "bellman_rec") {
        return [](Instance& ins, std::mt19937_64&, Info info) {
            return sopt_bellmanrec(ins, info);
        };
    } else if (argv[0] == "bellman_array_all") {
        return [](Instance& ins, std::mt19937_64&, Info info) {
            return sopt_bellman_array_all(ins, info);
        };
    } else if (argv[0] == "bellman_array_one") {
        return [](Instance& ins, std::mt19937_64&, Info info) {
            return sopt_bellman_array_one(ins, info);
        };
    } else if (argv[0] == "bellman_array_part") {
        return [](Instance& ins, std::mt19937_64&, Info info) {
            return sopt_bellman_array_part(ins, 64, info);
        };
    } else if (argv[0] == "bellman_array_rec") {
        return [](Instance& ins, std::mt19937_64&, Info info) {
            return sopt_bellman_array_rec(ins, info);
        };
    } else if (argv[0] == "bellman_list") {
        return [](Instance& ins, std::mt19937_64&, Info info) {
            return opt_bellman_list(ins, false, info);
        };
    } else if (argv[0] == "bellman_list_sort") {
        return [](Instance& ins, std::mt19937_64&, Info info) {
            return opt_bellman_list(ins, true, info);
        };
    } else if (argv[0] == "bellman_list_rec") {
        return [](Instance& ins, std::mt19937_64&, Info info) {
            return sopt_bellman_list_rec(ins, info);
        };
    } else if (argv[0] == "dpprofits_array") { // DPProfits
        return [](Instance& ins, std::mt19937_64&, Info info) {
            return opt_dpprofits_array(ins, info);
        };
    } else if (argv[0] == "dpprofits_array_all") {
        return [](Instance& ins, std::mt19937_64&, Info info) {
            return sopt_dpprofits_array_all(ins, info);
        };
    } else if (argv[0] == "bab") { // Branch-and-bound
        return [](Instance& ins, std::mt19937_64&, Info info) {
            return sopt_bab(ins, false, info);
        };
    } else if (argv[0] == "bab_sort") {
        return [](Instance& ins, std::mt19937_64&, Info info) {
            return sopt_bab(ins, true, info);
        };
    } else if (argv[0] == "expknap") { // Expknap
        return [argv](Instance& ins, std::mt19937_64&, Info info) {
            ExpknapOptionalParameters p;
            p.info = info;
            p.set_params(argv);
            return sopt_expknap(ins, p);
        };
    } else if (argv[0] == "expknap_combo") {
        return [](Instance& ins, std::mt19937_64&, Info info) {
            ExpknapOptionalParameters p;
            p.info = info;
            p.combo();
            return sopt_expknap(ins, p);
        };
    } else if (argv[0] == "balknap") { // Balknap
        return [argv](Instance& ins, std::mt19937_64&, Info info) {
            BalknapOptionalParameters p;
            p.info = info;
            p.set_params(argv);
            return sopt_balknap(ins, p);
        };
    } else if (argv[0] == "balknap_combo") {
        return [](Instance& ins, std::mt19937_64&, Info info) {
            BalknapOptionalParameters p;
            p.info = info;
            p.combo();
            return sopt_balknap(ins, p);
        };
    } else if (argv[0] == "minknap") { // Minknap
        return [argv](Instance& ins, std::mt19937_64&, Info info) {
            MinknapOptionalParameters p;
            p.info = info;
            p.set_params(argv);
            return sopt_minknap(ins, p);
        };
    } else if (argv[0] == "minknap_combo" || argv[0] == "combo") {
        return [](Instance& ins, std::mt19937_64&, Info info) {
            MinknapOptionalParameters p;
            p.info = info;
            p.combo();
            return sopt_minknap(ins, p);
        };

    /*
     * Upper bounds
     */
    } else if (argv[0] == "dantzig") { // Dantzig
        return [](Instance& ins, std::mt19937_64&, Info info) {
            Output output(ins, info);
            output.upper_bound = ub_dantzig(ins, info);
            return output;
        };
    } else if (argv[0] == "surrelax") { // Surrogate relaxation
        return [](Instance& ins, std::mt19937_64&, Info info) {
            return ub_surrelax(ins, info);
        };
    } else if (argv[0] == "surrelax_minknap") { // Surrogate relaxation
        return [](Instance& ins, std::mt19937_64&, Info info) {
            return ub_surrelax_minknap(ins, info);
        };


    } else {
        std::cerr << "\033[31m" << "ERROR, unknown algorithm: " << argv[0] << "\033[0m" << std::endl;
        assert(false);
        return [](Instance& ins, std::mt19937_64&, Info info) { return Output(ins, info); };
    }
}

