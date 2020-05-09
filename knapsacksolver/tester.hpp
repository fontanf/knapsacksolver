#pragma once

#include "knapsacksolver/solution.hpp"

#include <gtest/gtest.h>

namespace knapsacksolver
{

enum TestType { SOPT, OPT, LB, UB };
enum InstacesType { TEST, SMALL, MEDIUM, SC };

void test(InstacesType instances_type, std::vector<Output (*)(Instance&)> algorithms, TestType test_type = SOPT);

}

