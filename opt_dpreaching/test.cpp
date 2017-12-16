#include "../lib/tester.hpp"

TEST(DynamicProgramming, SimpleInstances)
{
	boost::filesystem::path p = boost::filesystem::current_path();
	p /= boost::filesystem::path("opt_dpreaching");
	p /= boost::filesystem::path("main");

	test(p.string() + " -a 1");
}

