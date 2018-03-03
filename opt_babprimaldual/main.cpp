#include "babprimaldual.hpp"

#include "../lib/solver.hpp"

class SolverBABPD: public Solver
{

private:

    Profit run_algo(
            Instance& instance,
            Solution& sol_best,
            Profit ub,
            std::map< std::string, std::string>& param,
            Info* info) override
    {
        std::string upper_bound = param.find("upper_bound")->second;
        if (Info::verbose(info))
            std::cout << "BRANCHANDBOUND..." << std::endl;
        BabPDData data(instance, upper_bound, info);
        data.update_best_solution(sol_best);
        data.ub = ub;
        sopt_babprimaldual(data);
        sol_best.update(data.sol_best);
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
    std::string reduction   = "";
    std::string upper_bound = "trivial";
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "produce help message")
        ("input-data,i",  po::value<std::string>(&input_data)->required(), "set input data (required)")
        ("output-file,o", po::value<std::string>(&output_file),            "set output file")
        ("cert-file,c",   po::value<std::string>(&cert_file),              "set certificate output file")
        ("reduction,r",   po::value<std::string>(&reduction),              "set reduction")
        ("upper-bound,u", po::value<std::string>(&upper_bound),            "set upper bound")
        ("surrogate,s",                                                    "solve surrogate instance")
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
    std::map<std::string, std::string> param;
    param["upper_bound"] = upper_bound;
    param["reduction"]   = reduction;
    if ((vm.count("surrogate")))
        param["surrogate"] = "";

    Instance instance(input_data);
    Solution sol(instance);
    Info info;
    info.verbose(vm.count("verbose"));

    SolverBABPD().run(instance, sol, param, &info);

    double t = info.elapsed_time();
    info.pt.put("Solution.Time", t);
    info.pt.put("Solution.OPT", sol.profit());
    if (Info::verbose(&info)) {
        std::cout << "---" << std::endl;
        std::cout << instance.print_opt(sol.profit()) << std::endl;
        std::cout << "TIME " << t << std::endl;
    }

    info.write_ini(output_file); // Write output file
    sol.write_cert(cert_file); // Write certificate file
    return 0;
}
