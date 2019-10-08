#include "knapsack/lib/algorithms.hpp"

#include <map>

using namespace knapsack;

func knapsack::get_algorithm(std::string str)
{
    benchtools::Algorithm algo(str);

    if (algo.name == "") {
        std::cerr << "\033[32m" << "ERROR, missing algorithm." << "\033[0m" << std::endl;
        return [](Instance& ins, std::mt19937_64&, Info info) { return Output(ins, info); };

    /*
     * Lower bounds
     */
    } else if (algo.name == "greedy") {
        return [](Instance& ins, std::mt19937_64&, Info info) {
            ins.sort_partially(info);
            return sol_greedy(ins, info);
        };
    } else if (algo.name == "greedynlogn") {
        return [](Instance& ins, std::mt19937_64&, Info info) {
            ins.sort_partially(info);
            return sol_greedynlogn(ins, info);
        };
    } else if (algo.name == "greedynlogn_for") {
        return [](Instance& ins, std::mt19937_64&, Info info) {
            ins.sort_partially(info);
            return sol_forwardgreedynlogn(ins, info);
        };
    } else if (algo.name == "greedynlogn_back") {
        return [](Instance& ins, std::mt19937_64&, Info info) {
            ins.sort_partially(info);
            return sol_backwardgreedynlogn(ins, info);
        };

    /*
     * Exact algorithms
     */
    } else if (algo.name == "bellman_array") { // Bellman
        return [](Instance& ins, std::mt19937_64&, Info info) {
            return opt_bellman_array(ins, info);
        };
    } else if (algo.name == "bellmanpar_array") {
        return [](Instance& ins, std::mt19937_64&, Info info) {
            return opt_bellmanpar_array(ins, info);
        };
    } else if (algo.name == "bellman_rec") {
        return [](Instance& ins, std::mt19937_64&, Info info) {
            return sopt_bellmanrec(ins, info);
        };
    } else if (algo.name == "bellman_array_all") {
        return [](Instance& ins, std::mt19937_64&, Info info) {
            return sopt_bellman_array_all(ins, info);
        };
    } else if (algo.name == "bellman_array_one") {
        return [](Instance& ins, std::mt19937_64&, Info info) {
            return sopt_bellman_array_one(ins, info);
        };
    } else if (algo.name == "bellman_array_part") {
        return [](Instance& ins, std::mt19937_64&, Info info) {
            return sopt_bellman_array_part(ins, 64, info);
        };
    } else if (algo.name == "bellman_array_rec") {
        return [](Instance& ins, std::mt19937_64&, Info info) {
            return sopt_bellman_array_rec(ins, info);
        };
    } else if (algo.name == "bellman_list") {
        return [](Instance& ins, std::mt19937_64&, Info info) {
            return opt_bellman_list(ins, false, info);
        };
    } else if (algo.name == "bellman_list_sort") {
        return [](Instance& ins, std::mt19937_64&, Info info) {
            return opt_bellman_list(ins, true, info);
        };
    } else if (algo.name == "bellman_list_rec") {
        return [](Instance& ins, std::mt19937_64&, Info info) {
            return sopt_bellman_list_rec(ins, info);
        };
    } else if (algo.name == "dpprofits_array") { // DPProfits
        return [](Instance& ins, std::mt19937_64&, Info info) {
            return opt_dpprofits_array(ins, info);
        };
    } else if (algo.name == "dpprofits_array_all") {
        return [](Instance& ins, std::mt19937_64&, Info info) {
            return sopt_dpprofits_array_all(ins, info);
        };
    } else if (algo.name == "bab") { // Branch-and-bound
        return [](Instance& ins, std::mt19937_64&, Info info) {
            return sopt_bab(ins, false, info);
        };
    } else if (algo.name == "bab_sort") {
        return [](Instance& ins, std::mt19937_64&, Info info) {
            return sopt_bab(ins, true, info);
        };
    } else if (algo.name == "expknap") { // Expknap
        return [algo](Instance& ins, std::mt19937_64&, Info info) {
            ExpknapOptionalParameters p;
            p.info = info;
            p.set_params(algo.args);
            return sopt_expknap(ins, p);
        };
    } else if (algo.name == "expknap_combo") {
        return [](Instance& ins, std::mt19937_64&, Info info) {
            ExpknapOptionalParameters p;
            p.info = info;
            p.combo();
            return sopt_expknap(ins, p);
        };
    } else if (algo.name == "balknap") { // Balknap
        return [algo](Instance& ins, std::mt19937_64&, Info info) {
            BalknapOptionalParameters p;
            p.info = info;
            p.set_params(algo.args);
            return sopt_balknap(ins, p);
        };
    } else if (algo.name == "balknap_combo") {
        return [](Instance& ins, std::mt19937_64&, Info info) {
            BalknapOptionalParameters p;
            p.info = info;
            p.combo();
            return sopt_balknap(ins, p);
        };
    } else if (algo.name == "minknap") { // Minknap
        return [algo](Instance& ins, std::mt19937_64&, Info info) {
            MinknapOptionalParameters p;
            p.info = info;
            p.set_params(algo.args);
            return sopt_minknap(ins, p);
        };
    } else if (algo.name == "minknap_combo" || algo.name == "combo") {
        return [](Instance& ins, std::mt19937_64&, Info info) {
            MinknapOptionalParameters p;
            p.info = info;
            p.combo();
            return sopt_minknap(ins, p);
        };

    /*
     * Upper bounds
     */
    } else if (algo.name == "dantzig") { // Dantzig
        return [](Instance& ins, std::mt19937_64&, Info info) {
            Output output(ins, info);
            output.upper_bound = ub_dantzig(ins, info);
            return output;
        };
    } else if (algo.name == "surrelax") { // Surrogate relaxation
        return [](Instance& ins, std::mt19937_64&, Info info) {
            return ub_surrelax(ins, info);
        };
    } else if (algo.name == "surrelax_minknap") { // Surrogate relaxation
        return [](Instance& ins, std::mt19937_64&, Info info) {
            return ub_surrelax_minknap(ins, info);
        };


    } else {
        std::cerr << "\033[31m" << "ERROR, unknown algorithm: " << algo.name << "\033[0m" << std::endl;
        assert(false);
        return [](Instance& ins, std::mt19937_64&, Info info) { return Output(ins, info); };
    }
}

