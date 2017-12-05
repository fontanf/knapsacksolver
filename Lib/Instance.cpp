#include "Instance.hpp"
#include "Solution.hpp"
#include "Tools.hpp"

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

	i_ = std::vector<ItemIdx>(n_);
	iota(i_.begin(), i_.end(), 1);
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

Instance::~Instance()
{
	delete[] w_;
	delete[] p_;
	if (solution_ != NULL)
		delete solution_;
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

Instance::Instance(const Instance& instance):
	instance_orig_((instance.instance_orig() == NULL)? &instance: instance.instance_orig())
{
	if (instance.instance_orig() == NULL) {
		solution_ = new Solution(instance);
	} else {
		solution_ = new Solution(*instance.solution());
	}
	assert(instance.instance_orig() == NULL);
	assert(solution_->profit() == 0);
	n_ = instance.item_number();
	c_ = instance.capacity();

	i_ = std::vector<ItemIdx>(n_);
	iota(i_.begin(), i_.end(), 1);
	sort(i_.begin(), i_.end(),
			[&instance](ItemIdx i1, ItemIdx i2) {
			return instance.profit(i1) * instance.weight(i2)
			< instance.profit(i2) * instance.weight(i1);});

	w_ = new Weight[n_];
	p_ = new Profit[n_];
	for (ItemIdx i=0; i<n_; ++i) {
		w_[i] = instance.weight(i_[i]);
		p_[i] = instance.profit(i_[i]);
	}
	assert(solution_->profit() == 0);
}

Instance::Instance(const Instance& instance, Profit lower_bound):
	instance_orig_((instance.instance_orig() == NULL)? &instance: instance.instance_orig())
{
	if (instance.instance_orig() == NULL) {
		solution_ = new Solution(instance);
	} else {
		solution_ = new Solution(*instance.solution());
	}
	Weight c = instance.capacity();
	c_ = instance.capacity();
	n_ = 0;
	for (ItemIdx i=1; i<=instance.item_number(); ++i) {
		Profit pi = instance.profit(i);
		Weight wi = instance.weight(i);
		if        (!upper_bound_without(instance, i, c-wi, lower_bound-pi)) {
		} else if (!upper_bound_without(instance, i, c,    lower_bound)) {
			solution_->set(instance.index(i), true);
			c_ -= instance.weight(i);
		} else {
			n_++;
			i_.push_back(instance.index(i));
		}
	}

	w_ = new Weight[n_];
	p_ = new Profit[n_];
	for (ItemIdx i=0; i<n_; ++i) {
		w_[i] = instance_orig_->weight(i_[i]);
		p_[i] = instance_orig_->profit(i_[i]);
	}
}

void Instance::info(std::ostream& os)
{
	os << "Reduction: "
		<< n_ << " / " << instance_orig_->item_number() << " (" << (double)n_ / (double)instance_orig_->item_number() << "); "
		<< c_ << " / " << instance_orig_->capacity()    << " (" << (double)c_ / (double)instance_orig_->capacity()    << "); ";
	if (solution_ != NULL)
		os << solution_->profit();
	os << std::endl;
}

std::ostream& operator<<(std::ostream& os, const Instance& instance)
{
	for (ItemIdx i=1; i<=instance.item_number(); ++i)
		os << instance.index(i) << " " << instance.profit(i) << " " << instance.weight(i) << std::endl;
	return os;
}
