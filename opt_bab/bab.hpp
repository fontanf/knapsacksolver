#ifndef BAB_HPP_TKVNPYD8
#define BAB_HPP_TKVNPYD8

#include "../lib/instance.hpp"
#include "../lib/solution.hpp"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

/**
 * ub_type: "trivial", "dantzig" or "dantzig_2"
 */
struct BabData
{
    BabData(const Instance& inst, std::string ub_type, Info* info = NULL):
        instance(inst),
        sol_curr(*inst.reduced_solution()),
        sol_best(*inst.reduced_solution()),
        ub_type(ub_type),
        info(info)
    {
        assert(ub_type == "trivial" || ub_type == "dantzig" || ub_type == "dantzig_2");
    }
    const Instance& instance;
    Solution sol_curr;
    Solution sol_best;
    ItemIdx i = 0;
    Profit ub = 0;
    Profit lb = 0;
    size_t nodes = 0;
    std::string ub_type;
    Info* info;

    bool update_best_solution() { return update_best_solution(sol_curr); }
    bool update_best_solution(const Solution& sol)
    {
        if (sol.profit() <= lb)
            return false;
        sol_best = sol;
        lb = sol_best.profit();
        if (Info::verbose(info))
            std::cout
                <<  "LB "   << sol_best.profit()
                << " GAP "  << instance.optimum() - sol_best.profit()
                << " NODE " << std::scientific << (double)nodes << std::defaultfloat
                << " TIME " << info->elapsed_time()
                << std::endl;
        return true;
    }
};

Profit sopt_bab(BabData& data);

Profit sopt_bab_rec(BabData& data);

Profit sopt_bab_stack(BabData& data);

#endif /* end of include guard: BAB_HPP_TKVNPYD8 */
