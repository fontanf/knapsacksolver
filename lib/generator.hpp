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
    int         h       = -1;
    std::string t       = "u";
    Seed        s       = 0;

    bool reduce         = false;

    std::default_random_engine g;

};

std::ostream& operator<<(std::ostream& os, const GenerateData& data);

Instance generate(GenerateData data);

}

