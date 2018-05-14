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

void write_compressed_file(boost::filesystem::path filepath, std::stringstream& data)
{
    std::ofstream file;
    file.open(filepath.string());

    boost::iostreams::filtering_streambuf<boost::iostreams::input> in;
    in.push(boost::iostreams::bzip2_compressor());
    in.push(data);
    boost::iostreams::copy(in, file);
}

void write_format_file(boost::filesystem::path dest, std::string str)
{
    boost::filesystem::create_directories(dest);
    boost::filesystem::path format = dest / "FORMAT.txt";
    std::ofstream f;
    f.open(format.string());
    f << str << std::endl;
    f.close();
}

void generate_uncorrelated(boost::filesystem::path dir,
        const std::vector<ItemIdx>& ns, const std::vector<Profit>& rs)
{
    ItemIdx hmax = 100;

    for (ItemIdx n: ns) {
        std::vector<Item> items(n);
        for (Profit r: rs) {
            boost::filesystem::path dest = dir
                / "u" / std::to_string(n) / std::to_string(r);
            boost::filesystem::create_directories(dest);
            write_format_file(dest, "knapsack_standard");

            std::default_random_engine generator;
            std::uniform_int_distribution<int> distribution(1,r);
            for (ItemIdx h=1; h<=hmax; ++h) {
                Weight wsum = 0;
                for (ItemIdx j=0; j<n; ++j) {
                    items[j].w = distribution(generator);
                    items[j].p = distribution(generator);
                    wsum += items[j].w;
                }
                Weight c = (h * wsum) / hmax;

                std::stringstream data;
                data << n << " " << c << std::endl << std::endl;
                for (ItemIdx j=0; j<n; ++j)
                    data << items[j].p << " " << items[j].w << std::endl;

                std::string filename = std::to_string(h) + ".txt.bz2";
                boost::filesystem::path filepath = dest / filename;
                write_compressed_file(filepath, data);
            }
        }
    }
}

void generate_weakly_correlated(boost::filesystem::path dir,
        const std::vector<ItemIdx>& ns, const std::vector<Profit>& rs)
{
    ItemIdx hmax = 100;

    for (ItemIdx n: ns) {
        std::vector<Item> items(n);
        for (Profit r: rs) {
            boost::filesystem::path dest = dir
                / "wc" / std::to_string(n) / std::to_string(r);
            boost::filesystem::create_directories(dest);
            write_format_file(dest, "knapsack_standard");

            std::default_random_engine generator;
            std::uniform_int_distribution<int> d1(1, r);
            std::uniform_int_distribution<int> d2(-(r+1)/10, r/10);
            for (ItemIdx h=1; h<=hmax; ++h) {
                Weight wsum = 0;
                for (ItemIdx j=0; j<n; ++j) {
                    items[j].w = d1(generator);
                    items[j].p = items[j].w + d2(generator);
                    wsum += items[j].w;
                }
                Weight c = (h * wsum) / hmax;

                std::stringstream data;
                data << n << " " << c << std::endl << std::endl;
                for (ItemIdx j=0; j<n; ++j)
                    data << items[j].p << " " << items[j].w << std::endl;

                std::string filename = std::to_string(h) + ".txt.bz2";
                boost::filesystem::path filepath = dest / filename;
                write_compressed_file(filepath, data);
            }
        }
    }
}

void generate_strongly_correlated(boost::filesystem::path dir,
        const std::vector<ItemIdx>& ns, const std::vector<Profit>& rs)
{
    ItemIdx hmax = 100;

    for (ItemIdx n: ns) {
        std::vector<Item> items(n);
        for (Profit r: rs) {
            boost::filesystem::path dest = dir
                / "sc" / std::to_string(n) / std::to_string(r);
            boost::filesystem::create_directories(dest);
            write_format_file(dest, "knapsack_standard");

            std::default_random_engine generator;
            std::uniform_int_distribution<int> d1(1, r);
            for (ItemIdx h=1; h<=hmax; ++h) {
                Weight wsum = 0;
                for (ItemIdx j=0; j<n; ++j) {
                    items[j].w = d1(generator);
                    items[j].p = items[j].w + r/10;
                    wsum += items[j].w;
                }
                Weight c = (h * wsum) / hmax;

                std::stringstream data;
                data << n << " " << c << std::endl << std::endl;
                for (ItemIdx j=0; j<n; ++j)
                    data << items[j].p << " " << items[j].w << std::endl;

                std::string filename = std::to_string(h) + ".txt.bz2";
                boost::filesystem::path filepath = dest / filename;
                write_compressed_file(filepath, data);
            }
        }
    }
}

void generate_inverse_strongly_correlated(boost::filesystem::path dir,
        const std::vector<ItemIdx>& ns, const std::vector<Profit>& rs)
{
    ItemIdx hmax = 100;

    for (ItemIdx n: ns) {
        std::vector<Item> items(n);
        for (Profit r: rs) {
            boost::filesystem::path dest = dir
                / "isc" / std::to_string(n) / std::to_string(r);
            boost::filesystem::create_directories(dest);
            write_format_file(dest, "knapsack_standard");

            std::default_random_engine generator;
            std::uniform_int_distribution<int> d1(1, r);
            for (ItemIdx h=1; h<=hmax; ++h) {
                Weight wsum = 0;
                for (ItemIdx j=0; j<n; ++j) {
                    items[j].p = d1(generator);
                    items[j].w = items[j].p + r/10;
                    wsum += items[j].w;
                }
                Weight c = (h * wsum) / hmax;

                std::stringstream data;
                data << n << " " << c << std::endl << std::endl;
                for (ItemIdx j=0; j<n; ++j)
                    data << items[j].p << " " << items[j].w << std::endl;

                std::string filename = std::to_string(h) + ".txt.bz2";
                boost::filesystem::path filepath = dest / filename;
                write_compressed_file(filepath, data);
            }
        }
    }
}

