#pragma once

#include "../lib/instance.hpp"
#include "../lib/solution.hpp"

Profit    opt_bellman_array(const Instance& ins, Info* info = NULL);
Solution sopt_bellman_array_all(const Instance& ins, Info* info = NULL);
Solution sopt_bellman_array_one(const Instance& ins, Info* info = NULL);
Solution sopt_bellman_array_part(const Instance& ins, Info* info = NULL);
Solution sopt_bellman_array_rec(const Instance& ins, Info* info = NULL);

