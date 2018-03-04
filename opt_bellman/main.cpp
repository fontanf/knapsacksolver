#include "bellman.hpp"

#include "../lib/solver.hpp"

class SolverBellman: public Solver
{

private:

    Profit run_algo(
            Instance& instance,
            Solution& sol_best,
            Profit ub,
            std::map< std::string, std::string>& param,
            Info* info) override
    {
        (void)ub;
        std::string algorithm   = param.find("algorithm")->second;
        std::string lower_bound = param.find("lower_bound")->second;
        std::string upper_bound = param.find("upper_bound")->second;
        if (algorithm == "opt") {
            return std::max(sol_best.profit(), opt_bellman(instance, info));
        } else if (algorithm == "sopt_1") {
            sol_best.update(sopt_bellman_1(instance, info));
        } else if (algorithm == "sopt_1it") {
            sol_best.update(sopt_bellman_1_it(instance, info));
        } else if (algorithm == "sopt_1rec") {
            sol_best.update(sopt_bellman_1_rec(instance, info));
        } else if (algorithm == "sopt_1stack") {
            sol_best.update(sopt_bellman_1_stack(instance, info));
        } else if (algorithm == "sopt_1map") {
            sol_best.update(sopt_bellman_1_map(instance, info));
        } else if (algorithm == "sopt_2") {
            sol_best.update(sopt_bellman_2(instance, info));
        } else if (algorithm == "sopt_rec") {
            sol_best.update(sopt_bellman_rec(instance, info));
        } else if (algorithm == "opt_list") {
            return std::max(sol_best.profit(), opt_bellman_list(instance, sol_best.profit(), lower_bound, upper_bound, info));
        } else if (algorithm == "sopt_list_rec") {
            sol_best.update(sopt_bellman_rec_list(instance, sol_best, lower_bound, upper_bound, info));
        } else {
            std::cerr << "Unknown or missing algorithm" << std::endl;
            assert(false);
            return 1;
        }
        return sol_best.profit();
    }
};

int main(int argc, char *argv[])
{
    namespace po = boost::program_options;

    // Parse program options
    std::string input_data  = "";
    std::string output_file = "";
    std::string cert_file   = "";
    std::string algorithm   = "sopt_1";
    std::string reduction   = "";
    std::string lower_bound = "none";
    std::string upper_bound = "none";
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "produce help message")
        ("input-data,i",  po::value<std::string>(&input_data)->required(), "set input data (required)")
        ("output-file,o", po::value<std::string>(&output_file),            "set output file")
        ("cert-file,c",   po::value<std::string>(&cert_file),              "set certificate output file")
        ("algorithm,a",   po::value<std::string>(&algorithm),              "set algorithm")
        ("reduction,r",   po::value<std::string>(&reduction),              "choose variable reduction")
        ("lower-bound,l", po::value<std::string>(&lower_bound),            "set lower bound")
        ("upper-bound,u", po::value<std::string>(&upper_bound),            "set upper bound")
        ("verbose,v",                                                      "enable verbosity")
        ("surrogate,s",                                                    "solve surrogate instance")
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
    std::map<std::string, std::string> param;
    param["algorithm"]   = algorithm;
    param["upper_bound"] = upper_bound;
    param["lower_bound"] = lower_bound;
    param["reduction"]   = reduction;
    if ((vm.count("surrogate")))
        param["surrogate"] = "";

    Instance instance(input_data);
    Solution sol(instance);
    Info info;
    info.verbose(vm.count("verbose"));

    Profit opt = SolverBellman().run(instance, sol, param, &info);

    double t = info.elapsed_time();
    info.pt.put("Solution.OPT", opt);
    info.pt.put("Solution.Time", t);
    if (Info::verbose(&info)) {
        std::cout << "---" << std::endl;
        std::cout << instance.print_opt(sol.profit()) << std::endl;
        std::cout << "TIME " << t << std::endl;
    }

    info.write_ini(output_file); // Write output file
    sol.write_cert(cert_file); // Write certificate file
    return 0;
}
