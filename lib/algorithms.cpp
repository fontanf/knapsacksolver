#include "knapsack/lib/algorithms.hpp"

#include <map>

using namespace knapsack;

func knapsack::get_algorithm(std::string str)
{
    benchtools::Algorithm algo(str);

    if (algo.name == "") {
        std::cerr << "\033[32m" << "ERROR, missing algorithm." << "\033[0m" << std::endl;
        return [](Instance&, Solution&, Profit&, std::mt19937_64&, Info) { };

    /*
     * Lower bounds
     */
    } else if (algo.name == "greedy") {
        return [](Instance& ins, Solution& sol, Profit&, std::mt19937_64&, Info info) {
            ins.sort_partially(info);
            sol = sol_greedy(ins, info);
        };
    } else if (algo.name == "greedynlogn") {
        return [](Instance& ins, Solution& sol, Profit&, std::mt19937_64&, Info info) {
            ins.sort_partially(info);
            sol = sol_greedynlogn(ins, info);
        };
    } else if (algo.name == "greedynlogn_for") {
        return [](Instance& ins, Solution& sol, Profit&, std::mt19937_64&, Info info) {
            ins.sort_partially(info);
            sol = sol_forwardgreedynlogn(ins, info);
        };
    } else if (algo.name == "greedynlogn_back") {
        return [](Instance& ins, Solution& sol, Profit&, std::mt19937_64&, Info info) {
            ins.sort_partially(info);
            sol = sol_backwardgreedynlogn(ins, info);
        };

    /*
     * Exact algorithms
     */
    } else if (algo.name == "bellman_array") { // Bellman
        return [](Instance& ins, Solution&, Profit& ub, std::mt19937_64&, Info info) {
            Profit p = opt_bellman_array(ins, info);
            if (info.check_time())
                ub = p;
        };
    } else if (algo.name == "bellmanpar_array") {
        return [](Instance& ins, Solution&, Profit& ub, std::mt19937_64&, Info info) {
            Profit p = opt_bellmanpar_array(ins, info);
            if (info.check_time())
                ub = p;
        };
    } else if (algo.name == "bellman_rec") {
        return [](Instance& ins, Solution& sol, Profit& ub, std::mt19937_64&, Info info) {
            sol = sopt_bellmanrec(ins, info);
            if (info.check_time())
                ub = sol.profit();
        };
    } else if (algo.name == "bellman_array_all") {
        return [](Instance& ins, Solution& sol, Profit& ub, std::mt19937_64&, Info info) {
            sol = sopt_bellman_array_all(ins, info);
            if (info.check_time())
                ub = sol.profit();
        };
    } else if (algo.name == "bellman_array_one") {
        return [](Instance& ins, Solution& sol, Profit& ub, std::mt19937_64&, Info info) {
            sol = sopt_bellman_array_one(ins, info);
            if (info.check_time())
                ub = sol.profit();
        };
    } else if (algo.name == "bellman_array_part") {
        return [](Instance& ins, Solution& sol, Profit& ub, std::mt19937_64&, Info info) {
            sol = sopt_bellman_array_part(ins, 64, info);
            if (info.check_time())
                ub = sol.profit();
        };
    } else if (algo.name == "bellman_array_rec") {
        return [](Instance& ins, Solution& sol, Profit& ub, std::mt19937_64&, Info info) {
            sol = sopt_bellman_array_rec(ins, info);
            if (info.check_time())
                ub = sol.profit();
        };
    } else if (algo.name == "bellman_list") {
        return [](Instance& ins, Solution&, Profit& ub, std::mt19937_64&, Info info) {
            Profit p = opt_bellman_list(ins, false, info);
            if (info.check_time())
                ub = p;
        };
    } else if (algo.name == "bellman_list_sort") {
        return [](Instance& ins, Solution&, Profit& ub, std::mt19937_64&, Info info) {
            Profit p = opt_bellman_list(ins, true, info);
            if (info.check_time())
                ub = p;
        };
    } else if (algo.name == "bellman_list_rec") {
        return [](Instance& ins, Solution& sol, Profit& ub, std::mt19937_64&, Info info) {
            sol = sopt_bellman_list_rec(ins, info);
            if (info.check_time())
                ub = sol.profit();
        };
    } else if (algo.name == "dpprofits_array") { // DPProfits
        return [](Instance& ins, Solution&, Profit& ub, std::mt19937_64&, Info info) {
            Profit p = opt_dpprofits_array(ins, info);
            if (info.check_time())
                ub = p;
        };
    } else if (algo.name == "dpprofits_array_all") {
        return [](Instance& ins, Solution& sol, Profit& ub, std::mt19937_64&, Info info) {
            sol = sopt_dpprofits_array_all(ins, info);
            if (info.check_time())
                ub = sol.profit();
        };
    } else if (algo.name == "bab") { // Branch-and-bound
        return [](Instance& ins, Solution& sol, Profit& ub, std::mt19937_64&, Info info) {
            sol = sopt_bab(ins, false, info);
            if (info.check_time())
                ub = sol.profit();
        };
    } else if (algo.name == "bab_sort") {
        return [](Instance& ins, Solution& sol, Profit& ub, std::mt19937_64&, Info info) {
            sol = sopt_bab(ins, true, info);
            if (info.check_time())
                ub = sol.profit();
        };
    } else if (algo.name == "astar") { // A*
        return [](Instance& ins, Solution& sol, Profit& ub, std::mt19937_64&, Info info) {
            sol = sopt_astar(ins, info);
            if (info.check_time())
                ub = sol.profit();
        };
    } else if (algo.name == "expknap") { // Expknap
        return [algo](Instance& ins, Solution& sol, Profit& ub, std::mt19937_64&, Info info) {
            sol = sopt_expknap(ins, ExpknapParams().set_params(algo.args), info);
            if (info.check_time())
                ub = sol.profit();
        };
    } else if (algo.name == "expknap_combo") {
        return [](Instance& ins, Solution& sol, Profit& ub, std::mt19937_64&, Info info) {
            sol = sopt_expknap(ins, ExpknapParams::combo(), info);
            if (info.check_time())
                ub = sol.profit();
        };
    } else if (algo.name == "balknap") { // Balknap
        return [algo](Instance& ins, Solution& sol, Profit& ub, std::mt19937_64&, Info info) {
            sol = sopt_balknap(ins, BalknapParams().set_params(algo.args), info);
            if (info.check_time())
                ub = sol.profit();
        };
    } else if (algo.name == "balknap_combo") {
        return [](Instance& ins, Solution& sol, Profit& ub, std::mt19937_64&, Info info) {
            sol = sopt_balknap(ins, BalknapParams::combo(), info);
            if (info.check_time())
                ub = sol.profit();
        };
    } else if (algo.name == "minknap") { // Minknap
        return [algo](Instance& ins, Solution& sol, Profit& ub, std::mt19937_64&, Info info) {
            sol = sopt_minknap(ins, MinknapParams().set_params(algo.args), info);
            if (info.check_time())
                ub = sol.profit();
        };
    } else if (algo.name == "minknap_combo" || algo.name == "combo") {
        return [](Instance& ins, Solution& sol, Profit& ub, std::mt19937_64&, Info info) {
            sol = sopt_minknap(ins, MinknapParams::combo(), info);
            if (info.check_time())
                ub = sol.profit();
        };

    /*
     * Upper bounds
     */
    } else if (algo.name == "dantzig") { // Dantzig
        return [](Instance& ins, Solution&, Profit& ub, std::mt19937_64&, Info info) {
            ub = ub_dantzig(ins, info);
        };
    /*
    } else if (algo.name == "surrelax") { // Surrogate relaxation
        return [](Instance& ins, Solution&, Profit& ub, std::mt19937_64&, Info info) {
            ins.sort_partially(info);
            Solution sol = sol_greedy(ins, Info(info, false, "greedy"));
            ub = ub_solvesurrelax(ins, sol.profit(), info).ub;
        };
    */


    } else {
        std::cerr << "\033[31m" << "ERROR, unknown algorithm: " << algo.name << "\033[0m" << std::endl;
        assert(false);
        return [](Instance&, Solution&, Profit&, std::mt19937_64&, Info) { };
    }
}

