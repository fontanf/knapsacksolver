#pragma once

#include "knapsack/lib/solution.hpp"

#include <gtest/gtest.h>

namespace knapsack
{

enum TestType { SOPT, OPT, LB, UB };
enum InstacesType { TEST, SMALL, MEDIUM, SC };

void test(InstacesType it, std::vector<Output (*)(Instance&)> fs, TestType tt = SOPT);

}

