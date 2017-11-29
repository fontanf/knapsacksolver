#include "../Lib/Parser.hpp"

#include <thread>
#include <gtest/gtest.h>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

void executeProgram(std::string cmd)
{
    system(cmd.c_str());
}

void check(boost::filesystem::path prog, boost::filesystem::path input)
{
	KnapsackInstance instance(input);
	Profit opt = instance.optimum();

	std::string output_file = "output_file.ini";
	std::string cert_file   = "cert_file.txt";
	std::string cmd = prog.string()
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

TEST(DynamicProgramming, SimpleInstances)
{
	boost::filesystem::path p = boost::filesystem::current_path();
	p /= boost::filesystem::path("DynamicProgramming");

	boost::filesystem::path rec  = p / "rec";
	boost::filesystem::path map  = p / "map";
	boost::filesystem::path it   = p / "it";
	boost::filesystem::path list = p / "list";

	boost::filesystem::path data_dir = boost::filesystem::current_path();
	data_dir /= boost::filesystem::path("TestInstances");

	boost::filesystem::directory_iterator end_itr;
	for (boost::filesystem::directory_iterator itr(data_dir); itr != end_itr; ++itr) {
		if (itr->path().filename() == "FORMAT.txt")
			continue;
		if (itr->path().filename() == "BUILD")
			continue;
		check(rec,  itr->path());
		check(map,  itr->path());
		check(it,   itr->path());
		check(list, itr->path());
	}
}

