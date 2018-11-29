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
    ItemIdx     n       = 1000;
    Profit      r       = 1000;
    int         h       = 50;
    std::string type    = "u";
    Seed        seed    = 0;
    std::default_random_engine g;
};

Instance generate(GenerateData data);

}

