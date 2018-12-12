#pragma once

#include "gtest/gtest.h"

#include "knapsack/lib/instance.hpp"

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

namespace knapsack
{

Instance test_instance(Cpt i)
{
    Instance ins(0, 0);
    switch (i) {
    case 0: // 0 item
        return ins;
    case 1: // 1 item
        ins.set_capacity(8);
        ins.add_items({{6, 8}});
        return ins;
    case 2: // 2 items
        ins.set_capacity(10);
        ins.add_items({{9, 8}, {7, 6}});
        return ins;
    case 3:
        ins.set_capacity(10);
        ins.add_items({{9, 5}, {7, 6}});
        return ins;
    case 4:
        ins.set_capacity(10);
        ins.add_items({{5, 5}, {4, 6}});
        return ins;
    case 5:
        ins.set_capacity(12);
        ins.add_items({{5, 7}, {4, 5}, {3, 11}});
        return ins;
    case 6:
        ins.set_capacity(10);
        ins.add_items({{5, 7}, {4, 5}, {3, 11}});
        return ins;
    case 7:
        ins.set_capacity(10);
        ins.add_items({{1, 1}, {2, 2}, {9, 10}});
        return ins;
    case 8: // 4 items
        ins.set_capacity(95);
        ins.add_items({{24, 34}, {42, 52}, {32, 42}, {4, 14}});
        return ins;
    case 9: // 5 items
        ins.set_capacity(100);
        ins.add_items({{10, 10}, {10, 15}, {10, 5}, {10, 12}, {10, 20}});
        return ins;
    case 10:
        ins.set_capacity(15);
        ins.add_items({{6, 7}, {6, 7}, {5, 5}, {5, 5}, {5, 5}});
        return ins;
    case 11:
        ins.set_capacity(12);
        ins.add_items({{3, 3}, {2, 2}, {4, 4}, {10, 9}, {10, 8}});
        return ins;
    case 12: // 6 items
        ins.set_capacity(25);
        ins.add_items({{10, 100}, {6, 7}, {6, 7}, {5, 5}, {5, 5}, {5, 5}});
        return ins;
    case 13:
        ins.set_capacity(1884);
        ins.add_items({{829, 729}, {785, 685}, {710, 610}, {781, 681}, {128, 28}, {243, 143}});
        return ins;
    case 14: // 7 items
        ins.set_capacity(40);
        ins.add_items({{11, 12}, {11, 12}, {11, 12}, {10, 10}, {10, 10}, {10, 10}, {10, 10}});
        return ins;
    case 15:
        ins.set_capacity(9);
        ins.add_items({{2, 6}, {3, 5}, {6, 8}, {7, 9}, {5, 6}, {9, 7}, {4, 3}});
        return ins;
    default:
        return ins;
    }
}

enum TestType { OPT, LB, UB };
enum InstacesType { TEST, SMALL, MEDIUM };

void test(InstacesType it, std::vector<Profit (*)(Instance&)> fs, TestType tt = OPT);

}

