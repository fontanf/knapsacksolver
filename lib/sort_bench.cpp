#include "instance.hpp"

int main(int argc, char *argv[])
{
	(void)argc;
	(void)argv;

	ItemIdx n = 10000000;
	Weight c = 64 * n;
	std::vector<Profit> p(n, 1);
	std::vector<Weight> w(n);
	iota(w.begin(), w.end(), 1);
	std::random_shuffle(w.begin(), w.end());

	Instance instance(n, c, p, w);

	std::cout << "sort_by_efficiency" << std::endl;
	Instance instance_eff  = Instance::sort_by_efficiency(instance);

	std::cout << "sort_partially_by_efficiency" << std::endl;
	Instance instance_peff = Instance::sort_partially_by_efficiency(instance);

	return 0;
}
