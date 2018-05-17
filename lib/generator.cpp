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

Instance generate_uncorrelated(ItemIdx n, Profit r, int h)
{
    Instance ins(n, 0);
    std::default_random_engine generator;
    std::uniform_int_distribution<int> distribution(1,r);
    Weight wsum = 0;
    Weight wmax = 0;
    for (ItemIdx j=0; j<n; ++j) {
        Weight w = distribution(generator);
        Profit p = distribution(generator);
        ins.add_item(w, p);
        wsum += w;
    }
    ins.set_capacity(std::max(wmax, (h * wsum) / 100));
    return ins;
}

Instance generate_weakly_correlated(ItemIdx n, Profit r, int h)
{
    Instance ins(n, 0);
    std::default_random_engine generator;
    std::uniform_int_distribution<int> d1(1, r);
    std::uniform_int_distribution<int> d2(-(r+1)/10, r/10);
    Weight wsum = 0;
    Weight wmax = 0;
    for (ItemIdx j=0; j<n; ++j) {
        Weight w = d1(generator);
        Profit p = w + d2(generator);
        ins.add_item(w, p);
        wsum += w;
        wmax = std::max(wmax, w);
    }
    ins.set_capacity(std::max(wmax, (h * wsum) / 100));
    return ins;
}

Instance generate_strongly_correlated(ItemIdx n, Profit r, int h)
{
    Instance ins(n, 0);
    std::default_random_engine generator;
    std::uniform_int_distribution<int> d1(1, r);
    Weight wsum = 0;
    Weight wmax = 0;
    for (ItemIdx j=0; j<n; ++j) {
        Weight w = d1(generator);
        Profit p = w + r/10;
        ins.add_item(w, p);
        wsum += w;
        wmax = std::max(wmax, w);
    }
    ins.set_capacity(std::max(wmax, (h * wsum) / 100));
    return ins;
}

Instance generate_inverse_strongly_correlated(ItemIdx n, Profit r, int h)
{
    Instance ins(n, 0);
    std::default_random_engine generator;
    std::uniform_int_distribution<int> d1(1, r);
    Weight wsum = 0;
    Weight wmax = 0;
    for (ItemIdx j=0; j<n; ++j) {
        Profit p = d1(generator);
        Weight w = p + r/10;
        ins.add_item(w, p);
        wsum += w;
        wmax = std::max(wmax, w);
    }
    ins.set_capacity(std::max(wmax, (h * wsum) / 100));
    return ins;
}

Instance generate_almost_strongly_correlated(ItemIdx n, Profit r, int h)
{
    Instance ins(n, 0);
    std::default_random_engine generator;
    std::uniform_int_distribution<int> d1(1, r);
    std::uniform_int_distribution<int> d2(-(r+1)/500, r/500);
    Weight wsum = 0;
    Weight wmax = 0;
    for (ItemIdx j=0; j<n; ++j) {
        Weight w = d1(generator);
        Profit p = w + r/10 + d2(generator);
        ins.add_item(w, p);
        wsum += w;
        wmax = std::max(wmax, w);
    }
    ins.set_capacity(std::max(wmax, (h * wsum) / 100));
    return ins;
}

Instance generate_subset_sum(ItemIdx n, Profit r, int h)
{
    Instance ins(n, 0);
    std::default_random_engine generator;
    std::uniform_int_distribution<int> d1(1, r);
    Weight wsum = 0;
    Weight wmax = 0;
    for (ItemIdx j=0; j<n; ++j) {
        Weight w = d1(generator);
        ins.add_item(w, w);
        wsum += w;
        wmax = std::max(wmax, w);
    }
    ins.set_capacity(std::max(wmax, (h * wsum) / 100));
    return ins;
}

Instance generate_similar_weights(ItemIdx n, Profit r, int h)
{
    Instance ins(n, 0);
    std::default_random_engine generator;
    std::uniform_int_distribution<int> d1(r, r+100);
    std::uniform_int_distribution<int> d2(1, 1000);
    Weight wsum = 0;
    Weight wmax = 0;
    for (ItemIdx j=0; j<n; ++j) {
        Weight w = d1(generator);
        Profit p = d2(generator);
        ins.add_item(w, p);
        wsum += w;
        wmax = std::max(wmax, w);
    }
    ins.set_capacity(std::max(wmax, (h * wsum) / 100));
    return ins;
}

Instance knapsack::generate(std::string type, ItemIdx n, Profit r, int h)
{
    if (type == "u") {
        return generate_uncorrelated(n, r, h);
    } else if (type == "wc") {
        return generate_weakly_correlated(n, r, h);
    } else if (type == "sc") {
        return generate_strongly_correlated(n, r, h);
    } else if (type == "isc") {
        return generate_inverse_strongly_correlated(n, r, h);
    } else if (type == "asc") {
        return generate_almost_strongly_correlated(n, r, h);
    } else if (type == "ss") {
        return generate_subset_sum(n, r, h);
    } else if (type == "sw") {
        return generate_similar_weights(n, r, h);
    } else {
        assert(false);
        return generate_uncorrelated(n, r, h);
    }
}

int main(int argc, char *argv[])
{
    namespace po = boost::program_options;

    // Parse program options
    std::string type = ""; // "u", "wc", "sc", "isc", "asc", "ss", "sw"
    ItemIdx n = -1;
    Profit  r = -1;
    int     h = -1;
    std::string output_file = "";
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "produce help message")
        (",t", po::value<std::string>(&type)->required(), "set instance type")
        (",n", po::value<ItemIdx>(&n)->required(), "size")
        (",r", po::value<Profit>(&r), "set R")
        (",h", po::value<Profit>(&r), "set h")
        (",o", po::value<std::string>(&output_file)->required(), "set output file")
        ("verbose,v",  "enable verbosity")
        ;
    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    if (vm.count("help")) {
        std::cout << desc << std::endl;;
        return 1;
    }
    try {
        po::notify(vm);
    } catch (po::required_option e) {
        std::cout << desc << std::endl;;
        return 1;
    }

    Instance ins = generate(type, n, r, h);

    return 0;
}
