#include "Parser.hpp"

#include <sstream>

Instance::Instance(boost::filesystem::path filepath)
{
	if (!boost::filesystem::exists(filepath)) {
		std::cout << filepath << ": file not found." << std::endl;
		assert(false);
	}

	boost::filesystem::path FORMAT = filepath.parent_path() / "FORMAT.txt";
	if (!boost::filesystem::exists(FORMAT)) {
		std::cout << FORMAT << ": file not found." << std::endl;
		assert(false);
	}

	boost::filesystem::fstream file(FORMAT, std::ios_base::in);
	std::getline(file, format_);
	if        (format_ == "knapsack_standard") {
		read_standard(filepath);
	} else if (format_ == "knapsack_pisinger") {
		read_pisinger(filepath);
	} else {
		std::cout << format_ << ": Unknown instance format." << std::endl;
		assert(false);
	}
}

void Instance::read_standard(boost::filesystem::path filepath)
{
	name_ = filepath.stem().string();
	boost::filesystem::fstream file(filepath, std::ios_base::in);
	file >> n_;
	file >> c_;
	w_ = new Weight[n_];
	p_ = new Profit[n_];
	w_max_ = 0;
	p_max_ = 0;
	for (ItemIdx i=0; i<n_; ++i) {
		file >> p_[i];
		file >> w_[i];
		if (p_[i] > p_max_)
			p_max_ = p_[i];
		if (w_[i] > w_max_)
			w_max_ = w_[i];
	}

	if (!file.eof())
		file >> opt_;

	file.close();
}

void Instance::read_pisinger(boost::filesystem::path filepath)
{
	boost::filesystem::fstream file(filepath, std::ios_base::in);
	uint_fast64_t null;

	std::getline(file, name_);

	std::string line;
	std::istringstream iss;

	std::getline(file, line, ' ');
	std::getline(file, line);
	std::istringstream(line) >> n_;

	std::getline(file, line, ' ');
	std::getline(file, line);
	std::istringstream(line) >> c_;

	std::getline(file, line, ' ');
	std::getline(file, line);
	std::istringstream(line) >> null;

	std::getline(file, line);

	w_ = new Weight[n_];
	p_ = new Profit[n_];
	w_max_ = 0;
	p_max_ = 0;
	opt_ = 0;

	for (ItemIdx i=0; i<n_; ++i) {
		ItemIdx idx;
		std::getline(file, line, ',');
		std::istringstream(line) >> idx;
		assert(idx == i+1);

		std::getline(file, line, ',');
		std::istringstream(line) >> p_[i];

		std::getline(file, line, ',');
		std::istringstream(line) >> w_[i];

		bool x;
		std::getline(file, line);
		std::istringstream(line) >> x;

		if (x == 1)
			opt_ += p_[i];
		if (p_[i] > p_max_)
			p_max_ = p_[i];
		if (w_[i] > w_max_)
			w_max_ = w_[i];
	}

	file.close();
}

Profit Instance::profit(std::vector<bool> v) const
{
	assert(v.size() == n_);
	Profit res = 0;
	for (ItemIdx i=1; i<=item_number(); ++i)
		if (v[i-1])
			res += profit(i);
	return res;
}

Instance::~Instance()
{
	delete[] w_;
	delete[] p_;
}

std::ostream& operator<<(std::ostream &os, const std::vector<bool>& sol)
{
	for (bool b: sol) {
		if (b) {
			os << 1 << std::endl;
		} else {
			os << 0 << std::endl;
		}
	}
	return os;
}

Profit Instance::check(boost::filesystem::path cert_file)
{
	if (!boost::filesystem::exists(cert_file))
		return -1;
	boost::filesystem::ifstream file(cert_file, std::ios_base::in);
	bool x;
	Profit p = 0;
	Weight c = 0;
	for (ItemIdx i=1; i<=item_number(); ++i) {
		file >> x;
		if (x) {
			p += profit(i);
			c += weight(i);
		}
	}
	if (c > capacity())
		return -1;
	return p;
}
