#include "knapsack/lib/generator.hpp"
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

std::function<Profit (Instance&, Info)> get_algorithm(std::string s)
{
    if (s == "bellman_array") { // bellman
        return opt_bellman_array;
    } else if (s == "bellmanpar_array") {
        return opt_bellmanpar_array;
    } else if (s == "bellmanrec") {
        return [](Instance& ins, Info info) { return sopt_bellmanrec(ins, info).profit(); };
    } else if (s == "bellman_array_all") {
        return [](Instance& ins, Info info) { return sopt_bellman_array_all(ins, info).profit(); };
    } else if (s == "bellman_array_one") {
        return [](Instance& ins, Info info) { return sopt_bellman_array_one(ins, info).profit(); };
    } else if (s == "bellman_array_part") {
        return [](Instance& ins, Info info) { return sopt_bellman_array_part(ins, 64, info).profit(); };
    } else if (s == "bellman_array_rec") {
        return [](Instance& ins, Info info) { return sopt_bellman_array_rec(ins, info).profit(); };
    } else if (s == "bellman_list") {
        return opt_bellman_list;
    } else if (s == "bellman_list_rec") {
        return [](Instance& ins, Info info) { return sopt_bellman_list_rec(ins, info).profit(); };
    } else if (s == "dpprofits_array") { // dpprofits
        return opt_dpprofits_array;
    } else if (s == "dpprofits_array_all") {
        return [](Instance& ins, Info info) { return sopt_dpprofits_array_all(ins, info).profit(); };
    } else if (s == "bab") { // bab
        return [](Instance& ins, Info info) { return sopt_bab(ins, info).profit(); };
    } else if (s == "astar") { // astar
        return [](Instance& ins, Info info) { return sopt_astar(ins, info).profit(); };
    } else if (s == "expknap_fontan") { // expknap
        return [](Instance& ins, Info info) { return Expknap(ins, ExpknapParams::fontan()).run(info).profit(); };
    /*
    } else if (s == "balknap") { // balknap
        BalknapParams p;
        sopt_balknap(ins, p, k, info);
    } else if (s == "minknap") { // minknap
        MinknapParams p;
        p.k = k;
        Minknap(ins, p).run(info);
    */
    } else if (s == "greedy") { // greedy
        return [](Instance& ins, Info info) {
            ins.sort_partially(info);
            return sol_greedy(ins, info).profit();
        };
    } else if (s == "greedynlogn") { // greedynlogn
        return [](Instance& ins, Info info) { return sol_greedynlogn(ins, info).profit(); };
    } else if (s == "greedynlogn_for") {
        return [](Instance& ins, Info info) { return sol_forwardgreedynlogn(ins, info).profit(); };
    } else if (s == "greedynlogn_back") {
        return [](Instance& ins, Info info) { return sol_backwardgreedynlogn(ins, info).profit(); };
    } else if (s == "dantzig") { // dantzig
        return [](Instance& ins, Info info) { return ub_dantzig(ins, info); };
    } else if (s == "surrelax") { // surrelax
        return [](Instance& ins, Info info) {
            ins.sort_partially(info);
            Solution sol = sol_greedynlogn(ins, Info(info, false, "greedynlogn"));
            return ub_surrogate(ins, sol.profit(), info).ub;
        };
    } else {
        return opt_bellman_array;
    }
}

template<typename Function>
double bench(Function func, GenerateData d)
{
    double t_max = 300;
    double t_total = 0.0;
    std::cout << d << std::flush;
    for (d.h=1; d.h<=100; ++d.h) {
        d.s = d.n + d.r + d.h;
        //std::cout << std::endl << "h " << d.h << " s " << d.s;
        Instance ins = generate(d);
        Info info = Info().set_timelimit(t_max - t_total);
        try {
            func(ins, info);
        } catch (...) {
            std::cout << " x" << std::endl;
            return -1;
        }
        double t = info.elapsed_time();
        t_total += t;
        if (t_total > t_max) {
            std::cout << " mean > " << t_max*10 << std::endl;
            return -1;
        }
    }
    d.h = -1;

    double mean = round(t_total * 100) / 10;
    std::cout << " mean " << mean << std::endl;
    return mean;
}

