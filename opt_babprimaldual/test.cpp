#include "../lib/tester.hpp"

TEST(BabPrimalDual, SimpleInstances)
{
	boost::filesystem::path p = boost::filesystem::current_path();
	p /= boost::filesystem::path("opt_babprimaldual");
	p /= boost::filesystem::path("main");
	test(p.string() + " -a sorted",     "sopt");
	test(p.string() + " -a partsorted", "sopt");
}

