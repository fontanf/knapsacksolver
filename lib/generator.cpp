#include "knapsack/lib/generator.hpp"

#include "knapsack/lib/instance.hpp"

#include <random>

#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/filter/zlib.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include <boost/iostreams/filter/bzip2.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/serialization/vector.hpp>

using namespace knapsack;

void write_format_file(boost::filesystem::path dest, std::string str)
{
    boost::filesystem::create_directories(dest);
    boost::filesystem::path format = dest / "FORMAT.txt";
    std::ofstream f;
    f.open(format.string());
    f << str << std::endl;
    f.close();
}

class DistributionType
{
public:
    virtual std::pair<Weight, Profit> item() = 0;
};

std::pair<Weight, Profit> item(GenerateData& data)
{
    Weight w = -1;
    Profit p = -1;
    if (data.type == "u") {
        std::uniform_int_distribution<int> d(1,data.r);
        w = d(data.g);
        p = d(data.g);
    } else if (data.type == "wc") {
        std::uniform_int_distribution<int> d1(1, data.r);
        std::uniform_int_distribution<int> d2(-(data.r+1)/10, data.r/10);
        w = d1(data.g);
        do {
            p = w + d2(data.g);
        } while (p <= 0);
    } else if (data.type == "sc") {
        std::uniform_int_distribution<int> d(1, data.r);
        w = d(data.g);
        p = w + data.r/10;
    } else if (data.type == "isc") {
        std::uniform_int_distribution<int> d(1, data.r);
        p = d(data.g);
        w = p + data.r/10;
    } else if (data.type == "asc") {
        std::uniform_int_distribution<int> d1(1,data.r);
        std::uniform_int_distribution<int> d2(-(data.r+1)/500, data.r/500);
        w = d1(data.g);
        p = w + data.r/10 + d2(data.g);
    } else if (data.type == "ss") {
        std::uniform_int_distribution<int> d(1, data.r);
        w = d(data.g);
        p = w;
    } else if (data.type == "sw") {
        std::uniform_int_distribution<int> d1(100000, 100100);
        std::uniform_int_distribution<int> d2(1, 1000);
        w = d1(data.g);
        p = d2(data.g);
    } else {
        assert(false);
    }
    return {w, p};
}

Instance generate_non_spanner(GenerateData& data)
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
        Weight w = (2*wp.first-1) / data.m + 1;
        Weight p = (2*wp.second-1) / data.m + 1;
        vec.push_back({w, p});
    }

    Instance ins(data.n, 0);
    ItemIdx i = 0;
    std::uniform_int_distribution<int> dist(1, data.m);
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
    ins.set_capacity(std::max(wmax, (data.h * wsum) / 100));
    return ins;
}

Instance knapsack::generate(GenerateData data)
{
    data.g.seed(data.seed);
    if (data.spanner) {
        return generate_spanner(data);
    } else {
        return generate_non_spanner(data);
    }
}

