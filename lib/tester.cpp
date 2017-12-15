#include "tester.hpp"

#include "instance.hpp"
#include "solution.hpp"

#include <thread>

void executeProgram(std::string cmd)
{
	std::cout << cmd << std::endl;
	system(cmd.c_str());
}

void check(std::string prog, boost::filesystem::path input)
{
	Instance instance(input);
	Profit opt = instance.optimum();

	std::string output_file = "output_file.ini";
	std::string cert_file   = "cert_file.txt";
	std::string cmd = prog
			+ " -i" + input.string()
			+ " -o" + output_file
			+ " -c" + cert_file;
	std::thread worker_rec(executeProgram, cmd);
	worker_rec.join();

	EXPECT_EQ(boost::filesystem::exists(output_file), true);
	if (!boost::filesystem::exists(output_file))
		return;

	EXPECT_EQ(boost::filesystem::exists(cert_file), true);
	if (!boost::filesystem::exists(cert_file))
		return;

	boost::property_tree::ptree pt;
	boost::property_tree::ini_parser::read_ini(output_file, pt);
	EXPECT_EQ(pt.get<Profit>("Solution.OPT"), opt);
	EXPECT_EQ(instance.check(cert_file),      opt);

	boost::filesystem::remove(output_file);
	boost::filesystem::remove(cert_file);
}

void test(std::string exec)
{
	boost::filesystem::path data_dir = boost::filesystem::current_path();
	data_dir /= boost::filesystem::path("test_instances");

	boost::filesystem::directory_iterator end_itr;
	for (boost::filesystem::directory_iterator itr(data_dir); itr != end_itr; ++itr) {
		if (itr->path().filename() == "FORMAT.txt")
			continue;
		if (itr->path().filename() == "BUILD")
			continue;
		check(exec, itr->path());
	}

}
