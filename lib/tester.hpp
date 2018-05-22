#include <gtest/gtest.h>

#include "knapsack/lib/instance.hpp"

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

namespace knapsack
{

void test(std::string exec, std::string test);

void test_pisinger(
        std::vector<ItemIdx> ns,
        std::vector<Profit> rs,
        std::vector<std::string> types,
        std::vector<Profit (*)(Instance&)> fs,
        int test = 0);

}

