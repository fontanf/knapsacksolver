#include "Instance.hpp"

#include <gtest/gtest.h>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

TEST(DynamicProgramming, SimpleInstances)
{
	boost::filesystem::path path = boost::filesystem::current_path();
	path /= boost::filesystem::path("TestInstances");
	path /= boost::filesystem::path("5_items.txt");

	Instance instance(path);
	Instance instance_sorted(instance);

	EXPECT_EQ(instance_sorted.profit(1), 5);
	EXPECT_EQ(instance_sorted.profit(2), 10);
	EXPECT_EQ(instance_sorted.profit(3), 12);
	EXPECT_EQ(instance_sorted.profit(4), 15);
	EXPECT_EQ(instance_sorted.profit(5), 20);

	EXPECT_EQ(instance_sorted.index(1), 3);
	EXPECT_EQ(instance_sorted.index(2), 1);
	EXPECT_EQ(instance_sorted.index(3), 4);
	EXPECT_EQ(instance_sorted.index(4), 2);
	EXPECT_EQ(instance_sorted.index(5), 5);

}
