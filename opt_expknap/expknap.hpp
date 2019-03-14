#pragma once

#include "knapsack/lib/instance.hpp"
#include "knapsack/lib/solution.hpp"
#include "knapsack/ub_surrogate/surrogate.hpp"

#include <mutex>
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

    static ExpknapParams combo()
    {
        return {
            .lb_greedy = 0,
            .lb_greedynlogn = -1,
            .ub_surrogate = 2000,
            .combo_core = true,
        };
    }
};

class Expknap
{

public:

    Expknap(Instance& ins, ExpknapParams p,
            std::shared_ptr<bool> end = nullptr):
        instance_(ins), params_(p), sol_curr_(ins), sol_best_(ins), end_(end)
    {
        if (end_ == nullptr)
            end_ = std::shared_ptr<bool>(new bool(false));
    }

    ~Expknap()
    {
        for (std::thread& thread: threads_)
            thread.join();
    }

    Solution run(Info& info);

private:

    void rec(Info& info);
    void update_bounds(Info& info);
    void surrogate(Instance ins, std::mutex& mutex, Info& info);
    void greedynlogn(Instance ins, std::mutex& mutex, Info& info);

    Instance& instance_;
    ExpknapParams params_;

    Profit opt_ = -1;
    Profit ub_ = -1;
    Solution sol_curr_;
    Solution sol_best_;
    ItemPos s_;
    ItemPos t_;
    Cpt node_number_ = 0;
    std::shared_ptr<bool> end_ = NULL;
    std::mutex mutex_;
    std::vector<std::thread> threads_;

};

}

