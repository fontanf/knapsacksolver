#include "../lib/tester.hpp"

TEST(BranchAndBound, SimpleInstances)
{
	boost::filesystem::path p = boost::filesystem::current_path();
	p /= boost::filesystem::path("opt_bab");
	p /= boost::filesystem::path("main");

	test(p.string() + " -a rec");
	test(p.string() + " -a stack");
}

