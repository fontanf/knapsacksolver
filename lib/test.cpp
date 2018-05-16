#include "knapsack/lib/instance.hpp"
#include "knapsack/lib/solution.hpp"

#include "knapsack/lb_greedy/greedy.hpp"

#include <gtest/gtest.h>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/regex.hpp>

using namespace knapsack;

TEST(Instance, Sort)
{
    Instance instance(5, 100);
    instance.add_items({
            {10, 10},
            {10, 15},
            {10, 5},
            {10, 12},
            {10, 20}});
    instance.sort();

    EXPECT_EQ(instance.item(4).p, 5);
    EXPECT_EQ(instance.item(3).p, 10);
    EXPECT_EQ(instance.item(2).p, 12);
    EXPECT_EQ(instance.item(1).p, 15);
    EXPECT_EQ(instance.item(0).p, 20);

    EXPECT_EQ(instance.item(4).j, 2);
    EXPECT_EQ(instance.item(3).j, 0);
    EXPECT_EQ(instance.item(2).j, 3);
    EXPECT_EQ(instance.item(1).j, 1);
    EXPECT_EQ(instance.item(0).j, 4);
}

TEST(Instance, SortPartially)
{
    Instance instance(9, 4); // break item b = 3 (1, 3)
    instance.add_items({
            {1, 1},
            {2, 1},
            {3, 1},
            {4, 1},
            {5, 1},
            {6, 1},
            {7, 1},
            {8, 1},
            {9, 1}});
    instance.sort_partially();
    EXPECT_EQ(instance.item(instance.break_item()).j, 2);
}

TEST(Instance, SortPartially2)
{
    Instance instance(9, 6);
    instance.add_items({ // break item b = 3 (1, 3)
            {1, 1},
            {9, 1},
            {2, 1},
            {8, 1},
            {3, 1},
            {7, 1},
            {4, 1},
            {5, 1},
            {6, 1}});
    instance.sort_partially();
    EXPECT_EQ(instance.item(instance.break_item()).j, 6);
}

TEST(Instance, SortPartially3)
{
    for (ItemIdx n = 0; n <= 1000; ++n) {
        std::vector<std::pair<Weight, Profit>> wp;
        Weight c = 0;
        for (ItemIdx i=0; i<n; ++i) {
            wp.push_back({i, 1});
            c += i;
        }
        c /= 2;
        c += 10;
        std::cout << "N " << n << " C " << c << std::endl;
        std::random_shuffle(wp.begin(), wp.end());

        Instance instance_eff(n, c);
        Instance instance_peff(n, c);
        instance_eff.add_items(wp);
        instance_peff.add_items(wp);
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

//TEST(Instance, ReductionSmallCoeff)
//{
    //auto data_dir = boost::filesystem::current_path();
    //data_dir /= "external";
    //data_dir /= "knapsack_instances_pisinger";
    //data_dir /= "smallcoeff";
    //const boost::regex my_filter("knapPI_.*_.*_1000");
    //boost::filesystem::directory_iterator end_itr;
    //for (boost::filesystem::directory_iterator i(data_dir); i != end_itr; ++i) {
        //std::cout << i->path() << std::endl;
        //boost::smatch what;
        //if(!boost::regex_match(i->path().filename().string(), what, my_filter))
            //continue;
        //for (boost::filesystem::directory_iterator ii(i->path()); ii != end_itr; ++ii) {
            //std::cout << "FILE " << ii->path() << std::endl;
            //assert(boost::filesystem::exists(ii->path()));
            //if (ii->path().filename() == "FORMAT.txt")
                //continue;
            //{
                //Instance instance(ii->path().string());
                //instance.sort_partially();
                //Solution sol = sol_greedy(instance);
                //instance.reduce1(sol.profit(), true);
            //}
            //{
                //Instance instance(ii->path().string());
                //instance.sort();
                //Solution sol = sol_greedy(instance);
                //instance.reduce2(sol.profit(), true);
            //}
        //}
    //}
//}

//TEST(Instance, ReductionLargeCoeff)
//{
    //auto data_dir = boost::filesystem::current_path();
    //data_dir /= "external";
    //data_dir /= "knapsack_instances_pisinger";
    //data_dir /= "largecoeff";
    //const boost::regex my_filter("knapPI_.*_50_.*");
    //boost::filesystem::directory_iterator end_itr;
    //for (boost::filesystem::directory_iterator i(data_dir); i != end_itr; ++i) {
        //std::cout << i->path() << std::endl;
        //boost::smatch what;
        //if(!boost::regex_match(i->path().filename().string(), what, my_filter))
            //continue;
        //for (boost::filesystem::directory_iterator ii(i->path()); ii != end_itr; ++ii) {
            //std::cout << "FILE " << ii->path() << std::endl;
            //assert(boost::filesystem::exists(ii->path()));
            //if (ii->path().filename() == "FORMAT.txt")
                //continue;
            //{
                //Instance instance(ii->path().string());
                //instance.sort_partially();
                //Solution sol = sol_greedy(instance);
                //instance.reduce1(sol.profit());
            //}
            //{
                //Instance instance(ii->path().string());
                //instance.sort();
                //Solution sol = sol_greedy(instance);
                //instance.reduce2(sol.profit());
            //}
        //}
    //}
//}

//TEST(Instance, ReductionHardInstances)
//{
    //auto data_dir = boost::filesystem::current_path();
    //data_dir /= "external";
    //data_dir /= "knapsack_instances_pisinger";
    //data_dir /= "hardinstances";
    //const boost::regex my_filter("knapPI_.*_100_.*");
    //boost::filesystem::directory_iterator end_itr;
    //for (boost::filesystem::directory_iterator i(data_dir); i != end_itr; ++i) {
        //std::cout << i->path() << std::endl;
        //boost::smatch what;
        //if(!boost::regex_match(i->path().filename().string(), what, my_filter))
            //continue;
        //for (boost::filesystem::directory_iterator ii(i->path()); ii != end_itr; ++ii) {
            //std::cout << "FILE " << ii->path() << std::endl;
            //assert(boost::filesystem::exists(ii->path()));
            //if (ii->path().filename() == "FORMAT.txt")
                //continue;
            //{
                //Instance instance(ii->path().string());
                //instance.sort_partially();
                //Solution sol = sol_greedy(instance);
                //instance.reduce1(sol.profit());
            //}
            //{
                //Instance instance(ii->path().string());
                //instance.sort();
                //Solution sol = sol_greedy(instance);
                //instance.reduce2(sol.profit());
            //}
        //}
    //}
//}
