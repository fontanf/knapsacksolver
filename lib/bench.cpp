#include "knapsack/lib/generator.hpp"

#include "knapsack/lib/algorithms.hpp"

#include <boost/program_options.hpp>

using namespace knapsack;

double bench(func f, GenerateData d, std::mt19937_64& gen, bool verbose = false)
{
    double t_max = 300;
    double t_total = 0.0;
    std::cout << d << std::flush;
    for (d.h=1; d.h<=100; ++d.h) {
        d.s = d.n + d.r + d.h;
        if (verbose)
            std::cout << std::endl << d << std::endl;
        Instance ins = generate(d);
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
                if (verbose) {
                    std::cout << std::endl;
                } else {
                    std::cout << " ";
                }
                std::cout << "mean > " << t_max*10 << std::endl;
                return -1;
            }
        } catch (...) {
            std::cout << " x" << std::endl;
            std::cout << std::endl << d << std::flush;
            Instance ins = generate(d);
            Info info = Info()
                .set_timelimit(t_max - t_total)
                .set_verbose(true)
                .set_log2stderr(true)
                ;
            f(ins, sol, ub, gen, info);
            exit(1);
            return -1;
        }
    }
    d.h = -1;

    double mean = round(t_total * 100) / 10;
    if (verbose) {
        std::cout << std::endl;
    } else {
        std::cout << " ";
    }
    std::cout << "mean " << mean << std::endl;
    return mean;
}

void bench_easy(std::string algorithm, std::mt19937_64& gen, bool verbose = false)
{
    std::ofstream file(algorithm + "_easy.csv");
    func f = get_algorithm(algorithm);
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
        d.n = n;
        file << n << std::flush;
        for (auto p: vec) {
            d.t = p.first;
            d.r = p.second;
            double res = bench(f, d, gen, verbose);
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

void bench_difficult_large(std::string algorithm, std::mt19937_64& gen, bool verbose = false)
{
    std::ofstream file(algorithm + "_difficult-large.csv");
    func f = get_algorithm(algorithm);
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
            double res = bench(f, d, gen, verbose);
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

void bench_difficult_small(std::string algorithm, std::mt19937_64& gen, bool verbose = false)
{
    std::ofstream file(algorithm + "_difficult-small.csv");
    func f = get_algorithm(algorithm);
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
            double res = bench(f, d, gen, verbose);
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

void bench_miscellaneous(std::string algorithm, std::mt19937_64& gen, bool verbose = false)
{
    std::ofstream file(algorithm + "_miscellaneous.csv");
    func f = get_algorithm(algorithm);
    std::vector<GenerateData> vec {
        GenerateData::gen_normal(1000, 100),
        GenerateData::gen_normal(1000, 50),
        GenerateData::gen_normal(1000, 25),
        GenerateData::gen_normal(1000, 10),
    };

    file << "n \\ Type,N 100,N 100,"
        << std::endl;
    for (ItemIdx n: {50, 100, 200, 500, 1000, 2000, 5000, 10000}) {
        file << n << std::flush;
        for (GenerateData d: vec) {
            d.n = n;
            double res = bench(f, d, gen, verbose);
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

int main(int argc, char *argv[])
{
    namespace po = boost::program_options;

    // Parse program options
    std::string algorithm = "bellman_array";

    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "produce help message")
        ("algorithm,a", po::value<std::string>(&algorithm), "set algorithm")
        ("verbose,v", "")
        ("easy", "")
        ("difficult-small", "")
        ("difficult-large", "")
        ("miscellaneous", "")
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

    bool verbose = vm.count("verbose");
    if (vm.count("easy"))
        bench_easy(algorithm, gen, verbose);
    if (vm.count("difficult-small"))
        bench_difficult_small(algorithm, gen, verbose);
    if (vm.count("difficult-large"))
        bench_difficult_large(algorithm, gen, verbose);
    if (vm.count("miscellaneous"))
        bench_miscellaneous(algorithm, gen, verbose);

    return 0;
}

