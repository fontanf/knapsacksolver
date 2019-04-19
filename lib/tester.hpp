#pragma once

#include "gtest/gtest.h"

#include "knapsack/lib/instance.hpp"

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

namespace knapsack
{

enum TestType { OPT, LB, UB };
enum InstacesType { TEST, SMALL, MEDIUM, SC };

void test(InstacesType it, std::vector<Profit (*)(Instance&)> fs, TestType tt = OPT);

}

