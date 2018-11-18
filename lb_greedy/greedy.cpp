#include "knapsack/lb_greedy/greedy.hpp"

using namespace knapsack;

#define DBG(x)
//#define DBG(x) x

Solution knapsack::sol_greedy(const Instance& ins, Info& info)
{
    DBG(std::cout << "GREEDYBEST..." << std::endl;)
    assert(ins.break_item_found());

    Solution sol = *ins.break_solution();
    std::string best_algo = "Break";
    ItemPos b = ins.break_item();

    if (b < ins.last_item()) {
        Profit  p = 0;
        ItemPos j = -1;

        DBG(std::cout << "BACKWARD GREEDY" << std::endl;)
        Weight rb = sol.remaining_capacity() - ins.item(b).w;
        for (ItemPos k=ins.first_item(); k<=b; ++k) {
            if (rb + ins.item(k).w >= 0 && ins.item(b).p - ins.item(k).p > p) {
                p = ins.item(b).p - ins.item(k).p;
                j = k;
            }
        }

        DBG(std::cout << "FORWARD GREEDY" << std::endl;)
        Weight rf = sol.remaining_capacity();
        for (ItemPos k=b+1; k<=ins.last_item(); ++k) {
            if (ins.item(k).w <= rf && ins.item(k).p > p) {
                p = ins.item(k).p;
                j = k;
            }
        }

        DBG(std::cout << "B " << b << " J " << j << std::endl;)
        if (j == -1) {
            best_algo = "Break";
        } else if (j <= b) {
            best_algo = "Backward";
            sol.set(b, true);
            sol.set(j, false);
        } else {
            best_algo = "Forward";
            sol.set(j, true);
        }
    }

    if (info.verbose())
        std::cout << "ALGO " << best_algo << std::endl;
    info.pt.put("Solution.Algo", best_algo);

    ins.check_sol(sol);
    DBG(std::cout << "GREEDY... END" << std::endl;)
    return algorithm_end(sol, info);
}

#undef DBG

