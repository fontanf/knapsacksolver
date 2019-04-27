#include "knapsack/lib/generator.hpp"

#include "knapsack/lib/instance.hpp"
#include "knapsack/opt_minknap/minknap.hpp"

#include <random>
#include <cmath>
#include <algorithm>

using namespace knapsack;

std::ostream& knapsack::operator<<(std::ostream& os, const GenerateData& data)
{
    os << "n " << data.n;
    if (data.normal)
        os << " w/normal sigma " << data.sigma;
    if (data.spanner) {
        os << " t spanner," << data.t << " v " << data.v << " m " << data.m;
    } else {
        os << " t " << data.t;
    }
    if (data.t != "sw")
        os << " r " << data.r;
    if (data.t == "mstr")
        os << " k1 " << data.k1 << " k2 " << data.k2;
    if (data.t == "mstr" || data.t == "pceil" || data.t == "circle" || data.t == "normal")
        os << " d " << data.d;
    if (data.h != -1)
        os << " h " << data.h;
    if (data.s != 0)
        os << " s " << data.s;
    return os;
}

std::pair<Weight, Profit> item(GenerateData& data)
{
    Weight w = -1;
    Profit p = -1;
    if (!data.normal) {
        std::uniform_int_distribution<Weight> d(1,data.r);
        w = d(data.g);
    } else {
        std::normal_distribution<double> d(data.r / 2, data.sigma);
        w = std::min(data.r, std::max((Weight)1, (Weight)d(data.g)));
    }

    if (data.t == "u") {
        std::uniform_int_distribution<Weight> d(1, data.r);
        p = d(data.g);
    } else if (data.t == "wc") {
        std::uniform_int_distribution<Profit> d2(-(data.r + 1) / 10, data.r / 10);
        p = std::max((Profit)1, w + d2(data.g));
    } else if (data.t == "sc") {
        p = w + data.r / 10;
    } else if (data.t == "isc") {
        p = w;
        w = p + data.r / 10;
    } else if (data.t == "asc") {
        std::uniform_int_distribution<Profit> d2(-(data.r + 1) / 500, data.r / 500);
        p = w + data.r / 10 + d2(data.g);
    } else if (data.t == "ss") {
        p = w;
    } else if (data.t == "sw") {
        if (!data.normal) {
            std::uniform_int_distribution<Weight> d(data.r, data.r + data.r / 1000);
            w = d(data.g);
        } else {
            std::normal_distribution<double> d(data.r + data.r / 2000, data.sigma);
            w = d(data.g);
        }
        std::uniform_int_distribution<Profit> d2(1, data.r / 100);
        p = d2(data.g);
    } else if (data.t == "mstr") {
        p = (w % (Profit)data.d == 0)? w + data.k1: w + data.k2;
    } else if (data.t == "pceil") {
        p = data.d * ((w - 1) / (Profit)data.d + 1);
    } else if (data.t == "circle") {
        p = data.d * sqrt(4 * data.r * data.r - (w - 2 * data.r) * (w - 2 * data.r));
    } else if (data.t == "normal") {
        std::normal_distribution<double> d(w, data.d);
        p = std::min(data.r, std::max((Profit)1, w + (Profit)d(data.g)));
    } else {
        exit(1);
    }
    return {w, p};
}

Instance generate_standard(GenerateData& data)
{
    Instance ins(data.n, 0);

    Weight wsum = 0;
    Weight wmax = 0;
    for (ItemIdx j=0; j<data.n; ++j) {
        auto wp = item(data);
        ins.add_item(wp.first, wp.second);
        wsum += wp.first;
        wmax = std::max(wmax, wp.first);
    }
    ins.set_capacity(std::max(wmax, (data.h * wsum) / 100));
    return ins;
}

Instance generate_spanner(GenerateData& data)
{
    std::vector<std::pair<Weight, Profit>> vec;
    for (ItemIdx j=0; j<data.v; ++j) {
        auto wp = item(data);
        Weight w = (2 * wp.first  - 1 + data.m) / data.m;
        Weight p = (2 * wp.second - 1 + data.m) / data.m;
        vec.push_back({w, p});
    }

    Instance ins(data.n, 0);
    ItemIdx i = 0;
    std::uniform_int_distribution<Cpt> dist(1, data.m);
    Weight wsum = 0;
    Weight wmax = 0;
    while (ins.item_number() < data.n) {
        auto wp = vec[i];
        i = (i + 1) % data.v;
        ItemIdx a = dist(data.g);
        Weight w = a * wp.first;
        Profit p = a * wp.second;
        ins.add_item(w, p);
        wsum += wp.first;
        wmax = std::max(wmax, w);
    }
    ins.set_capacity(std::max(wmax, (data.h * wsum) / 101));
    return ins;
}

Instance knapsack::generate(GenerateData data)
{
    data.g.seed(data.s);
    Instance ins = (data.spanner)? generate_spanner(data): generate_standard(data);
    return ins;
}

