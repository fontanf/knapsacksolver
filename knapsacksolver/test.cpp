#include "knapsacksolver/instance.hpp"
#include "knapsacksolver/solution.hpp"

#include "knapsacksolver/algorithms/greedy.hpp"

#include <gtest/gtest.h>

using namespace knapsacksolver;

TEST(Instance, Sort)
{
    Instance instance(100, {
            {10, 10},
            {10, 15},
            {10, 5},
            {10, 12},
            {10, 20}});
    Info info;
    instance.sort(info);

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
    Instance instance(4, {
            {1, 1},
            {2, 1},
            {3, 1},
            {4, 1},
            {5, 1},
            {6, 1},
            {7, 1},
            {8, 1},
            {9, 1}});
    Info info;
    instance.sort_partially(info);
    EXPECT_EQ(instance.item(instance.break_item()).j, 2);
}

TEST(Instance, SortPartially2)
{
    Instance instance(5, { // break item b = 3 (1, 3)
            {1, 1},
            {9, 1},
            {2, 1},
            {8, 1},
            {3, 1},
            {7, 1},
            {4, 1},
            {5, 1},
            {6, 1}});
    Info info;
    instance.sort_partially(info);
    EXPECT_EQ(instance.item(instance.break_item()).j, 4);
}

TEST(Instance, SortPartially3)
{
    std::mt19937_64 g(0);
    for (Counter n = 1; n <= 1000; ++n) {
        std::uniform_int_distribution<int> d1(1, n);
        for (Counter h = 1; h < 100; ++h) {
            Weight wmax = d1(g);
            std::uniform_int_distribution<int> d2(1, wmax);
            Instance ins;
            Weight wsum = 0;
            for (ItemIdx j = 0; j < n; ++j) {
                Weight w = d2(g);
                Profit p = d2(g);
                ins.add_item(w, p);
                wsum += w;
            }
            ins.set_capacity(std::max(wmax, (h * wsum) / 100));

            std::cout << "n " << ins.item_number()
                << " c " << ins.capacity() << std::endl;
            std::cout << "{";
            for (ItemIdx j = 0; j < ins.item_number(); ++j) {
                std::cout << "{" << ins.item(j).w << "," << ins.item(j).p << "}";
                if (j != ins.item_number() - 1)
                    std::cout << ", ";
            }
            std::cout << "}" << std::endl;

            Info info_tmp;
            ins.sort_partially(info_tmp, 2);

            Solution sol(ins);
            ItemPos b = ins.break_item();

            if (b == ins.item_number()) {
                continue;
            }

            if (ins.break_solution()->remaining_capacity() == 0) {
                continue;
            }

            Weight wb = ins.item(b).w;
            Profit pb = ins.item(b).p;

            std::cout << "b " << b << " wb " << wb << " pb " << pb << std::endl;
            for (ItemPos j=0; j<ins.break_item(); ++j) {
                EXPECT_GE(ins.item(j).p*wb, ins.item(j).w*pb);
                if (ins.item(j).p*wb < ins.item(j).w*pb) {
                    std::cout << ins << std::endl;
                    return;
                }
            }
            for (ItemPos j=ins.break_item(); j<ins.item_number(); ++j) {
                EXPECT_LE(ins.item(j).p*wb, ins.item(j).w*pb);
                if (ins.item(j).p*wb > ins.item(j).w*pb) {
                    std::cout << ins << std::endl;
                    return;
                }
            }

            EXPECT_LE(ins.break_solution()->weight(), ins.capacity());
            if (ins.break_solution()->weight() > ins.capacity()){
                std::cout << ins << std::endl;
                return;
            }

            EXPECT_GT(ins.break_solution()->weight() + ins.item(b).w,
                    ins.capacity());
            if (ins.break_solution()->weight() + ins.item(b).w
                    <= ins.capacity()) {
                std::cout << "w_bar " << ins.break_solution()->weight() << std::endl;
                std::cout << "w_b   " << ins.item(b).w << std::endl;
                std::cout << ins << std::endl;
                return ;
            }

        }
    }
}

