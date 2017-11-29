#include "parser.hpp"

#include <gtest/gtest.h>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

TEST(DynamicProgramming, SimpleInstances)
{
	boost::filesystem::path path = boost::filesystem::current_path();
	path /= boost::filesystem::path("problems");
	path /= boost::filesystem::path("knapsack");
	path /= boost::filesystem::path("instances");
	path /= boost::filesystem::path("tests");
	path /= boost::filesystem::path("6.knapsack1.inst");

	KnapsackInstance instance(path);
	instance.sortDensity();

	EXPECT_EQ(instance.valueDensity(1), 5);
	EXPECT_EQ(instance.valueDensity(2), 10);
	EXPECT_EQ(instance.valueDensity(3), 12);
	EXPECT_EQ(instance.valueDensity(4), 15);
	EXPECT_EQ(instance.valueDensity(5), 20);

	EXPECT_EQ(instance.indexDensity(1), 3);
	EXPECT_EQ(instance.indexDensity(2), 1);
	EXPECT_EQ(instance.indexDensity(3), 4);
	EXPECT_EQ(instance.indexDensity(4), 2);
	EXPECT_EQ(instance.indexDensity(5), 5);

}
