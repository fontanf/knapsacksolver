#include "../lib/tester.hpp"

TEST(DynamicProgrammingByReaching, SimpleInstances)
{
	boost::filesystem::path p = boost::filesystem::current_path();
	p /= boost::filesystem::path("opt_dpprofits");
	p /= boost::filesystem::path("main");

	test(p.string() + " -a sopt",        "sopt");
	test(p.string() + " -a opt",          "opt");
}

