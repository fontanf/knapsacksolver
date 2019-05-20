#pragma once

#include "knapsack/lib/instance.hpp"

#include <gtest/gtest.h>

namespace knapsack
{

enum TestType { OPT, LB, UB };
enum InstacesType { TEST, SMALL, MEDIUM, SC };

void test(InstacesType it, std::vector<Profit (*)(Instance&)> fs, TestType tt = OPT);

}