void generate_almost_strongly_correlated(boost::filesystem::path dir,
        const std::vector<ItemIdx>& ns, const std::vector<Profit>& rs)
{
    ItemIdx hmax = 100;

    for (ItemIdx n: ns) {
        std::vector<Item> items(n);
        for (Profit r: rs) {
            boost::filesystem::path dest = dir
                / "asc" / std::to_string(n) / std::to_string(r);
            boost::filesystem::create_directories(dest);
            write_format_file(dest, "knapsack_standard");

            std::default_random_engine generator;
            std::uniform_int_distribution<int> d1(1, r);
            std::uniform_int_distribution<int> d2(-(r+1)/500, r/500);
            for (ItemIdx h=1; h<=hmax; ++h) {
                Weight wsum = 0;
                for (ItemIdx j=0; j<n; ++j) {
                    items[j].w = d1(generator);
                    items[j].p = items[j].w + r/10 + d2(generator);
                    wsum += items[j].w;
                }
                Weight c = (h * wsum) / hmax;

                std::stringstream data;
                data << n << " " << c << std::endl << std::endl;
                for (ItemIdx j=0; j<n; ++j)
                    data << items[j].p << " " << items[j].w << std::endl;

                std::string filename = std::to_string(h) + ".txt.bz2";
                boost::filesystem::path filepath = dest / filename;
                write_compressed_file(filepath, data);
            }
        }
    }
}

void generate_subset_sum(boost::filesystem::path dir,
        const std::vector<ItemIdx>& ns, const std::vector<Profit>& rs)
{
    ItemIdx hmax = 100;

    for (ItemIdx n: ns) {
        std::vector<Weight> weights(n);
        for (Profit r: rs) {
            boost::filesystem::path dest = dir
                / "ss" / std::to_string(n) / std::to_string(r);
            boost::filesystem::create_directories(dest);
            write_format_file(dest, "subsetsum_standard");

            std::default_random_engine generator;
            std::uniform_int_distribution<int> d1(1, r);
            for (ItemIdx h=1; h<=hmax; ++h) {
                Weight wsum = 0;
                for (ItemIdx j=0; j<n; ++j) {
                    weights[j] = d1(generator);
                    wsum += weights[j];
                }
                Weight c = (h * wsum) / hmax;

                std::stringstream data;
                data << n << " " << c << std::endl << std::endl;
                for (ItemIdx j=0; j<n; ++j)
                    data << weights[j] << std::endl;

                std::string filename = std::to_string(h) + ".txt.bz2";
                boost::filesystem::path filepath = dest / filename;
                write_compressed_file(filepath, data);
            }
        }
    }
}

void generate_similar_weights(boost::filesystem::path dir,
        const std::vector<ItemIdx> ns)
{
    ItemIdx hmax = 100;

    for (ItemIdx n: ns) {
        boost::filesystem::path dest = dir
            / "sw" / std::to_string(n);
        boost::filesystem::create_directories(dest);
        write_format_file(dest, "knapsack_standard");

        std::vector<Item> items(n);
        std::default_random_engine generator;
        std::uniform_int_distribution<int> d1(100000, 100100);
        std::uniform_int_distribution<int> d2(1, 1000);
        for (ItemIdx h=1; h<=hmax; ++h) {
            Weight wsum = 0;
            for (ItemIdx j=0; j<n; ++j) {
                items[j].w = d1(generator);
                items[j].p = d2(generator);
                wsum += items[j].w;
            }
            Weight c = (h * wsum) / hmax;

            std::stringstream data;
            data << n << " " << c << std::endl << std::endl;
            for (ItemIdx j=0; j<n; ++j)
                data << items[j].p << " " << items[j].w << std::endl;

            std::string filename = std::to_string(h) + ".txt.bz2";
            boost::filesystem::path filepath = dest / filename;
            write_compressed_file(filepath, data);
        }
    }
}

void generate_all(boost::filesystem::path dir)
{
    std::vector<ItemIdx> ns {
            10, 20, 50,
            100, 200, 500,
            1000, 2000, 5000,
            10000, 20000, 50000,
            100000
    };

    std::vector<Profit> rs {
            10,
            100,
            1000,
            10000,
            100000,
            1000000,
            10000000,
            100000000,
    };

    generate_uncorrelated(dir, ns, rs);
    generate_weakly_correlated(dir, ns, rs);
    generate_strongly_correlated(dir, ns, rs);
    generate_inverse_strongly_correlated(dir, ns, rs);
    generate_almost_strongly_correlated(dir, ns, rs);
    generate_subset_sum(dir, ns, rs);
    generate_similar_weights(dir, ns);
}

int main(int argc, char *argv[])
{
    boost::filesystem::path dir("./data_pisinger");
    generate_all(dir);
    return 0;
}
