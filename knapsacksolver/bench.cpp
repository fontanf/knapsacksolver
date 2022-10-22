#include "knapsacksolver/generator.hpp"

#include "knapsacksolver/algorithms/algorithms.hpp"

#include <iomanip>
#include <sstream>

#include <boost/program_options.hpp>

using namespace knapsacksolver;

std::string path(std::string d, ItemPos n)
{
    if (d == "u3") {
        return "data/knapsack/smallcoeff/"
            "knapPI_1_" + std::to_string(n) + "_1000/"
            "knapPI_1_" + std::to_string(n) + "_1000_";
    } else if (d == "u4") {
        return "data/knapsack/smallcoeff/"
            "knapPI_1_" + std::to_string(n) + "_10000/"
            "knapPI_1_" + std::to_string(n) + "_10000_";
    } else if (d == "u5") {
        return "data/knapsack/largecoeff/"
            "knapPI_1_" + std::to_string(n) + "_100000/"
            "knapPI_1_" + std::to_string(n) + "_100000_";
    } else if (d == "u6") {
        return "data/knapsack/largecoeff/"
            "knapPI_1_" + std::to_string(n) + "_1000000/"
            "knapPI_1_" + std::to_string(n) + "_1000000_";
    } else if (d == "u7") {
        return "data/knapsack/largecoeff/"
            "knapPI_1_" + std::to_string(n) + "_10000000/"
            "knapPI_1_" + std::to_string(n) + "_10000000_";

    } else if (d == "wc3") {
        return "data/knapsack/smallcoeff/"
            "knapPI_2_" + std::to_string(n) + "_1000/"
            "knapPI_2_" + std::to_string(n) + "_1000_";
    } else if (d == "wc4") {
        return "data/knapsack/smallcoeff/"
            "knapPI_2_" + std::to_string(n) + "_10000/"
            "knapPI_2_" + std::to_string(n) + "_10000_";
    } else if (d == "wc5") {
        return "data/knapsack/largecoeff/"
            "knapPI_2_" + std::to_string(n) + "_100000/"
            "knapPI_2_" + std::to_string(n) + "_100000_";
    } else if (d == "wc6") {
        return "data/knapsack/largecoeff/"
            "knapPI_2_" + std::to_string(n) + "_1000000/"
            "knapPI_2_" + std::to_string(n) + "_1000000_";
    } else if (d == "wc7") {
        return "data/knapsack/largecoeff/"
            "knapPI_2_" + std::to_string(n) + "_10000000/"
            "knapPI_2_" + std::to_string(n) + "_10000000_";

    } else if (d == "sc3") {
        return "data/knapsack/smallcoeff/"
            "knapPI_3_" + std::to_string(n) + "_1000/"
            "knapPI_3_" + std::to_string(n) + "_1000_";
    } else if (d == "sc4") {
        return "data/knapsack/smallcoeff/"
            "knapPI_3_" + std::to_string(n) + "_10000/"
            "knapPI_3_" + std::to_string(n) + "_10000_";
    } else if (d == "sc5") {
        return "data/knapsack/largecoeff/"
            "knapPI_3_" + std::to_string(n) + "_100000/"
            "knapPI_3_" + std::to_string(n) + "_100000_";
    } else if (d == "sc6") {
        return "data/knapsack/largecoeff/"
            "knapPI_3_" + std::to_string(n) + "_1000000/"
            "knapPI_3_" + std::to_string(n) + "_1000000_";
    } else if (d == "sc7") {
        return "data/knapsack/largecoeff/"
            "knapPI_3_" + std::to_string(n) + "_10000000/"
            "knapPI_3_" + std::to_string(n) + "_10000000_";

    } else if (d == "isc3") {
        return "data/knapsack/smallcoeff/"
            "knapPI_4_" + std::to_string(n) + "_1000/"
            "knapPI_4_" + std::to_string(n) + "_1000_";
    } else if (d == "isc4") {
        return "data/knapsack/smallcoeff/"
            "knapPI_4_" + std::to_string(n) + "_10000/"
            "knapPI_4_" + std::to_string(n) + "_10000_";
    } else if (d == "isc5") {
        return "data/knapsack/largecoeff/"
            "knapPI_4_" + std::to_string(n) + "_100000/"
            "knapPI_4_" + std::to_string(n) + "_100000_";
    } else if (d == "isc6") {
        return "data/knapsack/largecoeff/"
            "knapPI_4_" + std::to_string(n) + "_1000000/"
            "knapPI_4_" + std::to_string(n) + "_1000000_";
    } else if (d == "isc7") {
        return "data/knapsack/largecoeff/"
            "knapPI_4_" + std::to_string(n) + "_10000000/"
            "knapPI_4_" + std::to_string(n) + "_10000000_";

    } else if (d == "asc3") {
        return "data/knapsack/smallcoeff/"
            "knapPI_5_" + std::to_string(n) + "_1000/"
            "knapPI_5_" + std::to_string(n) + "_1000_";
    } else if (d == "asc4") {
        return "data/knapsack/smallcoeff/"
            "knapPI_5_" + std::to_string(n) + "_10000/"
            "knapPI_5_" + std::to_string(n) + "_10000_";
    } else if (d == "asc5") {
        return "data/knapsack/largecoeff/"
            "knapPI_5_" + std::to_string(n) + "_100000/"
            "knapPI_5_" + std::to_string(n) + "_100000_";
    } else if (d == "asc6") {
        return "data/knapsack/largecoeff/"
            "knapPI_5_" + std::to_string(n) + "_1000000/"
            "knapPI_5_" + std::to_string(n) + "_1000000_";
    } else if (d == "asc7") {
        return "data/knapsack/largecoeff/"
            "knapPI_5_" + std::to_string(n) + "_10000000/"
            "knapPI_5_" + std::to_string(n) + "_10000000_";

    } else if (d == "ss3") {
        return "data/knapsack/smallcoeff/"
            "knapPI_6_" + std::to_string(n) + "_1000/"
            "knapPI_6_" + std::to_string(n) + "_1000_";
    } else if (d == "ss4") {
        return "data/knapsack/smallcoeff/"
            "knapPI_6_" + std::to_string(n) + "_10000/"
            "knapPI_6_" + std::to_string(n) + "_10000_";
    } else if (d == "ss5") {
        return "data/knapsack/largecoeff/"
            "knapPI_6_" + std::to_string(n) + "_100000/"
            "knapPI_6_" + std::to_string(n) + "_100000_";
    } else if (d == "ss6") {
        return "data/knapsack/largecoeff/"
            "knapPI_6_" + std::to_string(n) + "_1000000/"
            "knapPI_6_" + std::to_string(n) + "_1000000_";
    } else if (d == "ss7") {
        return "data/knapsack/largecoeff/"
            "knapPI_6_" + std::to_string(n) + "_10000000/"
            "knapPI_6_" + std::to_string(n) + "_10000000_";

    } else if (d == "sw5") {
        return "data/knapsack/smallcoeff/"
            "knapPI_9_" + std::to_string(n) + "_1000/"
            "knapPI_9_" + std::to_string(n) + "_1000_";
    } else if (d == "sw7") {
        return "data/knapsack/largecoeff/"
            "knapPI_9_" + std::to_string(n) + "_1000000/"
            "knapPI_9_" + std::to_string(n) + "_1000000_";
    } else if (d == "sw8") {
        return "data/knapsack/largecoeff/"
            "knapPI_9_" + std::to_string(n) + "_1000000/"
            "knapPI_9_" + std::to_string(n) + "_1000000_";

    } else if (d == "sp/u3") {
        return "data/knapsack/hardinstances/"
            "knapPI_11_" + std::to_string(n) + "_1000/"
            "knapPI_11_" + std::to_string(n) + "_1000_";
    } else if (d == "sp/wc3") {
        return "data/knapsack/hardinstances/"
            "knapPI_12_" + std::to_string(n) + "_1000/"
            "knapPI_12_" + std::to_string(n) + "_1000_";
    } else if (d == "sp/sc3") {
        return "data/knapsack/hardinstances/"
            "knapPI_13_" + std::to_string(n) + "_1000/"
            "knapPI_13_" + std::to_string(n) + "_1000_";
    } else if (d == "mstr3") {
        return "data/knapsack/hardinstances/"
            "knapPI_14_" + std::to_string(n) + "_1000/"
            "knapPI_14_" + std::to_string(n) + "_1000_";
    } else if (d == "pceil3") {
        return "data/knapsack/hardinstances/"
            "knapPI_15_" + std::to_string(n) + "_1000/"
            "knapPI_15_" + std::to_string(n) + "_1000_";
    } else if (d == "circle3") {
        return "data/knapsack/hardinstances/"
            "knapPI_16_" + std::to_string(n) + "_1000/"
            "knapPI_16_" + std::to_string(n) + "_1000_";
    }
    return "";
}

