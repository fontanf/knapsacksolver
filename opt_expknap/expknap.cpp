#include "expknap.hpp"

#include "../lb_greedy/greedy.hpp"
#include "../ub_dembo/dembo.hpp"

#define DBG(x)
//#define DBG(x) x

Solution sopt_expknap(Instance& ins, Info* info)
{
    (void)info;
    if (ins.item_number() == 0)
        return Solution(ins);

    ins.sort_partially();
    if (ins.break_item() == ins.last_item()+1)
        return *ins.break_solution();

    Solution sol_curr = *ins.break_solution();
    Solution sol_best = sol_bestgreedy(ins);
    ItemPos a = ins.break_item() - 1;
    ItemPos b = ins.break_item();
    ItemPos d = -1; // depth
    std::vector<int> t(ins.item_number());
    while (true) {
        d++;
        int x = 0;
        DBG(std::cout << "F " << ins.first_item() << " A " << a
                << " B " << b << " L " << ins.last_item()
                << " D " << d
                << " R " << sol_curr.remaining_capacity()
                << " P " << sol_curr.profit()
                << std::flush;)

        if (ins.int_right_size() > 0 && b+1 > ins.last_sorted_item())
            ins.sort_right(sol_best.profit());
        if (ins.int_left_size() > 0 && a-1 < ins.first_sorted_item())
            ins.sort_left(sol_best.profit());

        // Leaf test
        if ((sol_curr.remaining_capacity() >= 0 && b == ins.last_item() + 1)
                || (sol_curr.remaining_capacity() < 0 && a == ins.first_item() - 1)) {
            DBG(std::cout << " BT" << std::flush;)
            while (true) {
                d--;
                DBG(std::cout << " " << d << std::flush;)
                if (d < 0)
                    goto end;
                if (t[d] == 1) {
                    b--;
                    DBG(std::cout << " B " << b << std::flush;)
                    if (sol_curr.contains(b)) {
                        x = 1;
                        break;
                    }
                } else {
                    a++;
                    DBG(std::cout << " A " << a << std::flush;)
                    if (!sol_curr.contains(a)) {
                        x = -1;
                        break;
                    }
                }
            }
        }

        if (x == 1 || (x == 0 && sol_curr.remaining_capacity() >= 0)) {
            t[d] = 1;
            if (!sol_curr.contains(b)) { // Add item b
                Weight r = sol_curr.remaining_capacity() - ins.item(b).w;
                DBG(std::cout << " " << ins.optimal_solution()->contains(b) << std::flush;)
                DBG(std::cout << " R' " << r << std::flush;)
                DBG(std::cout << " UB" << std::flush;)
                Profit ub = (r >= 0)?
                        ub_dembo(ins, b+1, sol_curr.profit() + ins.item(b).p, r):
                        ub_dembo_rev(ins, a, sol_curr.profit() + ins.item(b).p, r);
                DBG(std::cout << " " << ub << std::flush;)
                if (ub > sol_best.profit()) {
                    DBG(std::cout << " OK" << std::flush;)
                    sol_curr.set(b, true);
                    sol_best.update(sol_curr); // Update best solution
                } else {
                    DBG(std::cout << " X" << std::flush;)
                }
            } else { // Remove item
                sol_curr.set(b, false);
            }
            b++;
        } else {
            t[d] = -1;
            if (sol_curr.contains(a)) { // Remove item a
                Weight r = sol_curr.remaining_capacity() + ins.item(a).w;
                DBG(std::cout << " " << ins.optimal_solution()->contains(a) << std::flush;)
                DBG(std::cout << " R' " << r << std::flush;)
                DBG(std::cout << " UB" << std::flush;)
                Profit ub = (r >= 0)?
                        ub_dembo(ins, b, sol_curr.profit() - ins.item(a).p, r):
                        ub_dembo_rev(ins, a-1, sol_curr.profit() - ins.item(a).p, r);
                DBG(std::cout << " " << ub << std::flush;)
                if (ub > sol_best.profit()) {
                    DBG(std::cout << " OK" << std::flush;)
                    sol_curr.set(a, false);
                    sol_best.update(sol_curr); // Update best solution
                } else {
                    DBG(std::cout << " X" << std::flush;)
                }
            } else { // Remove item
                sol_curr.set(a, true);
            }
            a--;
        }
        DBG(std::cout << std::endl;)
    }
end:

    DBG(std::cout << std::endl << "END" << std::endl;)
    assert(ins.check_sopt(sol_best));
    return sol_best;
}

#undef DBG
