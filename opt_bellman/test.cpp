#include "../lib/tester.hpp"

TEST(DynamicProgramming, SimpleInstances)
{
	boost::filesystem::path p = boost::filesystem::current_path();
	p /= boost::filesystem::path("opt_bellman");

	test(p / "bellman_1");
	test(p / "bellman_2");
	test(p / "it");
	test(p / "rec");
	test(p / "stack");
	test(p / "map");
}

