#include "ls.hpp"

#include <set>

Profit sol_ls(Solution& solution, Profit ub,
		boost::property_tree::ptree* pt, bool verbose)
{
	const Instance* instance = solution.instance();
	ItemIdx n = instance->item_number();

	for (ItemIdx i=1; i<=n; ++i)
		if (instance->weight(i) <= solution.remaining_capacity())
			solution.set(i, true);

	size_t k = 1;
	if (pt != NULL) {
		pt->put("Solution" + std::to_string(k) + ".Value", solution.profit());
		++k;
	}

	if (verbose)
		std::cout
			<< "LB " << solution.profit()
			<< " GAP " << instance->optimum() - solution.profit()
			<< std::endl;

	auto comp1 = [instance](ItemIdx i, ItemIdx j){ return instance->weight(i) < instance->weight(j); };
	auto comp2 = [instance](ItemIdx i, ItemIdx j){ return instance->weight(i) > instance->weight(j); };
	std::set<ItemIdx, decltype(comp2)> taken(comp2);
	std::set<ItemIdx, decltype(comp1)> left(comp1);
	for (ItemIdx i=1; i<=n; ++i) {
		if (solution.get(i)) {
			taken.insert(i);
		} else {
			left.insert(i);
		}
	}

	//std::cout << *instance << std::endl;

	bool b = true;
	while (b) {
		b = false;

		if (solution.profit() == ub)
			break;

		//std::cout << "taken ";
		//for (ItemIdx i: taken)
			//std::cout << i << " ";
		//std::cout << std::endl;
		//std::cout << "left ";
		//for (ItemIdx j: left)
			//std::cout << j << " " ;
		//std::cout << std::endl;

		auto i1   = left.begin();
		auto i2_1 = left.begin();
		auto i2_2 = left.begin(); i2_2++;
		bool i1_end = false;
		bool i2_end = false;
		Profit pi_prec = -1;
		Weight wi1 = 0, wi2 = 0, wi2_1 = 0, wi2_2 = 0;
		Profit pi1 = 0, pi2 = 0, pi2_1 = 0, pi2_2 = 0;
		
		while (!i1_end || !i2_end) {
			if (!i1_end) {
				wi1 = instance->weight(*i1);
				pi1 = instance->profit(*i1);
				//std::cout << "i1 " << *i1 << " wi1 " << wi1 << " pi1 " << pi1 << std::endl;
			}
			if (!i2_end) {
				wi2_1 = instance->weight(*i2_1);
				wi2_2 = instance->weight(*i2_2);
				pi2_1 = instance->profit(*i2_1);
				pi2_2 = instance->profit(*i2_2);
				wi2 = wi2_1 + wi2_2;
				pi2 = pi2_1 + pi2_2;
				//std::cout << "i2_1 " << *i2_1 << " i2_2 " << *i2_2 << " wi2 " << wi2 << " pi2 " << pi2 << std::endl;
			}
			Weight wi = 0;
			Profit pi = 0;
			std::vector<std::set<ItemIdx, decltype(comp1)>::iterator> to_add;
			if (!i1_end && (i2_end
					|| wi1 < wi2
					|| (wi1 == wi2 && pi1 > pi2))) {
				to_add.push_back(i1);
				wi = wi1;
				pi = pi1;
				if (std::next(i1) == left.end()) {
					i1_end = true;
				} else {
					i1++;
				}
			} else {
				to_add.push_back(i2_1);
				to_add.push_back(i2_2);
				wi = wi2;
				pi = pi2;
				if (std::next(i2_1) == i2_2 && next(i2_2) == left.end()) {
					i2_end = true;
				} else if (std::next(i2_1) == i2_2) {
					i2_2++;
				} else if (std::next(i2_2) == left.end()) {
					i2_1++;
				} else {
					Weight w1 = instance->weight(*std::next(i2_1)) + wi2_2;
					Weight w2 = wi2_1 + instance->weight(*std::next(i2_2));
					Profit p1 = instance->profit(*std::next(i2_1)) + pi2_2;
					Profit p2 = pi2_1 + instance->profit(*std::next(i2_2));
					if (w1 < w2 || (w1 == w2 && p1 > p2)) {
						i2_1++;
					} else {
						i2_2++;
					}
				}
			}
			//std::cout << "wi " << wi << " wi+r " << wi + solution.remaining_capacity() << " pi " << pi << " to_add ";
			//for (auto i: to_add)
				//std::cout << *i << " ";
			//std::cout << std::endl;

			if (pi <= pi_prec) {
				//std::cout << "Continue" << std::endl;
				continue;
			}

			auto j1   = taken.begin();
			auto j2_1 = taken.begin();
			auto j2_2 = taken.begin(); j2_2++;
			bool j1_end = false;
			bool j2_end = false;
			Weight wj1 = 0, wj2 = 0, wj2_1 = 0, wj2_2 = 0;
			Profit pj1 = 0, pj2 = 0, pj2_1 = 0, pj2_2 = 0;
			while (!j1_end || !j2_end) {
				if (!j1_end) {
					pj1 = instance->profit(*j1);
					wj1 = instance->weight(*j1);
					//std::cout << "j1 " << *j1 << " wj1 " << wj1 << " pj1 " << pj1 << std::endl;
				}
				if (!j2_end) {
					wj2_1 = instance->weight(*j2_1);
					wj2_2 = instance->weight(*j2_2);
					pj2_1 = instance->profit(*j2_1);
					pj2_2 = instance->profit(*j2_2);
					wj2 = wj2_1 + wj2_2;
					pj2 = pj2_1 + pj2_2;
					//std::cout << "j2_1 " << *j2_1 << " j2_2 " << *j2_2 << " wj2 " << wj2 << " pj2 " << pj2 << std::endl;
				}
				Weight wj = 0;
				Profit pj = 0;

				std::vector<std::set<ItemIdx, decltype(comp2)>::iterator> to_remove;
				if (!j1_end && (j2_end
						|| wj1 > wj2
						|| (wj1 == wj2 && pj1 > pj2))) {
					to_remove.push_back(j1);
					wj = wj1;
					pj = pj1;
					if (std::next(j1) == taken.end()) {
						j1_end = true;
					} else {
						j1++;
					}
				} else {
					to_remove.push_back(j2_1);
					to_remove.push_back(j2_2);
					wj = wj2;
					pj = pj2;
					if (std::next(j2_1) == j2_2 && std::next(j2_2) == taken.end()) {
						j2_end = true;
					} else if (std::next(j2_1) == j2_2) {
						j2_2++;
					} else if (std::next(j2_2) == taken.end()) {
						j2_1++;
					} else {
						Weight w1 = instance->weight(*std::next(j2_1)) + wi2_2;
						Weight w2 = wi2_1 + instance->weight(*std::next(j2_2));
						Profit p1 = instance->profit(*std::next(j2_1)) + pi2_2;
						Profit p2 = pi2_1 + instance->profit(*std::next(j2_2));
						if (w1 > w2 || (w1 == w2 && p1 > p2)) {
							j2_1++;
						} else {
							j2_2++;
						}
					}
				}

				//std::cout << "wj " << wj << " pj " << pj << " to_remove ";
				//for (auto j: to_remove)
					//std::cout << *j << " ";
				//std::cout << std::endl;

				if (wi > wj + solution.remaining_capacity()) {
					//std::cout << "Break" << std::endl;
					break;
				}

				if (pj < pi || (pj == pi && wi < wj)) {
					if (verbose) {
						std::cout << "Remove ";
						for (auto i: to_remove)
							std::cout << *i << " ";
						std::cout << "Add ";
						for (auto j: to_add)
							std::cout << *j << " ";
					}
					for (auto i: to_remove) {
						solution.set(*i, false);
						left.insert(*i);
						taken.erase(i);
					}
					for (auto j: to_add) {
						solution.set(*j, true);
						taken.insert(*j);
						left.erase(j);
					}
					if (verbose)
						std::cout
							<< "LB " << solution.profit()
							<< " GAP " << instance->optimum() - solution.profit()
							<< std::endl;
					if (pt != NULL) {
						pt->put("Solution" + std::to_string(k) + ".Value", solution.profit());
						++k;
					}
					b = true;
					break;
				}

			}
			pi_prec = pi;
			if (b)
				break;
		}
	}

	if (verbose)
		std::cout
			<< "LB " << solution.profit()
			<< " GAP " << instance->optimum() - solution.profit()
			<< std::endl;
	return solution.profit();
}

Solution sol_ls(const Instance& instance, Profit ub,
		boost::property_tree::ptree* pt, bool verbose)
{
	Solution sol(instance);
	sol_ls(sol, ub, pt, verbose);
	return sol;
}

Profit lb_ls(const Instance& instance, Profit ub)
{
	return sol_ls(instance, ub).profit();
}

