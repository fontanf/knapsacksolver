#include "../lib/tester.hpp"

TEST(Balknap, SimpleInstances)
{
	boost::filesystem::path p = boost::filesystem::current_path();
	p /= boost::filesystem::path("opt_balknap");
	p /= boost::filesystem::path("main");

	test(p.string() + " -a sopt_list_sorted",     "sopt");
	test(p.string() + " -a sopt_list_partsorted", "sopt");
	test(p.string() + " -a opt_list_sorted",       "opt");
	test(p.string() + " -a opt_list_partsorted",   "opt");
	test(p.string() + " -a sopt",                 "sopt");
	test(p.string() + " -a sopt",                  "opt");
}

