#include "instance.hpp"
#include "solution.hpp"

#include "../lb_greedy/greedy.hpp"

#include <gtest/gtest.h>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/regex.hpp>

TEST(Instance, Sort)
{
    Instance instance({
            {0, 10, 10},
            {1, 10, 15},
            {2, 10, 5},
            {3, 10, 12},
            {4, 10, 20}}, 100);
    instance.sort();

    EXPECT_EQ(instance.item(4).p, 5);
    EXPECT_EQ(instance.item(3).p, 10);
    EXPECT_EQ(instance.item(2).p, 12);
    EXPECT_EQ(instance.item(1).p, 15);
    EXPECT_EQ(instance.item(0).p, 20);

    EXPECT_EQ(instance.item(4).i, 2);
    EXPECT_EQ(instance.item(3).i, 0);
    EXPECT_EQ(instance.item(2).i, 3);
    EXPECT_EQ(instance.item(1).i, 1);
    EXPECT_EQ(instance.item(0).i, 4);
}

TEST(Instance, SortPartially)
{
    Instance instance({ // break item b = 3 (1, 3)
            {0, 1, 1},
            {1, 2, 1},
            {2, 3, 1},
            {3, 4, 1},
            {4, 5, 1},
            {5, 6, 1},
            {6, 7, 1},
            {7, 8, 1},
            {8, 9, 1}}, 4);
    instance.sort_partially();
    EXPECT_EQ(instance.item(instance.break_item()).i, 2);
}

TEST(Instance, SortPartially2)
{
    Instance instance({ // break item b = 3 (1, 3)
            {0, 1, 1},
            {1, 9, 1},
            {2, 2, 1},
            {3, 8, 1},
            {4, 3, 1},
            {5, 7, 1},
            {6, 4, 1},
            {7, 5, 1},
            {8, 6, 1}}, 6);
    instance.sort_partially();
    EXPECT_EQ(instance.item(instance.break_item()).i, 6);
}

TEST(Instance, SortPartially3)
{
    for (ItemIdx n = 0; n <= 1000; ++n) {
        std::vector<Item> items;
        std::vector<Profit> p(n, 1);
        std::vector<Weight> w(n);
        iota(w.begin(), w.end(), 1);
        Weight c = 0;
        for (ItemIdx i=0; i<n; ++i) {
            items.push_back({i, i, 1});
            c += w[i];
        }
        c /= 2;
        c += 10;
        std::cout << "n " << n << " c " << c << std::endl;
        std::random_shuffle(w.begin(), w.end());

        Instance instance_eff(items, c);
        Instance instance_peff(items, c);
        instance_eff.sort();
        instance_peff.sort_partially();
        EXPECT_EQ(
                instance_eff.break_item(),
                instance_peff.break_item());
        EXPECT_EQ(
                instance_eff.break_profit(),
                instance_peff.break_profit());
        EXPECT_EQ(
                instance_eff.break_weight(),
                instance_peff.break_weight());
        EXPECT_EQ(
                instance_eff.break_capacity(),
                instance_peff.break_capacity());
    }
}

TEST(Instance, ReductionSmallCoeff)
{
    auto data_dir = boost::filesystem::current_path();
    data_dir /= "external";
    data_dir /= "knapsack_instances_pisinger";
    data_dir /= "smallcoeff";
    const boost::regex my_filter("knapPI_.*_.*_1000");
    boost::filesystem::directory_iterator end_itr;
    for (boost::filesystem::directory_iterator i(data_dir); i != end_itr; ++i) {
        std::cout << i->path() << std::endl;
        boost::smatch what;
        if(!boost::regex_match(i->path().filename().string(), what, my_filter))
            continue;
        for (boost::filesystem::directory_iterator ii(i->path()); ii != end_itr; ++ii) {
            std::cout << "FILE " << ii->path() << std::endl;
            assert(boost::filesystem::exists(ii->path()));
            if (ii->path().filename() == "FORMAT.txt")
                continue;
            {
                Instance instance(ii->path().string());
                instance.sort_partially();
                Solution sol = sol_greedy(instance);
                instance.reduce1(sol.profit(), true);
            }
            {
                Instance instance(ii->path().string());
                instance.sort();
                Solution sol = sol_greedy(instance);
                instance.reduce2(sol.profit(), true);
            }
        }
    }
}

TEST(Instance, ReductionLargeCoeff)
{
    auto data_dir = boost::filesystem::current_path();
    data_dir /= "external";
    data_dir /= "knapsack_instances_pisinger";
    data_dir /= "largecoeff";
    const boost::regex my_filter("knapPI_.*_50_.*");
    boost::filesystem::directory_iterator end_itr;
    for (boost::filesystem::directory_iterator i(data_dir); i != end_itr; ++i) {
        std::cout << i->path() << std::endl;
        boost::smatch what;
        if(!boost::regex_match(i->path().filename().string(), what, my_filter))
            continue;
        for (boost::filesystem::directory_iterator ii(i->path()); ii != end_itr; ++ii) {
            std::cout << "FILE " << ii->path() << std::endl;
            assert(boost::filesystem::exists(ii->path()));
            if (ii->path().filename() == "FORMAT.txt")
                continue;
            {
                Instance instance(ii->path().string());
                instance.sort_partially();
                Solution sol = sol_greedy(instance);
                instance.reduce1(sol.profit());
            }
            {
                Instance instance(ii->path().string());
                instance.sort();
                Solution sol = sol_greedy(instance);
                instance.reduce2(sol.profit());
            }
        }
    }
}

TEST(Instance, ReductionHardInstances)
{
    auto data_dir = boost::filesystem::current_path();
    data_dir /= "external";
    data_dir /= "knapsack_instances_pisinger";
    data_dir /= "hardinstances";
    const boost::regex my_filter("knapPI_.*_100_.*");
    boost::filesystem::directory_iterator end_itr;
    for (boost::filesystem::directory_iterator i(data_dir); i != end_itr; ++i) {
        std::cout << i->path() << std::endl;
        boost::smatch what;
        if(!boost::regex_match(i->path().filename().string(), what, my_filter))
            continue;
        for (boost::filesystem::directory_iterator ii(i->path()); ii != end_itr; ++ii) {
            std::cout << "FILE " << ii->path() << std::endl;
            assert(boost::filesystem::exists(ii->path()));
            if (ii->path().filename() == "FORMAT.txt")
                continue;
            {
                Instance instance(ii->path().string());
                instance.sort_partially();
                Solution sol = sol_greedy(instance);
                instance.reduce1(sol.profit());
            }
            {
                Instance instance(ii->path().string());
                instance.sort();
                Solution sol = sol_greedy(instance);
                instance.reduce2(sol.profit());
            }
        }
    }
}
