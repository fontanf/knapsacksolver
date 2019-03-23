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

void opt_combo(Instance& ins, Info& info, double time_limit)
{
    auto p = MinknapParams::combo();
    p.time_limit = time_limit;
    Minknap(ins, p).run(info).profit();
}

void opt_expknap_fontan(Instance& ins, Info& info, double time_limit)
{
    ExpknapParams p = ExpknapParams::fontan();
    p.time_limit = time_limit;
    Expknap(ins, p).run(info);
}


template<typename Function>
std::string bench(Function func, GenerateData d)
{
    double t_max = 600;
    double t_total = 0.0;
    std::cout << d << std::flush;
    for (d.h=1; d.h<=100; ++d.h) {
        d.s = d.n + d.r + d.h;
        //std::cout << std::endl << "h " << d.h << " s " << d.s;
        Instance ins = generate(d);
        Info info;
        func(ins, info, t_max - t_total);
        double t = info.elapsed_time();
        t_total += t;
        if (t_total > t_max)
            return "-";
    }
    d.h = -1;

    double mean = round(t_total * 100) / 10;
    std::cout << " mean " << mean << std::endl;
    return std::to_string(mean);
}

void bench_easy(std::ostream& file)
{
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
            file << "," << bench(opt_expknap_fontan, d);
        }
        file << std::endl;
    }
    file << std::endl;
}

void bench_difficult_large(std::ostream& file)
{
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
            file << "," << bench(opt_expknap_fontan, d);
        }
        file << std::endl;
    }
    file << std::endl;
}

void bench_difficult_small(std::ostream& file)
{
    GenerateData d;
    std::vector<GenerateData> vec {
        };

    //file << "n \\ Type R,U 10^3,U 10^4,WC 10^3,WC 10^4,SC 10^3,SC 10^4,ISC 10^3,ISC 10^4,ASC 10^3,ASC 10^4,SS 10^3,SS 10^4,SW 10^5" << std::endl;
    for (ItemIdx n: {50, 100, 200, 500, 1000, 2000, 5000, 10000}) {
        d.n = n;
        file << n << std::flush;
        for (auto p: vec) {
            file << "," << bench(opt_expknap_fontan, d);
        }
        file << std::endl;
    }
    file << std::endl;
}

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    std::ofstream file("out.csv");
    bench_easy(file);
    bench_difficult_large(file);
    //bench_difficult_small(file);
    file.close();

    return 0;
}

