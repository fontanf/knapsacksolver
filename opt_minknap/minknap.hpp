#pragma once

#include "knapsack/lib/instance.hpp"
#include "knapsack/lib/solution.hpp"
#include "knapsack/lib/part_solution_2.hpp"

namespace knapsack
{

using namespace knapsack;

struct MinknapState
{
    Weight w;
    Profit p;
    PartSol2 sol;
};

std::ostream& operator<<(std::ostream& os, const MinknapState& s);

struct MinknapParams
{
    Cpt k = 64;
    StateIdx lb_greedy = 0;
    StateIdx lb_greedynlogn = -1;
    StateIdx lb_pairing = -1;
    StateIdx ub_surrogate = -1;
    StateIdx solve_sur = -1;
    bool combo_core = false;
    double time_limit = -1;

    static MinknapParams pure()
    {
        return {
            .k = 64,
            .lb_greedy = -1,
            .lb_greedynlogn = -1,
            .lb_pairing = -1,
            .ub_surrogate = -1,
            .solve_sur = -1,
            .combo_core = false,
        };
    }

    static MinknapParams combo()
    {
        return {
            .k = 64,
            .lb_greedy = 0,
            .lb_greedynlogn = -1,
            .lb_pairing = 10000,
            .ub_surrogate = 2000,
            .solve_sur = 2000,
            .combo_core = true,
        };
    }

    static MinknapParams fontan()
    {
        return {
            .k = 64,
            .lb_greedy = 0,
            .lb_greedynlogn = 5000,
            .lb_pairing = 10000,
            .ub_surrogate = 2000,
            .solve_sur = 2000,
            .combo_core = true,
        };
    }
};

class Minknap
{

public:

    Minknap(Instance& ins, MinknapParams p):
        instance_(ins),
        params_(p),
        psolf_(ins, p.k)
    {  }

    Solution run(Info info = Info());

private:

    void add_item(Info& info);
    void remove_item(Info& info);

    Instance& instance_;
    MinknapParams params_;

    Profit opt_ = -1;
    Profit lb_ = 0;
    Profit ub_ = -1;
    ItemPos s_;
    ItemPos t_;
    std::vector<MinknapState> l0_;
    PartSolFactory2 psolf_;
    MinknapState best_state_;
    StateIdx state_number_ = 0;
    StateIdx distinct_state_number_ = 0;

};

Profit opt_minknap(Instance& ins, Info info);
Profit opt_minknap(Instance& ins);

}