void bench_literature(
        std::string algorithm,
        std::string dataset_name,
        std::vector<std::string>& dataset,
        std::mt19937_64& generator)
{
    std::ofstream file(algorithm + "_" + dataset_name + ".csv");

    // CSV
    file << "n \\ ds";
    for (auto& d: dataset)
        file << "," << d;
    file << std::endl;

    for (ItemIdx n: {50, 100, 200, 500, 1000, 2000, 5000, 10000}) {

        file << n << std::flush; // CSV

        for (Counter k = 0; k < (Counter)dataset.size(); ++k) {

            std::cout << "n " << n << ", " << dataset[k] << "..." << std::flush; // Standard output

            double t_max = 300;
            double t_total = 0.0;
            double mean = -1;
            for (Counter h = 1; h <= 100 && t_total < t_max; ++h) {
                Instance instance(path(dataset[k], n) + std::to_string(h) + ".csv", "pisinger");
                //std::cout << path(dataset[k], n) + std::to_string(h) + ".csv" << std::endl;
                try {
                    Info info = Info()
                        .set_time_limit(t_max - t_total)
                        //.set_verbosity_level(1)
                        ;
                    auto output = run(algorithm, instance, generator, info);

                    if (instance.optimal_solution() != NULL) {
                        if (output.solution.feasible()
                                && output.solution.profit() > instance.optimum()) {
                            throw std::runtime_error(
                                    "Computed solution strictly better"
                                    " than provided optimum.");
                        }
                        if (output.upper_bound != -1
                                && output.upper_bound < instance.optimum()) {
                            throw std::runtime_error(
                                    "Computed upper bound strictly lower"
                                    " than provided optimum.");
                        }
                    }

                    double t = info.elapsed_time();
                    t_total += t;
                } catch (...) {
                    t_total = t_max + 1;
                }
            }

            if (t_total <= t_max) {
                mean = round(t_total * 100) / 10;
                std::cout << " mean " << mean << std::endl; // Standard output
                file << "," << mean << std::flush; // CSV
            } else {
                std::cout << " x" << std::endl; // Standard output
                file << ",x" << std::flush; // CSV
            }
        }

        file << std::endl; // CSV
    }

    std::cout << std::endl; // Standard output
    file << std::endl; // CSV
}

