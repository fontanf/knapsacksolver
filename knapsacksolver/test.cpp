#include "knapsacksolver/solution.hpp"

#include "knapsacksolver/algorithms/greedy.hpp"

#include <gtest/gtest.h>

using namespace knapsacksolver;

TEST(Instance, Sort)
{
    Instance instancetance(100, {
            {10, 10},
            {10, 15},
            {10, 5},
            {10, 12},
            {10, 20}});
    Info info;
    instancetance.sort(FFOT_DBG(info));

    EXPECT_EQ(instancetance.item(4).p, 5);
    EXPECT_EQ(instancetance.item(3).p, 10);
    EXPECT_EQ(instancetance.item(2).p, 12);
    EXPECT_EQ(instancetance.item(1).p, 15);
    EXPECT_EQ(instancetance.item(0).p, 20);

    EXPECT_EQ(instancetance.item(4).j, 2);
    EXPECT_EQ(instancetance.item(3).j, 0);
    EXPECT_EQ(instancetance.item(2).j, 3);
    EXPECT_EQ(instancetance.item(1).j, 1);
    EXPECT_EQ(instancetance.item(0).j, 4);
}

TEST(Instance, SortPartially)
{
    Instance instancetance(4, {
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
    instancetance.sort_partially(FFOT_DBG(info));
    EXPECT_EQ(instancetance.item(instancetance.break_item()).j, 2);
}

TEST(Instance, SortPartially2)
{
    Instance instancetance(5, { // break item b = 3 (1, 3)
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
    instancetance.sort_partially(FFOT_DBG(info));
    EXPECT_EQ(instancetance.item(instancetance.break_item()).j, 4);
}

TEST(Instance, SortPartially3)
{
    std::mt19937_64 g(0);
    for (Counter n = 1; n <= 1000; ++n) {
        std::uniform_int_distribution<int> d1(1, n);
        for (Counter h = 1; h < 100; ++h) {
            Weight wmax = d1(g);
            std::uniform_int_distribution<int> d2(1, wmax);
            Instance instance;
            Weight wsum = 0;
            for (ItemIdx j = 0; j < n; ++j) {
                Weight w = d2(g);
                Profit p = d2(g);
                instance.add_item(w, p);
                wsum += w;
            }
            instance.set_capacity(std::max(wmax, (h * wsum) / 100));

            std::cout << "n " << instance.number_of_items()
                << " c " << instance.capacity() << std::endl;
            std::cout << "{";
            for (ItemIdx j = 0; j < instance.number_of_items(); ++j) {
                std::cout << "{" << instance.item(j).w << "," << instance.item(j).p << "}";
                if (j != instance.number_of_items() - 1)
                    std::cout << ", ";
            }
            std::cout << "}" << std::endl;

            Info info_tmp;
            instance.sort_partially(FFOT_DBG(info_tmp FFOT_COMMA) 2);

            Solution sol(instance);
            ItemPos b = instance.break_item();

            if (b == instance.number_of_items()) {
                continue;
            }

            if (instance.break_solution()->remaining_capacity() == 0) {
                continue;
            }

            Weight wb = instance.item(b).w;
            Profit pb = instance.item(b).p;

            std::cout << "b " << b << " wb " << wb << " pb " << pb << std::endl;
            for (ItemPos j=0; j<instance.break_item(); ++j) {
                EXPECT_GE(instance.item(j).p*wb, instance.item(j).w*pb);
                if (instance.item(j).p*wb < instance.item(j).w*pb) {
                    std::cout << instance << std::endl;
                    return;
                }
            }
            for (ItemPos j=instance.break_item(); j<instance.number_of_items(); ++j) {
                EXPECT_LE(instance.item(j).p*wb, instance.item(j).w*pb);
                if (instance.item(j).p*wb > instance.item(j).w*pb) {
                    std::cout << instance << std::endl;
                    return;
                }
            }

            EXPECT_LE(instance.break_solution()->weight(), instance.capacity());
            if (instance.break_solution()->weight() > instance.capacity()){
                std::cout << instance << std::endl;
                return;
            }

            EXPECT_GT(instance.break_solution()->weight() + instance.item(b).w,
                    instance.capacity());
            if (instance.break_solution()->weight() + instance.item(b).w
                    <= instance.capacity()) {
                std::cout << "w_bar " << instance.break_solution()->weight() << std::endl;
                std::cout << "w_b   " << instance.item(b).w << std::endl;
                std::cout << instance << std::endl;
                return ;
            }

        }
    }
}

