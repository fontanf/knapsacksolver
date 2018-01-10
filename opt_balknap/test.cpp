#include "../lib/tester.hpp"

TEST(Balknap, SimpleInstances)
{
	boost::filesystem::path p = boost::filesystem::current_path();
	p /= boost::filesystem::path("opt_balknap");
	p /= boost::filesystem::path("main");

	test(p.string() + " -a list", "opt");
	test(p.string() + " -a 1",   "sopt");
	test(p.string(),              "opt");
}

