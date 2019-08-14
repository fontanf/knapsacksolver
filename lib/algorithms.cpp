#include "knapsack/lib/algorithms.hpp"

#include <map>

using namespace knapsack;

func knapsack::get_algorithm(std::string str)
{
    std::stringstream ss(str);
    std::istream_iterator<std::string> begin(ss);
    std::istream_iterator<std::string> end;
    std::vector<std::string> vstrings(begin, end);
    std::map<std::string, std::string> args;
    for (auto it=std::next(vstrings.begin());
            it!=vstrings.end() && std::next(it)!=vstrings.end();
            it=std::next(std::next(it)))
        args[*it] = *std::next(it);

    if (vstrings[0] == "") {
        std::cerr << "\033[32m" << "ERROR, missing algorithm." << "\033[0m" << std::endl;
        return [](Instance&, Solution&, Profit&, std::mt19937_64&, Info) { };

    /*
     * Lower bounds
     */
    } else if (vstrings[0] == "greedy") {
        return [](Instance& ins, Solution& sol, Profit&, std::mt19937_64&, Info info) {
            ins.sort_partially(info);
            sol = sol_greedy(ins, info);
        };
    } else if (vstrings[0] == "greedynlogn") {
        return [](Instance& ins, Solution& sol, Profit&, std::mt19937_64&, Info info) {
            ins.sort_partially(info);
            sol = sol_greedynlogn(ins, info);
        };
    } else if (vstrings[0] == "greedynlogn_for") {
        return [](Instance& ins, Solution& sol, Profit&, std::mt19937_64&, Info info) {
            ins.sort_partially(info);
            sol = sol_forwardgreedynlogn(ins, info);
        };
    } else if (vstrings[0] == "greedynlogn_back") {
        return [](Instance& ins, Solution& sol, Profit&, std::mt19937_64&, Info info) {
            ins.sort_partially(info);
            sol = sol_backwardgreedynlogn(ins, info);
        };

    /*
     * Exact algorithms
     */
    } else if (vstrings[0] == "bellman_array") { // Bellman
        return [](Instance& ins, Solution&, Profit& ub, std::mt19937_64&, Info info) {
            Profit p = opt_bellman_array(ins, info);
            if (info.check_time())
                ub = p;
        };
    } else if (vstrings[0] == "bellmanpar_array") {
        return [](Instance& ins, Solution&, Profit& ub, std::mt19937_64&, Info info) {
            Profit p = opt_bellmanpar_array(ins, info);
            if (info.check_time())
                ub = p;
        };
    } else if (vstrings[0] == "bellman_rec") {
        return [](Instance& ins, Solution& sol, Profit& ub, std::mt19937_64&, Info info) {
            sol = sopt_bellmanrec(ins, info);
            if (info.check_time())
                ub = sol.profit();
        };
    } else if (vstrings[0] == "bellman_array_all") {
        return [](Instance& ins, Solution& sol, Profit& ub, std::mt19937_64&, Info info) {
            sol = sopt_bellman_array_all(ins, info);
            if (info.check_time())
                ub = sol.profit();
        };
    } else if (vstrings[0] == "bellman_array_one") {
        return [](Instance& ins, Solution& sol, Profit& ub, std::mt19937_64&, Info info) {
            sol = sopt_bellman_array_one(ins, info);
            if (info.check_time())
                ub = sol.profit();
        };
    } else if (vstrings[0] == "bellman_array_part") {
        return [](Instance& ins, Solution& sol, Profit& ub, std::mt19937_64&, Info info) {
            sol = sopt_bellman_array_part(ins, 64, info);
            if (info.check_time())
                ub = sol.profit();
        };
    } else if (vstrings[0] == "bellman_array_rec") {
        return [](Instance& ins, Solution& sol, Profit& ub, std::mt19937_64&, Info info) {
            sol = sopt_bellman_array_rec(ins, info);
            if (info.check_time())
                ub = sol.profit();
        };
    } else if (vstrings[0] == "bellman_list") {
        return [](Instance& ins, Solution&, Profit& ub, std::mt19937_64&, Info info) {
            Profit p = opt_bellman_list(ins, false, info);
            if (info.check_time())
                ub = p;
        };
    } else if (vstrings[0] == "bellman_list_sort") {
        return [](Instance& ins, Solution&, Profit& ub, std::mt19937_64&, Info info) {
            Profit p = opt_bellman_list(ins, true, info);
            if (info.check_time())
                ub = p;
        };
    } else if (vstrings[0] == "bellman_list_rec") {
        return [](Instance& ins, Solution& sol, Profit& ub, std::mt19937_64&, Info info) {
            sol = sopt_bellman_list_rec(ins, info);
            if (info.check_time())
                ub = sol.profit();
        };
    } else if (vstrings[0] == "dpprofits_array") { // DPProfits
        return [](Instance& ins, Solution&, Profit& ub, std::mt19937_64&, Info info) {
            Profit p = opt_dpprofits_array(ins, info);
            if (info.check_time())
                ub = p;
        };
    } else if (vstrings[0] == "dpprofits_array_all") {
        return [](Instance& ins, Solution& sol, Profit& ub, std::mt19937_64&, Info info) {
            sol = sopt_dpprofits_array_all(ins, info);
            if (info.check_time())
                ub = sol.profit();
        };
    } else if (vstrings[0] == "bab") { // Branch-and-bound
        return [](Instance& ins, Solution& sol, Profit& ub, std::mt19937_64&, Info info) {
            sol = sopt_bab(ins, false, info);
            if (info.check_time())
                ub = sol.profit();
        };
    } else if (vstrings[0] == "bab_sort") {
        return [](Instance& ins, Solution& sol, Profit& ub, std::mt19937_64&, Info info) {
            sol = sopt_bab(ins, true, info);
            if (info.check_time())
                ub = sol.profit();
        };
    } else if (vstrings[0] == "astar") { // A*
        return [](Instance& ins, Solution& sol, Profit& ub, std::mt19937_64&, Info info) {
            sol = sopt_astar(ins, info);
            if (info.check_time())
                ub = sol.profit();
        };
    } else if (vstrings[0] == "expknap") { // Expknap
        return [args](Instance& ins, Solution& sol, Profit& ub, std::mt19937_64&, Info info) {
            sol = Expknap(ins, ExpknapParams().set_params(args)).run(info);
            if (info.check_time())
                ub = sol.profit();
        };
    } else if (vstrings[0] == "expknap_combo") {
        return [](Instance& ins, Solution& sol, Profit& ub, std::mt19937_64&, Info info) {
            sol = Expknap(ins, ExpknapParams::combo()).run(info);
            if (info.check_time())
                ub = sol.profit();
        };
    } else if (vstrings[0] == "balknap") { // Balknap
        return [args](Instance& ins, Solution& sol, Profit& ub, std::mt19937_64&, Info info) {
            sol = Balknap(ins, BalknapParams().set_params(args)).run(info);
            if (info.check_time())
                ub = sol.profit();
        };
    } else if (vstrings[0] == "balknap_combo") {
        return [](Instance& ins, Solution& sol, Profit& ub, std::mt19937_64&, Info info) {
            sol = Balknap(ins, BalknapParams::combo()).run(info);
            if (info.check_time())
                ub = sol.profit();
        };
    } else if (vstrings[0] == "minknap") { // Minknap
        return [args](Instance& ins, Solution& sol, Profit& ub, std::mt19937_64&, Info info) {
            sol = Minknap(ins, MinknapParams().set_params(args)).run(info);
            if (info.check_time())
                ub = sol.profit();
        };
    } else if (vstrings[0] == "minknap_combo" || vstrings[0] == "combo") {
        return [](Instance& ins, Solution& sol, Profit& ub, std::mt19937_64&, Info info) {
            sol = Minknap(ins, MinknapParams::combo()).run(info);
            if (info.check_time())
                ub = sol.profit();
        };

    /*
     * Upper bounds
     */
    } else if (vstrings[0] == "dantzig") { // Dantzig
        return [](Instance& ins, Solution&, Profit& ub, std::mt19937_64&, Info info) {
            ub = ub_dantzig(ins, info);
        };
    /*
    } else if (vstrings[0] == "surrelax") { // Surrogate relaxation
        return [](Instance& ins, Solution&, Profit& ub, std::mt19937_64&, Info info) {
            ins.sort_partially(info);
            Solution sol = sol_greedy(ins, Info(info, false, "greedy"));
            ub = ub_solvesurrelax(ins, sol.profit(), info).ub;
        };
    */


    } else {
        std::cerr << "\033[31m" << "ERROR, unknown algorithm: " << vstrings[0] << "\033[0m" << std::endl;
        assert(false);
        return [](Instance&, Solution&, Profit&, std::mt19937_64&, Info) { };
    }
}