void bench_easy(std::string algorithm)
{
    std::ofstream file(algorithm + "_easy.csv");
    std::function<Profit (Instance&, Info)> func = get_algorithm(algorithm);
    GenerateData d;
    std::vector<std::pair<std::string, Profit>> vec {
        {"u",   1000}, {"u",   10000},
        {"wc",  1000}, {"wc",  10000},
        {"sc",  1000}, {"sc",  10000},
        {"isc", 1000}, {"isc", 10000},
        {"asc", 1000}, {"asc", 10000},
        {"ss",  1000}, {"ss",  10000},
        {"sw", 100000}};

    file << "n \\ Type R,U 10^3,U 10^4,WC 10^3,WC 10^4,SC 10^3,SC 10^4,ISC 10^3,ISC 10^4,ASC 10^3,ASC 10^4,SS 10^3,SS 10^4,SW 10^5" << std::endl;
    for (ItemIdx n: {50, 100, 200, 500, 1000, 2000, 5000, 10000}) {
    //for (ItemIdx n: {1, 2, 5, 10, 20, 50, 100, 200}) {
        d.n = n;
        file << n << std::flush;
        for (auto p: vec) {
            d.t = p.first;
            d.r = p.second;
            double res = bench(func, d);
            if (res < 0) {
                file << ",x" << std::flush;
            } else {
                file << "," << res << std::flush;
            }
        }
        file << std::endl;
    }
    std::cout << std::endl;
    file << std::endl;
    file.close();
}

void bench_difficult_large(std::string algorithm)
{
    std::ofstream file(algorithm + "_difficult_large.csv");
    std::function<Profit (Instance&, Info)> func = get_algorithm(algorithm);
    GenerateData d;
    std::vector<std::pair<std::string, Profit>> vec {
        {"u",   100000}, {"u",   1000000}, {"u",   10000000},
        {"wc",  100000}, {"wc",  1000000}, {"wc",  10000000},
        {"sc",  100000}, {"sc",  1000000}, {"sc",  10000000},
        {"isc", 100000}, {"isc", 1000000}, {"isc", 10000000},
        {"asc", 100000}, {"asc", 1000000}, {"asc", 10000000},
        {"ss",  100000}, {"ss",  1000000}, {"ss",  10000000},
                                           {"sw",  10000000}, {"sw", 100000000}};

    file << "n \\ Type R"
        ",U 10^5,U 10^6,U 10^7"
        ",WC 10^5,WC 10^6,WC 10^7"
        ",SC 10^5,SC 10^6,SC 10^7"
        ",ISC 10^5,ISC 10^6,ISC 10^7"
        ",ASC 10^5,ASC 10^6,ASC 10^7"
        ",SS 10^5,SS 10^6,SS 10^7"
        ",SW 10^7,SW 10^8"
        << std::endl;
    for (ItemIdx n: {50, 100, 200, 500, 1000, 2000, 5000, 10000}) {
        d.n = n;
        file << n << std::flush;
        for (auto p: vec) {
            d.t = p.first;
            d.r = p.second;
            file << "," << bench(func, d) << std::flush;
        }
        file << std::endl;
    }
    std::cout << std::endl;
    file << std::endl;
    file.close();
}

void bench_difficult_small(std::string algorithm)
{
    std::ofstream file(algorithm + "_difficult_small.csv");
    std::function<Profit (Instance&, Info)> func = get_algorithm(algorithm);
    std::vector<GenerateData> vec {
        GenerateData::gen_spanner("u", 1000, 2, 10),
        GenerateData::gen_spanner("wc", 1000, 2, 10),
        GenerateData::gen_spanner("sc", 1000, 2, 10),
        GenerateData::gen_mstr(1000, 300, 200, 6),
        GenerateData::gen_pceil(1000, 3),
        GenerateData::gen_circle(1000, 2.0/3),
        };

    file << "n \\ Type"
        ",U SP 2 10,WC SP 2 10,SC SP 2 10"
        ",MSTR 3R/10 2R/10 6,PCEIL 3,CIRCLE 2/3"
        << std::endl;
    for (ItemIdx n: {50, 100, 200, 500, 1000, 2000, 5000, 10000}) {
        file << n << std::flush;
        for (GenerateData d: vec) {
            d.n = n;
            file << "," << bench(func, d) << std::flush;
        }
        file << std::endl;
    }
    std::cout << std::endl;
    file << std::endl;
    file.close();
}

int main(int argc, char *argv[])
{
    namespace po = boost::program_options;

    // Parse program options
    std::string algorithm = "bellman_array";

    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "produce help message")
        ("algorithm,a", po::value<std::string>(&algorithm), "set algorithm")
        ("easy", "")
        ("difficult-small", "")
        ("difficult-large", "")
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

    if (vm.count("easy"))
        bench_easy(algorithm);
    if (vm.count("difficult-small"))
        bench_difficult_small(algorithm);
    if (vm.count("difficult-large"))
        bench_difficult_large(algorithm);

    return 0;
}

