#pragma once

#include "knapsacksolver/knapsack/algorithms/greedy.hpp"
#include "knapsacksolver/knapsack/algorithms/greedy_nlogn.hpp"
#include "knapsacksolver/knapsack/algorithms/upper_bound_dantzig.hpp"
#include "knapsacksolver/knapsack/algorithms/surrogate_relaxation.hpp"
#include "knapsacksolver/knapsack/algorithms/dynamic_programming_bellman.hpp"
#include "knapsacksolver/knapsack/algorithms/dynamic_programming_profits.hpp"
#include "knapsacksolver/knapsack/algorithms/dynamic_programming_balancing.hpp"
#include "knapsacksolver/knapsack/algorithms/dynamic_programming_primal_dual.hpp"
#include "knapsacksolver/knapsack/algorithms/branch_and_bound.hpp"
#include "knapsacksolver/knapsack/algorithms/branch_and_bound_primal_dual.hpp"

namespace knapsacksolver
{
namespace knapsack
{

Output run(
        std::string algorithm,
        Instance& instance,
        std::mt19937_64& generator,
        Info info);

}
}

