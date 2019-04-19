#pragma once

#include "gtest/gtest.h"

#include "knapsack/lib/instance.hpp"

namespace knapsack
{

enum TestType { OPT, LB, UB };
enum InstacesType { TEST, SMALL, MEDIUM, SC };

void test(InstacesType it, std::vector<Profit (*)(Instance&)> fs, TestType tt = OPT);

}

