#pragma once

#include "knapsack/lib/instance.hpp"
#include "knapsack/lib/solution.hpp"
#include "knapsack/lib/part_solution_1.hpp"

#include <thread>
#include <map>

namespace knapsack
{

struct BalknapState
{
    Weight mu;
    Profit pi;
    bool operator()(const BalknapState& s1, const BalknapState& s2)
    {
        if (s1.mu != s2.mu)
            return s1.mu < s2.mu;
        if (s1.pi != s2.pi)
            return s1.pi < s2.pi;
        return false;
    }
};

struct BalknapValue
{
    ItemPos a;
    ItemPos a_prec;
    PartSol1 sol;
};

std::ostream& operator<<(std::ostream& os, const std::pair<BalknapState, BalknapValue>& s);

struct BalknapParams
{
    ItemPos k            = 64;
    char ub              = 't';
    StateIdx greedy      = 0;
    StateIdx greedynlogn = -1;
    StateIdx surrogate   = 2000;

    static BalknapParams pure()
    {
        return {
            .k = 64,
            .ub = 'b',
            .greedy = -1,
            .greedynlogn = -1,
            .surrogate = -1,
        };
    }

    static BalknapParams combo()
    {
        return {
            .k = 64,
            .ub = 't',
            .greedy = 0,
            .greedynlogn = 5000,
            .surrogate = 2000,
        };
    }
};

class Balknap
{

public:

    Balknap(Instance& ins, BalknapParams p, bool* end = NULL):
        instance_(ins),
        params_(p),
        sol_best_(ins),
        end_(end)
    {
        sur_ = (end_ != NULL);
        if (end_ == NULL)
            end_ = new bool(false);
    }

    ~Balknap()
    {
        if (!sur_)
            delete end_;
    }

    Solution run(Info info = Info());

private:

    ItemPos find_state(bool left, Info& info);

    void update_bounds(Info& info);

    Instance& instance_;
    BalknapParams params_;

    Profit lb_ = 0;
    Profit ub_ = -1;
    Solution sol_best_;

    ItemPos t_;
    std::map<BalknapState, BalknapValue, BalknapState> map_;

    StateIdx state_number_ = 0;
    StateIdx distinct_state_number_ = 0;

    bool sur_ = false;
    bool* end_ = NULL;
    std::vector<std::thread> threads_;

};

}

