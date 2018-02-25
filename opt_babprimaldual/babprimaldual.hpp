#ifndef BAB_HPP_TKVNPYD8
#define BAB_HPP_TKVNPYD8

#include "../lib/instance.hpp"
#include "../lib/solution.hpp"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

#define DBG(x)
//#define DBG(x) x

struct BabPDData
{
    BabPDData(const Instance& inst, std::string ub_type, Info* info):
        instance(inst),
        sol_curr(*inst.reduced_solution()),
        sol_best(*inst.reduced_solution()),
        ub_type(ub_type),
        info(info)
    {
        assert(ub_type == "trivial" || ub_type == "dantzig");
        for (b=0; b<inst.item_number(); ++b) {
            if (instance.item(b).w > sol_curr.remaining_capacity())
                break;
            sol_curr.set(b, true);
            sol_best.set(b, true);
        }
        break_item = b;
        a = b-1;
        lb = sol_curr.profit();
        DBG(std::cout << "b " << break_item << " p(S) " << sol_curr.profit() << std::endl;)
    }
    const Instance& instance;
    Solution sol_curr;
    Solution sol_best;
    ItemIdx  a;
    ItemIdx  b  = 0;
    ItemIdx  break_item;
    Profit   ub = 0;
    Profit   lb;
    size_t   nodes = 0;
    std::string ub_type;
    Info* info;

    bool update_best_solution() { return update_best_solution(sol_curr); }
    bool update_best_solution(const Solution& sol)
    {
        DBG(std::cout << "UBDATEBESTSOLUTION... p(S) " << sol.profit() << " p(Sb) " << sol_best.profit() << " LB " << lb << std::endl;)
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

#undef DBG

Profit sopt_babprimaldual(BabPDData& data);

#endif /* end of include guard: BAB_HPP_TKVNPYD8 */
