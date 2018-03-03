#include "solver.hpp"

#include "../lb_greedy/greedy.hpp"
#include "../lb_greedynlogn/greedynlogn.hpp"
#include "../ub_surrogate/surrogate.hpp"

Profit Solver::run(
        Instance& instance,
        Solution& sol_best,
        const std::map< std::string, std::string>& param,
        Info* info)
{
    bool optimal = false;
    Profit ub;
    std::string reduction   = param.find("reduction")->second;
    std::string upper_bound = param.find("upper_bound")->second;
    bool        surrogate   = (param.find("surrogate") != param.end());

    if (Info::verbose(info))
        std::cout << "INITIAL BOUNDS..." << std::flush;
    if (reduction == "2" || upper_bound == "dantzig") {
        instance.sort();
        sol_best = sol_bestgreedynlogn(instance);
    } else {
        instance.sort_partially();
        sol_best = sol_bestgreedy(instance);
    }
    auto surout = ub_surrogate(instance, sol_best.profit());
    ub = surout.ub;
    if (Info::verbose(info)) {
        std::cout
            << " " << instance.print_lb(sol_best.profit())
            << " " << instance.print_ub(ub)
            << std::endl;
    }
    optimal = (sol_best.profit() == surout.ub);

    if (!optimal && surrogate) {
        if (Info::verbose(info))
            std::cout << "SOLVE SURROGATE INSTANCE..." << std::endl;
        Instance ins_sur(instance);
        ins_sur.surrogate(surout.multiplier, surout.bound);
        Info info_sur;
        info_sur.verbose(true);
        Solution sol_sur(ins_sur);
        run(ins_sur, sol_sur, param, &info_sur);
        ub = sol_sur.profit();
        if (Info::verbose(info))
            std::cout << "... SURROGATE INSTANCE SOLVED" << std::endl;
        if (Info::verbose(info))
            std::cout
                << instance.print_ub(ub)
                << " CARD " << sol_sur.item_number() << " K " << surout.bound << std::endl;
        if (sol_sur.item_number() == surout.bound) {
            sol_best = sol_sur;
            optimal = true;
        }
    }

    // Variable reduction
    if (!optimal) {
        if (reduction == "1") {
            optimal = instance.reduce1(sol_best, Info::verbose(info));
        } else if (reduction == "2") {
            optimal = instance.reduce2(sol_best, Info::verbose(info));
        }
    }

    // Run main algorithm
    if (!optimal)
        run_algo(instance, sol_best, ub, param, info);

    return sol_best.profit();
}
