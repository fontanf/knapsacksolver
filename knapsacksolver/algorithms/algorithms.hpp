#pragma once

#include "knapsacksolver/algorithms/greedy.hpp"
#include "knapsacksolver/algorithms/greedy_nlogn.hpp"
#include "knapsacksolver/algorithms/upper_bound_dantzig.hpp"
#include "knapsacksolver/algorithms/surrogate_relaxation.hpp"
#include "knapsacksolver/algorithms/dynamic_programming_bellman.hpp"
#include "knapsacksolver/algorithms/dynamic_programming_profits.hpp"
#include "knapsacksolver/algorithms/dynamic_programming_balancing.hpp"
#include "knapsacksolver/algorithms/dynamic_programming_primal_dual.hpp"
#include "knapsacksolver/algorithms/branch_and_bound.hpp"
#include "knapsacksolver/algorithms/branch_and_bound_primal_dual.hpp"

namespace knapsacksolver
{

Output run(
        std::string algorithm,
        Instance& instance,
        std::mt19937_64& generator,
        Info info);

}

