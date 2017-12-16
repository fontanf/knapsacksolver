#include "../lib/tester.hpp"

TEST(BellmanRecursion, SimpleInstances)
{
	boost::filesystem::path p = boost::filesystem::current_path();
	p /= boost::filesystem::path("opt_bellman");
	p /= boost::filesystem::path("main");

	test(p.string() + " -a 1");
	test(p.string() + " -a 2");
	test(p.string() + " -a it");
	test(p.string() + " -a rec");
	test(p.string() + " -a stack");
	test(p.string() + " -a map");
}

