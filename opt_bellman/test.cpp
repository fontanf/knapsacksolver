#include "../lib/tester.hpp"

TEST(BellmanRecursion, SimpleInstances)
{
	boost::filesystem::path p = boost::filesystem::current_path();
	p /= boost::filesystem::path("opt_bellman");
	p /= boost::filesystem::path("main");

	test(p.string() + " -a reclist", "sopt");
	test(p.string() + " -a list",     "opt");
	test(p.string() + " -a rec",     "sopt");
	test(p.string() + " -a 2",       "sopt");
	test(p.string() + " -a 1",       "sopt");
	test(p.string() + " -a 1it",     "sopt");
	test(p.string() + " -a 1rec",    "sopt");
	test(p.string() + " -a 1stack",  "sopt");
	test(p.string() + " -a 1map",    "sopt");
	test(p.string(),                  "opt");
}

