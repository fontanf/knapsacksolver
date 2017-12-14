#include "instance.hpp"
#include "solution.hpp"

#include <gtest/gtest.h>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

TEST(Instance, SortByEfficiency)
{
	Instance instance(5, 100, {10, 15, 5, 12, 20}, {10, 10, 10, 10, 10});
	Instance instance_sorted = Instance::sort_by_efficiency(instance);

	EXPECT_EQ(instance_sorted.profit(5), 5);
	EXPECT_EQ(instance_sorted.profit(4), 10);
	EXPECT_EQ(instance_sorted.profit(3), 12);
	EXPECT_EQ(instance_sorted.profit(2), 15);
	EXPECT_EQ(instance_sorted.profit(1), 20);

	EXPECT_EQ(instance_sorted.index(5), 3);
	EXPECT_EQ(instance_sorted.index(4), 1);
	EXPECT_EQ(instance_sorted.index(3), 4);
	EXPECT_EQ(instance_sorted.index(2), 2);
	EXPECT_EQ(instance_sorted.index(1), 5);
}

TEST(Instance, SortPartiallyByEfficiency1)
{
	Instance instance(9, 4,
			{1, 1, 1, 1, 1, 1, 1, 1, 1},
			{1, 2, 3, 4, 5, 6, 7, 8, 9});
	// break item b = 3 (1, 3)
	Instance instance_sorted = Instance::sort_partially_by_efficiency(instance);

	ItemIdx b = 0;
	Weight r = instance_sorted.capacity();
	for (ItemIdx i=1; i<=instance_sorted.item_number(); ++i) {
		Weight wi = instance_sorted.weight(i);
		if (wi > r) {
			b = i;
			break;
		}
		r -= wi;
	}
	EXPECT_EQ(instance_sorted.index_orig(b), 3);
}

TEST(Instance, SortPartiallyByEfficiency2)
{
	Instance instance(9, 6,
			{1, 1, 1, 1, 1, 1, 1, 1, 1},
			{1, 9, 2, 8, 3, 7, 4, 5, 6});
	// break item b = 3 (1, 3)
	Instance instance_sorted = Instance::sort_partially_by_efficiency(instance);
	std::cout << instance_sorted << std::endl;

	ItemIdx b = 0;
	Weight r = instance_sorted.capacity();
	for (ItemIdx i=1; i<=instance_sorted.item_number(); ++i) {
		Weight wi = instance_sorted.weight(i);
		if (wi > r) {
			b = i;
			break;
		}
		r -= wi;
	}
	EXPECT_EQ(instance_sorted.index_orig(b), 7);
}

TEST(Instance, SortPartiallyByEfficiency3)
{
	for (ItemIdx n = 0; n <= 1000; ++n) {
		Weight c = 64 * n;
		std::vector<Profit> p(n, 1);
		std::vector<Weight> w(n);
		iota(w.begin(), w.end(), 1);
		std::random_shuffle(w.begin(), w.end());

		Instance instance(n, c, p, w);
		Instance instance_eff  = Instance::sort_by_efficiency(instance);
		Instance instance_peff = Instance::sort_partially_by_efficiency(instance);

		Weight r   = c;
		Weight rp  = c;
		Profit pr  = 0;
		Profit prp = 0;
		for (ItemIdx i=1; i<=n; ++i) {
			Weight wi  = instance_eff.weight(i);
			Weight wip = instance_peff.weight(i);
			if (wi > r) {
				std::cout << i << std::endl;
				EXPECT_GT(wip, rp);
				EXPECT_EQ(pr, prp);
				break;
			}
			pr  += instance_eff.profit(i);
			prp += instance_peff.profit(i);
			r   -= wi;
			rp  -= wip;
		}
	}
}
