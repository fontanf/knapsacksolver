#include "knapsack/lib/generator.hpp"

#include "knapsack/lib/algorithms.hpp"

#include <iomanip>
#include <sstream>

#include <boost/program_options.hpp>

using namespace knapsack;

void bench(
        std::string algorithm,
        std::string dataset_name,
        std::vector<std::pair<std::string, Generator>>& dataset,
        std::mt19937_64& gen,
        ItemIdx n_max = 10000,
        bool verbose = false)
{
    std::ofstream file(algorithm + "_" + dataset_name + ".csv");
    func f = get_algorithm(algorithm);
    std::vector<int8_t> ok(dataset.size(), 1);

    file << "n \\ ds";
    for (auto& p: dataset)
        file << "," << p.first;
    file << std::endl;

    for (ItemIdx n: {50,
            100, 200, 500,
            1000, 2000, 5000,
            10000, 20000, 50000,
            100000, 200000, 500000,
            1000000}) {
        if (n > n_max)
            break;
        file << n << std::flush;
        for (Cpt k=0; k<(Cpt)dataset.size(); ++k) {
            Generator& d = dataset[k].second;
            d.n = n;
            std::cout << d << "..." << std::flush;

            double t_max = 300;
            double t_total = 0.0;
            double mean = -1;
            if (ok[k]) {
                for (d.h=1; d.h<=d.hmax && ok[k]; ++d.h) {
                    d.s = d.n + d.r + d.h;
                    if (verbose)
                        std::cout << std::endl << d << std::endl;
                    Instance ins = d.generate();
                    Solution sol(ins);
                    Profit ub = INT_MAX;
                    Info info = Info()
                        .set_timelimit(t_max - t_total)
                        .set_verbose(verbose)
                        ;
                    try {
                        f(ins, sol, ub, gen, info);
                        double t = info.elapsed_time();
                        t_total += t;
                        if (t_total > t_max) {
                            ok[k] = 0;
                            mean = -1;
                        }
                    } catch (...) {
                        std::cout << " x" << std::endl;
                        std::cout << std::endl << d << std::flush;
                        Instance ins = d.generate();
                        Info info = Info()
                            .set_timelimit(t_max - t_total)
                            .set_verbose(true)
                            .set_log2stderr(true)
                            ;
                        f(ins, sol, ub, gen, info);
                        exit(1);
                        mean = -1;
                    }
                }
                d.h = -1;
                d.s = -1;

                mean = round(t_total * 100) / 10;
                if (verbose) {
                    std::cout << std::endl;
                } else {
                    std::cout << " ";
                }
            }

            if (ok[k]) {
                std::cout << "mean " << mean << std::endl;
                file << "," << mean << std::flush;
            } else {
                std::cout << "mean > " << t_max * 10 << std::endl;
                file << ",x" << std::flush;
            }
        }
        file << std::endl;
    }
    std::cout << std::endl;
    file << std::endl;
    file.close();
}

void bench_normal(std::string algorithm, double time_limit, std::mt19937_64& gen)
{
    func f = get_algorithm(algorithm);

    std::vector<ItemIdx> ns {100, 1000, 10000, 100000, 1000000};
    std::vector<Weight> rs {1000, 10000, 100000, 1000000, 10000000, 100000000};
    std::vector<double> xs {0.0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9};

    nlohmann::json json;
    json["lab"][0] = {"n", ns};
    json["lab"][1] = {"r", rs};
    json["lab"][2] = {"x", xs};

    int val_max_r = 255 - 52;
    int val_max_g = 255 - 101;
    int val_max_b = 255 - 164;

    Generator d;
    d.normal = true;
    d.t = "normal";
    d.dw = 10;
    d.d = 10;
    d.s = 0;

    for (Cpt in = 0; in < (Cpt)ns.size(); ++in) {
        ItemIdx n = ns[in];
        d.n = n;
        for (Cpt ir = 0; ir < (Cpt)rs.size(); ++ir) {
            Weight r = rs[ir];
            d.r = r;

            // Standard output
            std::string s = "--- n " + std::to_string(n) + " r " + std::to_string(r) + " --- ";
            int pos = (int)((80 - s.length())/2);
            for(int i=0; i<pos; i++)
                std::cout << " ";
            std::cout << s << std::endl;

            for (Cpt ix = 0; ix < (Cpt)xs.size(); ++ix) {
                double x = xs[ix];
                d.x = x;
                d.s++;

                // Standard output
                std::cout << "x " << std::right << std::setw(6) << x << std::flush;

                Instance ins = d.generate();
                Solution sol(ins);
                Profit ub = LLONG_MAX;
                double t = time_limit + 1;
                try {
                    Info info = Info()
                        .set_timelimit(time_limit)
                        //.set_verbose(true)
                        ;
                    f(ins, sol, ub, gen, info);
                    t = info.elapsed_time();
                } catch (...) {
                }

                std::stringstream t_str;
                if (t <= time_limit && sol.profit() == ub) {
                    t_str << (double)std::round(t * 10000) / 10;
                    std::cout << "\033[32m";
                } else {
                    t_str << "> " << time_limit * 1000;
                }

                // Json
                int col_r = (t > time_limit)? 0: 255 - (int)(val_max_r * cbrt(t / time_limit));
                int col_g = (t > time_limit)? 0: 255 - (int)(val_max_g * cbrt(t / time_limit));
                int col_b = (t > time_limit)? 0: 255 - (int)(val_max_b * cbrt(t / time_limit));
                std::string rgb_str = "rgb("
                    + std::to_string(col_r) + ","
                    + std::to_string(col_g) + ","
                    + std::to_string(col_b) + ")";
                json["tab"][in][ir][ix][0]["c"] = rgb_str;
                json["tab"][in][ir][ix][0]["t"] = t_str.str();

                // Standard output
                std::cout << " | LB" << std::right << std::setw(20) << sol.profit();
                std::cout << " | UB" << std::right << std::setw(20) << ub;
                std::cout << " | T (s)" << std::right << std::setw(8) << t_str.str();
                std::cout << "\033[0m" << std::endl;
            }
        }
    }

    std::ofstream o(algorithm + ".json");
    o << std::setw(4) << json << std::endl;
}

