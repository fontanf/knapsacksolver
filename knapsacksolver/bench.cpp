#include "knapsacksolver/generator.hpp"

#include "knapsacksolver/algorithms/algorithms.hpp"

#include <iomanip>
#include <sstream>

#include <boost/program_options.hpp>

using namespace knapsacksolver;

void bench_literature(
        std::string algorithm,
        std::string dataset_name,
        std::vector<std::pair<std::string, Generator>>& dataset,
        std::mt19937_64& gen)
{
    std::ofstream file(algorithm + "_" + dataset_name + ".csv");
    func f = get_algorithm(algorithm);

    // CSV
    file << "n \\ ds";
    for (auto& p: dataset)
        file << "," << p.first;
    file << std::endl;

    Seed s = 0;
    for (ItemIdx n: {50, 100, 200, 500, 1000, 2000, 5000, 10000}) {

        file << n << std::flush; // CSV

        for (Counter k = 0; k < (Counter)dataset.size(); ++k) {
            Generator& d = dataset[k].second;
            d.n = n;
            d.s = -1;
            d.h = -1;

            std::cout << d << "..." << std::flush; // Standard output

            double t_max = 300;
            double t_total = 0.0;
            double mean = -1;
            for (d.h=1; d.h<=d.hmax && t_total < t_max; ++d.h) {
                s++;
                d.s = s;
                Instance ins = d.generate();
                try {
                    Info info = Info()
                        .set_timelimit(t_max - t_total)
                        //.set_verbose(true)
                        ;
                    f(ins, gen, info);
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
    int col_r = 0;
    int col_g = 0;
    int col_b = 0;

    Generator d;
    d.normal = true;
    d.t = "normal";
    d.dw = 10;
    d.d = 10;
    d.s = 0;

    for (Counter in = 0; in < (Counter)ns.size(); ++in) {
        ItemIdx n = ns[in];
        d.n = n;
        for (Counter ir = 0; ir < (Counter)rs.size(); ++ir) {
            Weight r = rs[ir];
            d.r = r;

            // Standard output
            std::string s = "--- n " + std::to_string(n) + " r " + std::to_string(r) + " --- ";
            int pos = (int)((80 - s.length())/2);
            for(int i=0; i<pos; i++)
                std::cout << " ";
            std::cout << s << std::endl;

            for (Counter ix = 0; ix < (Counter)xs.size(); ++ix) {
                double x = xs[ix];
                d.x = x;
                d.s++;

                // Standard output
                std::cout
                    << "x " << std::right << std::setw(5) << x
                    << " s " << std::right << std::setw(5) << d.s
                    << std::flush;

                Instance ins = d.generate();
                double t = time_limit + 1;
                Info info = Info()
                    .set_timelimit(time_limit)
                    //.set_verbose(true)
                    ;
                Output output(ins, info);
                try {
                    info.reset_time();
                    output = f(ins, gen, info);
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
    } catch (po::required_option e) {
        std::cout << desc << std::endl;;
        return 1;
    }

    Seed seed = 0;
    std::mt19937_64 gen(seed);

    for (std::string algorithm: algorithms) {
        for (std::string dataset: datasets) {
            std::cout << "*** " << algorithm << " / " << dataset << " ***" << std::endl;

            if (dataset == "easy" || dataset == "literature") {
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
                bench_literature(algorithm, "easy", dataset_easy, gen);
            }

            if (dataset == "difficultsmall" || dataset == "literature") {
                std::vector<std::pair<std::string, Generator>> dataset_difficultsmall {
                        {"sp/u3", Generator::spanner_generator("u", 1000, 2, 10)},
                        {"sp/wc3", Generator::spanner_generator("wc", 1000, 2, 10)},
                        {"sp/sc3", Generator::spanner_generator("sc", 1000, 2, 10)},
                        {"mstr3", Generator::mstr_generator(1000, 300, 200, 6)},
                        {"pceil3", Generator::pceil_generator(1000, 3)},
                        {"circle3", Generator::circle_generator(1000, 2.0/3)},
                };
                bench_literature(algorithm, "difficultsmall", dataset_difficultsmall, gen);
            }

            if (dataset == "difficultlarge" || dataset == "literature") {
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
                bench_literature(algorithm, "difficultlarge", dataset_difficultlarge, gen);
            }

            if (dataset == "normal")
                bench_normal(algorithm, time_limit, gen);

        }
    }

    return 0;
}

