#pragma once

#include "knapsacksolver/solution.hpp"
#include "knapsacksolver/part_solution_1.hpp"

#include <thread>
#include <map>

namespace knapsacksolver
{

struct DynamicProgrammingBalancingOptionalParameters
{
    Info info = Info();

    char ub = 't';
    bool greedy = true;
    StateIdx greedy_nlogn = -1;
    StateIdx surrogate_relaxation = -1;
    ItemPos partial_solution_size = 64;

    bool* end = NULL;
    bool stop_if_end = false;
    bool set_end = true;

    DynamicProgrammingBalancingOptionalParameters& set_pure()
    {
        ub = 'b';
        greedy = false;
        greedy_nlogn = -1;
        surrogate_relaxation = -1;
        partial_solution_size = 64;
        return *this;
    }

    DynamicProgrammingBalancingOptionalParameters& set_combo()
    {
        ub = 't';
        greedy = -1;
        greedy_nlogn = 0;
        surrogate_relaxation = 2000;
        partial_solution_size = 64;
        return *this;
    }

};

struct DynamicProgrammingBalancingOutput: Output
{
    DynamicProgrammingBalancingOutput(
            const Instance& instance,
            Info& info):
        Output(instance, info) { }

    Counter number_of_recursive_calls = 0;

    DynamicProgrammingBalancingOutput& algorithm_end(Info& info)
    {
        info.add_to_json("Algorithm", "RecursiveCallNumber", number_of_recursive_calls);
        Output::algorithm_end(info);
        info.os() << "Number of recursive calls:  " << number_of_recursive_calls << std::endl;
        return *this;
    }
};

DynamicProgrammingBalancingOutput dynamic_programming_balancing(
        Instance& instance,
        DynamicProgrammingBalancingOptionalParameters parameters = {});

}

