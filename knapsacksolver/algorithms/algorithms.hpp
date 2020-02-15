#pragma once

#include "knapsacksolver/instance.hpp"
#include "knapsacksolver/solution.hpp"

#include "knapsacksolver/algorithms/bellman.hpp"
#include "knapsacksolver/algorithms/dpprofits.hpp"
#include "knapsacksolver/algorithms/branchandbound.hpp"
#include "knapsacksolver/algorithms/balknap.hpp"
#include "knapsacksolver/algorithms/minknap.hpp"
#include "knapsacksolver/algorithms/expknap.hpp"
#include "knapsacksolver/algorithms/greedy.hpp"
#include "knapsacksolver/algorithms/greedynlogn.hpp"
#include "knapsacksolver/algorithms/dantzig.hpp"
#include "knapsacksolver/algorithms/surrelax.hpp"

namespace knapsacksolver
{

typedef std::function<Output (Instance&, std::mt19937_64&, Info)> func;

func get_algorithm(std::string str);

}

