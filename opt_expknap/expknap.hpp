#pragma once

#include "knapsack/lib/instance.hpp"
#include "knapsack/lib/solution.hpp"
#include "knapsack/ub_surrogate/surrogate.hpp"

#include <thread>

namespace knapsack
{

struct ExpknapParams
{
    StateIdx lb_greedy = 0;
    StateIdx lb_greedynlogn = -1;
    StateIdx ub_surrogate = -1;
    bool combo_core = false;

    static ExpknapParams pure()
    {
        return {
            .lb_greedy = -1,
            .lb_greedynlogn = -1,
            .ub_surrogate = -1,
            .combo_core = false,
        };
    }

    static ExpknapParams fontan()
    {
        return {
            .lb_greedy = 0,
            .lb_greedynlogn = 50000,
            .ub_surrogate = 20000,
            .combo_core = false,
        };
    }
};

class Expknap
{

public:

    Expknap(Instance& ins, ExpknapParams p, bool* end = NULL):
        instance_(ins), params_(p), sol_curr_(ins), sol_best_(ins), end_(end)
    {
        sur_ = (end_ != NULL);
        if (end_ == NULL)
            end_ = new bool(false);
    }

    ~Expknap()
    {
        if (!sur_)
            delete end_;
    }

    Solution run(Info info = Info());

private:

    void rec(ItemPos s, ItemPos t, Info& info);
    void update_bounds(Info& info);

    Instance& instance_;
    ExpknapParams params_;

    Profit opt_ = -1;
    Profit ub_ = -1;
    Profit lb_ = 0;
    Solution sol_curr_;
    Solution sol_best_;
    Cpt node_number_ = 0;

    bool sur_ = false;
    bool* end_ = NULL;
    std::vector<std::thread> threads_;

};

}