void bench_normal(
        std::string algorithm,
        double time_limit,
        std::mt19937_64& generator)
{
    std::vector<ItemIdx> ns {100, 1000, 10000, 100000};
    std::vector<Weight> rs {1000, 10000, 100000, 1000000, 10000000, 100000000};
    std::vector<double> xs {0.0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9};

    nlohmann::json json;
    json["lab"][0] = {"n", ns};
    json["lab"][1] = {"r", rs};
    json["lab"][2] = {"x", xs};

    int val_max_r = 255 - 52;
    int val_max_g = 255 - 101;
    int val_max_b = 255 - 164;
    int col_r = 0;
    int col_g = 0;
    int col_b = 0;

    for (Counter in = 0; in < (Counter)ns.size(); ++in) {
        ItemIdx n = ns[in];
        for (Counter ir = 0; ir < (Counter)rs.size(); ++ir) {
            Weight r = rs[ir];

            // Standard output
            std::string s = "--- n " + std::to_string(n) + " r " + std::to_string(r) + " --- ";
            int pos = (int)((80 - s.length())/2);
            for(int i = 0; i < pos; i++)
                std::cout << " ";
            std::cout << s << std::endl;

            for (Counter ix = 0; ix < (Counter)xs.size(); ++ix) {
                double x = xs[ix];

                // Standard output
                std::cout
                    << "x " << std::right << std::setw(5) << x
                    << std::flush;

                Instance instance(
                        "data/knapsack/normal/knap_n" + std::to_string(n)
                        + "_r" + std::to_string(r)
                        + "_x0." + std::to_string((int)(10 * x)), "standard");
                double t = time_limit + 1;
                Info info = Info()
                    .set_time_limit(time_limit)
                    //.set_verbosity_level(1)
                    ;
                Output output(instance, info);
                try {
                    info.reset_time();
                    output = run(algorithm, instance, generator, info);
                    t = info.elapsed_time();
                } catch (...) {
                }

                std::stringstream t_str;
                if (t <= time_limit && output.lower_bound == output.upper_bound) {
                    t_str << (double)std::round(t * 10000) / 10;
                    std::cout << "\033[32m";
                    col_r = 255 - (int)(val_max_r * cbrt(t / time_limit));
                    col_g = 255 - (int)(val_max_g * cbrt(t / time_limit));
                    col_b = 255 - (int)(val_max_b * cbrt(t / time_limit));
                } else {
                    t_str << "> " << time_limit * 1000;
                    col_r = 0;
                    col_g = 0;
                    col_b = 0;
                }

                // Json
                std::string rgb_str = "rgb("
                    + std::to_string(col_r) + ","
                    + std::to_string(col_g) + ","
                    + std::to_string(col_b) + ")";
                json["tab"][in][ir][ix][0]["c"] = rgb_str;
                json["tab"][in][ir][ix][0]["t"] = t_str.str();

                // Standard output
                std::cout << " | LB" << std::right << std::setw(20) << output.solution.profit();
                std::cout << " | UB" << std::right << std::setw(20) << output.upper_bound;
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
    } catch (const po::required_option& e) {
        std::cout << desc << std::endl;;
        return 1;
    }

    Seed seed = 0;
    std::mt19937_64 gen(seed);

    for (std::string algorithm: algorithms) {
        for (std::string dataset: datasets) {
            std::cout << "*** " << algorithm << " / " << dataset << " ***" << std::endl;

            if (dataset == "easy" || dataset == "literature") {
                std::vector<std::string> dataset_easy {
                        "u3", "u4",
                        "wc3", "wc4",
                        "sc3", "sc4",
                        "isc3", "isc4",
                        "asc3", "asc4",
                        "ss3", "ss4",
                        "sw5",
                };
                bench_literature(algorithm, "easy", dataset_easy, gen);
            }

            if (dataset == "difficultsmall" || dataset == "literature") {
                std::vector<std::string> dataset_difficultsmall {
                        "sp/u3", "sp/wc3", "sp/sc3", "mstr3", "pceil3", "circle3",
                };
                bench_literature(algorithm, "difficultsmall", dataset_difficultsmall, gen);
            }

            if (dataset == "difficultlarge" || dataset == "literature") {
                std::vector<std::string> dataset_difficultlarge {
                        "u5", "u6", "u7",
                        "wc5", "wc6", "wc7",
                        "sc5", "sc6", "sc7",
                        "isc5", "isc6", "isc7",
                        "asc5", "asc6", "asc7",
                        "ss5", "ss6", "ss7",
                        "sw7", "sw8",
                };
                bench_literature(algorithm, "difficultlarge", dataset_difficultlarge, gen);
            }

            if (dataset == "normal")
                bench_normal(algorithm, time_limit, gen);

        }
    }

    return 0;
}

