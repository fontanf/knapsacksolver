#pragma once

#include "knapsack/lib/instance.hpp"
#include "knapsack/lib/solution.hpp"
#include "knapsack/ub_surrogate/surrogate.hpp"

#include <thread>

namespace knapsack
{

struct ExpknapParams
{
    StateIdx greedy = 0;
    StateIdx greedynlogn = -1;
    StateIdx surrogate = -1;
    bool combo_core = false;

    static ExpknapParams pure()
    {
        return {
            .greedy = -1,
            .greedynlogn = -1,
            .surrogate = -1,
            .combo_core = false,
        };
    }

    static ExpknapParams combo()
    {
        return {
            .greedy = 0,
            .greedynlogn = 50000,
            .surrogate = 20000,
            .combo_core = true,
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
    Solution sol_curr_;
    Solution sol_best_;
    Cpt node_number_ = 0;

    bool sur_ = false;
    bool* end_ = NULL;
    std::vector<std::thread> threads_;

};

}

