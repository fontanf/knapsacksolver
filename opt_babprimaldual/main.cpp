#include "babprimaldual.hpp"

#include "../lb_greedy/greedy.hpp"
#include "../lb_greedynlogn/greedynlogn.hpp"
#include "../ub_surrogate/surrogate.hpp"

int main(int argc, char *argv[])
{
    namespace po = boost::program_options;

    // Parse program options
    std::string input_data  = "";
    std::string output_file = "";
    std::string cert_file   = "";
    std::string algorithm   = "";
    std::string reduction   = "";
    std::string upper_bound = "trivial";
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "produce help message")
        ("input-data,i",  po::value<std::string>(&input_data)->required(), "set input data (required)")
        ("output-file,o", po::value<std::string>(&output_file),            "set output file")
        ("cert-file,c",   po::value<std::string>(&cert_file),              "set certificate output file")
        ("algorithm,a",   po::value<std::string>(&algorithm),              "set algorithm")
        ("reduction,r",   po::value<std::string>(&reduction),              "set reduction")
        ("upper-bound,u", po::value<std::string>(&upper_bound),            "set upper bound")
        ("verbose,v",                                                      "enable verbosity")
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

    Instance instance(input_data);
    Solution sol_best(instance);
    Info info;
    info.verbose(vm.count("verbose"));
    bool optimal = false;

    if (reduction == "2" || upper_bound == "dantzig") {
        instance.sort();
        sol_best = sol_bestgreedynlogn(instance);
    } else {
        instance.sort_partially();
        sol_best = sol_bestgreedy(instance);
    }
    Profit ub = ub_surrogate(instance, sol_best.profit()).ub;
    if (Info::verbose(&info)) {
        std::cout
            <<  "LB " << sol_best.profit() << " GAP " << instance.optimum() - sol_best.profit()
            << " UB " << ub << " GAP " << ub - instance.optimum() << std::endl;
    }
    optimal = (sol_best.profit() == ub);

    // Variable reduction
    if (!optimal) {
        if (reduction == "1") {
            optimal = instance.reduce1(sol_best, Info::verbose(&info));
        } else if (reduction == "2") {
            optimal = instance.reduce2(sol_best, Info::verbose(&info));
        }
    }

    if (!optimal) {
        BabPDData data(instance, upper_bound, &info);
        data.update_best_solution(sol_best);
        data.ub = ub;
        sopt_babprimaldual(data);
        sol_best.update(data.sol_best);
    }

    double t = info.elapsed_time();
    info.pt.put("Solution.Time", t);
    info.pt.put("Solution.OPT", sol_best.profit());
    if (Info::verbose(&info)) {
        std::cout << "---" << std::endl;
        std::cout << "OPT " << sol_best.profit() << std::endl;
        std::cout << "EXP " << instance.optimum() << std::endl;
        std::cout << "TIME " << t << std::endl;
    }

    info.write_ini(output_file); // Write output file
    sol_best.write_cert(cert_file); // Write certificate file
    return 0;
}
