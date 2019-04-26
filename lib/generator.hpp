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

    std::default_random_engine g;

    static GenerateData gen_spanner(std::string type, Profit r, ItemIdx v, Profit m)
    {
        GenerateData d;
        d.r = r;
        d.t = type;
        d.spanner = true;
        d.v = v;
        d.m = m;
        return d;
    };

    static GenerateData gen_mstr(Profit r, ItemIdx k1, ItemIdx k2, double d)
    {
        GenerateData data;
        data.t = "mstr";
        data.r = r;
        data.k1 = k1;
        data.k2 = k2;
        data.d = d;
        return data;
    };

    static GenerateData gen_pceil(Profit r, double d)
    {
        GenerateData data;
        data.t = "pceil";
        data.r = r;
        data.d = d;
        return data;
    };

    static GenerateData gen_circle(Profit r, double d)
    {
        GenerateData data;
        data.t = "circle";
        data.r = r;
        data.d = d;
        return data;
    };

    static GenerateData normal(Profit r, double d)
    {
        GenerateData data;
        data.t = "normal";
        data.r = r;
        data.d = d;
        return data;
    };

};

std::ostream& operator<<(std::ostream& os, const GenerateData& data);

Instance generate(GenerateData data);

}

