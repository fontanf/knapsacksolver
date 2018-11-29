#pragma once

#include "knapsack/lib/instance.hpp"

namespace knapsack
{

typedef int_fast64_t Seed;

struct GenerateData
{
    bool        spanner = false;
    ItemIdx     v       = 2;
    Profit      m       = 10;

    ItemIdx     k1      = 300; // 3*R / 10
    ItemIdx     k2      = 200; // 2*R / 10
    double      d       = 6;

    ItemIdx     n       = 1000;
    Profit      r       = 1000;
    int         h       = 50;
    std::string type    = "u";
    Seed        seed    = 0;
    std::default_random_engine g;
};

Instance generate(GenerateData data);

}

