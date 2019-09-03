#include "knapsack/lib/generator.hpp"

#include "knapsack/lib/instance.hpp"
#include "knapsack/opt_minknap/minknap.hpp"

#include <random>
#include <cmath>
#include <algorithm>

using namespace knapsack;

std::ostream& knapsack::operator<<(std::ostream& os, const Generator& data)
{
    os << "n " << data.n;
    if (data.normal)
        os << " w/normal dw " << data.dw;
    if (data.spanner) {
        os << " t spanner," << data.t << " v " << data.v << " m " << data.m;
    } else {
        os << " t " << data.t;
    }
    os << " r " << data.r;
    if (data.t == "mstr")
        os << " k1 " << data.k1 << " k2 " << data.k2 << " d " << data.d;
    if (data.t == "normal")
        os << " d " << data.d;
    if (data.t == "pceil" || data.t == "circle")
        os << " d " << data.d;
    if (data.h != -1) {
        os << " h " << data.h << "/" << data.hmax;
    } else {
        os << " x " << data.x;
    }
    if (data.s != -1)
        os << " s " << data.s;
    return os;
}

std::pair<Weight, Profit> Generator::item()
{
    Weight w = -1;
    Profit p = -1;
    if (!normal) {
        std::uniform_int_distribution<Weight> dist(1, r);
        w = dist(g);
    } else {
        std::normal_distribution<double> dist(r / 2, (double)r / dw);
        do {
            w = (Weight)dist(g);
        } while (w < 1 || w > r);
    }

    if (t == "u") {
        std::uniform_int_distribution<Weight> dist(1, r);
        p = dist(g);
    } else if (t == "wc") {
        std::uniform_int_distribution<Profit> dist2(-(r + 1) / 10, r / 10);
        p = std::max((Profit)1, w + dist2(g));
    } else if (t == "sc") {
        p = w + r / 10;
    } else if (t == "isc") {
        p = w;
        w = p + r / 10;
    } else if (t == "asc") {
        std::uniform_int_distribution<Profit> dist(-(r + 1) / 500, r / 500);
        p = w + r / 10 + dist(g);
    } else if (t == "ss") {
        p = w;
    } else if (t == "sw") {
        if (!normal) {
            std::uniform_int_distribution<Weight> dist(r, r + r / 1000);
            w = dist(g);
        } else {
            std::normal_distribution<double> dist(r + r / 2000, (double)r / dw);
            w = dist(g);
        }
        std::uniform_int_distribution<Profit> dist2(1, r / 100);
        p = dist2(g);
    } else if (t == "mstr") {
        p = (w % (Profit)d == 0)? w + k1: w + k2;
    } else if (t == "pceil") {
        p = d * ((w - 1) / (Profit)d + 1);
    } else if (t == "circle") {
        p = d * sqrt(4 * r * r - (w - 2 * r) * (w - 2 * r));
    } else if (t == "normal") {
        std::normal_distribution<double> dist(w, (double)w / d);
        do {
            p = (Profit)dist(g);
        } while (p < 1 || p > r);
    } else {
        exit(1);
    }
    return {w, p};
}

Instance Generator::generate_standard()
{
    Instance ins;
    Weight wsum = 0;
    Weight wmax = 0;
    for (ItemIdx j=0; j<n; ++j) {
        auto wp = item();
        ins.add_item(wp.first, wp.second);
        wsum += wp.first;
        wmax = std::max(wmax, wp.first);
    }
    if (h != -1) {
        ins.set_capacity(std::max(wmax, (h * wsum) / (hmax + 1)));
    } else {
        ins.set_capacity(r * (1 - x) + wsum * x);
    }
    return ins;
}

Instance Generator::generate_spanner()
{
    std::vector<std::pair<Weight, Profit>> vec;
    for (ItemIdx j=0; j<v; ++j) {
        auto wp = item();
        Weight w = (2 * wp.first  - 1 + m) / m;
        Weight p = (2 * wp.second - 1 + m) / m;
        vec.push_back({w, p});
    }

    Instance ins;
    ItemIdx i = 0;
    std::uniform_int_distribution<Cpt> dist(1, m);
    Weight wsum = 0;
    Weight wmax = 0;
    while (ins.item_number() < n) {
        auto wp = vec[i];
        i = (i + 1) % v;
        ItemIdx a = dist(g);
        Weight w = a * wp.first;
        Profit p = a * wp.second;
        ins.add_item(w, p);
        wsum += wp.first;
        wmax = std::max(wmax, w);
    }
    if (h != -1) {
        ins.set_capacity(std::max(wmax, (h * wsum) / (hmax + 1)));
    } else {
        ins.set_capacity(r * (1 - x) + wsum * x);
    }
    return ins;
}

Instance Generator::generate()
{
    g.seed(s);
    return (spanner)? generate_spanner(): generate_standard();
}

