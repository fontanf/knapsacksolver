#include "../lib/tester.hpp"

TEST(DynamicProgrammingByReaching, SimpleInstances)
{
	boost::filesystem::path p = boost::filesystem::current_path();
	p /= boost::filesystem::path("opt_dpprofits");
	p /= boost::filesystem::path("main");

	test(p.string() + " -a 1", "sopt");
	test(p.string(),            "opt");
}

