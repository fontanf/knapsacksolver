#include "greedy.hpp"
#include "../lib/instance.hpp"
#include "../lib/solution.hpp"

#include <gtest/gtest.h>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

TEST(ExtGreedy, 1)
{
	Instance instance(5, 8, {
			{0, 1, 1},
			{1, 2, 1},
			{2, 3, 1},
			{3, 4, 1},
			{4, 5, 1},
			});
	instance.sort_partially();
	Profit   lb  =  lb_extgreedy(instance);
	Solution sol = sol_extgreedy(instance);
	EXPECT_EQ(lb,           3);
	EXPECT_EQ(sol.profit(), 3);
}

TEST(ExtGreedy, 2)
{
	Instance instance(5, 8, {
			{0, 1, 2},
			{1, 2, 3},
			{2, 3, 4},
			{3, 4, 5},
			{4, 5, 6},
			});
	instance.sort_partially();
	Profit   lb  =  lb_extgreedy(instance);
	Solution sol = sol_extgreedy(instance);
	EXPECT_EQ(lb,           11);
	EXPECT_EQ(sol.profit(), 11);
}