int main(int argc, char *argv[])
{
    namespace po = boost::program_options;

    // Parse program options
    std::vector<std::string> algorithms;
    std::vector<std::string> datasets;
    double time_limit = 1;

    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "produce help message")
        ("algorithms,a", po::value<std::vector<std::string>>(&algorithms)->multitoken(), "set algorithms")
        ("datasets,d", po::value<std::vector<std::string>>(&datasets)->multitoken(), "datasets (easy, difficultlarge, difficultsmall)")
        ("time-limit,t", po::value<double>(&time_limit), "time limit")
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

    Seed seed = 0;
    std::mt19937_64 gen(seed);

    std::vector<std::pair<std::string, Generator>> dataset_easy {
            {"u3", Generator::classical_generator("u", 1000)},
            {"u4", Generator::classical_generator("u", 10000)},
            {"wc3", Generator::classical_generator("wc", 1000)},
            {"wc4", Generator::classical_generator("wc", 10000)},
            {"sc3", Generator::classical_generator("sc", 1000)},
            {"sc4", Generator::classical_generator("sc", 10000)},
            {"isc3", Generator::classical_generator("isc", 1000)},
            {"isc4", Generator::classical_generator("isc", 10000)},
            {"asc3", Generator::classical_generator("asc", 1000)},
            {"asc4", Generator::classical_generator("asc", 10000)},
            {"ss3", Generator::classical_generator("ss", 1000)},
            {"ss4", Generator::classical_generator("ss", 10000)},
            {"sw5", Generator::classical_generator("sw", 100000)},
    };

    std::vector<std::pair<std::string, Generator>> dataset_difficultlarge {
            {"u5", Generator::classical_generator("u", 100000)},
            {"u6", Generator::classical_generator("u", 1000000)},
            {"u7", Generator::classical_generator("u", 10000000)},
            {"wc5", Generator::classical_generator("wc", 100000)},
            {"wc6", Generator::classical_generator("wc", 1000000)},
            {"wc7", Generator::classical_generator("wc", 10000000)},
            {"sc5", Generator::classical_generator("sc", 100000)},
            {"sc6", Generator::classical_generator("sc", 1000000)},
            {"sc7", Generator::classical_generator("sc", 10000000)},
            {"isc5", Generator::classical_generator("isc", 100000)},
            {"isc6", Generator::classical_generator("isc", 1000000)},
            {"isc7", Generator::classical_generator("isc", 10000000)},
            {"asc5", Generator::classical_generator("asc", 100000)},
            {"asc6", Generator::classical_generator("asc", 1000000)},
            {"asc7", Generator::classical_generator("asc", 10000000)},
            {"ss5", Generator::classical_generator("ss", 100000)},
            {"ss6", Generator::classical_generator("ss", 1000000)},
            {"ss7", Generator::classical_generator("ss", 10000000)},
            {"sw6", Generator::classical_generator("sw", 1000000)},
            {"sw7", Generator::classical_generator("sw", 10000000)},
            {"sw8", Generator::classical_generator("sw", 100000000)},
    };

    std::vector<std::pair<std::string, Generator>> dataset_difficultsmall {
            {"sp/u3", Generator::spanner_generator("u", 1000, 2, 10)},
            {"sp/wc3", Generator::spanner_generator("wc", 1000, 2, 10)},
            {"sp/sc3", Generator::spanner_generator("sc", 1000, 2, 10)},
            {"mstr3", Generator::mstr_generator(1000, 300, 200, 6)},
            {"pceil3", Generator::pceil_generator(1000, 3)},
            {"circle3", Generator::circle_generator(1000, 2.0/3)},
    };

    bool verbose = vm.count("verbose");
    for (std::string algorithm: algorithms) {
        std::cout << "*** " << algorithm << " ***" << std::endl;
        for (std::string dataset: datasets) {
            if (dataset == "easy" || dataset == "literature") {
                bench(algorithm, "easy", dataset_easy, gen, 10000, verbose);
            } else if (dataset == "difficultsmall" || dataset == "literature") {
                bench(algorithm, "difficultsmall", dataset_difficultsmall, gen, 10000, verbose);
            } else if (dataset == "difficultlarge" || dataset == "literature") {
                bench(algorithm, "difficultlarge", dataset_difficultlarge, gen, 10000, verbose);
            } else if (dataset == "normal") {
                bench_normal(algorithm, time_limit, gen);
            }
        }
    }

    return 0;
}

