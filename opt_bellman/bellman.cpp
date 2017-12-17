#include "bellman.hpp"

#include <map>

#define INDEX(i,w) (i)*(c+1) + (w)

Profit opt_bellman(const Instance& instance,
		boost::property_tree::ptree* pt, bool verbose)
{
	Weight c = instance.capacity();
	Profit* values = new Profit[c+1];
	for (Weight w=c+1; w-->0;)
		values[w] = 0;
	for (ItemIdx i=1; i<=instance.item_number(); ++i) {
		Weight wi = instance.weight(i);
		Profit pi = instance.profit(i);
		for (Weight w=c+1; w-->0;)
			if (w >= wi && values[w-wi] + pi > values[w])
				values[w] = values[w-wi] + pi;
	}
	Profit opt = values[c];
	if (pt != NULL)
		pt->put("Solution.OPT", opt);
	if (verbose)
		std::cout << "OPT: " << opt << std::endl;

	delete[] values; // Free memory
	return opt;
}

/******************************************************************************/

Solution sopt_bellman_1(const Instance& instance,
		boost::property_tree::ptree* pt, bool verbose)
{
	return sopt_bellman_1_it(instance, pt, verbose);
}

/******************************************************************************/

Solution sopt_bellman_1_it(const Instance& instance,
		boost::property_tree::ptree* pt, bool verbose)
{
	// Initialize memory table
	ItemIdx n = instance.item_number();
	Weight  c = instance.capacity();
	ValIdx values_size = (n+1)*(c+1);
	Profit* values = new Profit[values_size];

	// Compute optimal value
	for (Weight w=0; w<=instance.capacity(); ++w)
		values[INDEX(0,w)] = 0;
	for (ItemIdx i=1; i<=instance.item_number(); ++i) {
		Weight wi = instance.weight(i);
		for (Weight w=0; w<=instance.capacity(); ++w) {
			Profit v0 = values[INDEX(i-1,w)];
			Profit v1 = (w < wi)? 0: values[INDEX(i-1,w-wi)] + instance.profit(i);
			values[INDEX(i,w)] = (v1 > v0)? v1: v0;
		}
	}
	Profit opt = values[values_size-1];

	// Retrieve optimal solution
	Solution solution(instance);
	ItemIdx i = n;
	Weight  w = c;
	Profit  v = 0;
	while (v < opt) {
		Weight wi = instance.weight(i);
		Profit pi = instance.profit(i);
		Profit v0 = values[INDEX(i-1,w)];
		Profit v1 = (w < wi)? 0: values[INDEX(i-1,w-wi)] + pi;
		if (v1 > v0) {
			v += pi;
			w -= wi;
			solution.set(i, true);
		}
		i--;
	}

	delete[] values; // Free memory

	if (pt != NULL)
		pt->put("Solution.OPT", opt);
	if (verbose)
		std::cout << "OPT: " << opt << std::endl;

	return solution;
}

/******************************************************************************/

struct RecData
{
	RecData(const Instance& instance):
		instance(instance), i(instance.item_number()), w(instance.capacity())
	{
		ItemIdx n = instance.item_number();
		Weight  c = instance.capacity();
		ValIdx values_size = (n+1)*(c+1);
		values = new Profit[values_size];
		for (ItemIdx i=0; i<values_size; ++i)
			values[i] = -1;
	}
	~RecData() { delete[] values; }
	const Instance& instance;
	Profit* values;
	ItemIdx i;
	Weight w;
	size_t nodes = 0;
};

Profit sopt_bellman_1_rec_rec(RecData& d)
{
	Weight c = d.instance.capacity();

	if (d.values[INDEX(d.i,d.w)] != -1)
		return d.values[INDEX(d.i,d.w)];

	if (d.i == 0) {
		d.values[INDEX(d.i,d.w)] = 0;
		return 0;
	}

	d.i--;
	Profit v0 = sopt_bellman_1_rec_rec(d);
	Profit v1 = -1;
	if (d.w >= d.instance.weight(d.i+1)) {
		d.w -= d.instance.weight(d.i+1);
		v1 = d.instance.profit(d.i+1) + sopt_bellman_1_rec_rec(d);
		d.w += d.instance.weight(d.i+1);
	}
	d.i++;

	d.nodes++;

	if (v1 > v0) {
		d.values[INDEX(d.i,d.w)] = v1;
		return v1;
	} else {
		d.values[INDEX(d.i,d.w)] = v0;
		return v0;
	}
}

Solution sopt_bellman_1_rec(const Instance& instance,
		boost::property_tree::ptree* pt, bool verbose)
{
	ItemIdx n = instance.item_number();
	Weight  c = instance.capacity();

	// Compute optimal value
	RecData data(instance);
	Profit opt = sopt_bellman_1_rec_rec(data);

	// Retrieve optimal solution
	Solution solution(instance);
	ItemIdx i = n;
	Weight  w = c;
	Profit  v = 0;
	while (v < opt) {
		Weight wi = instance.weight(i);
		Profit pi = instance.profit(i);
		Profit v0 = data.values[INDEX(i-1,w)];
		Profit v1 = (w < wi)? 0: data.values[INDEX(i-1,w-wi)] + pi;
		if (v1 > v0) {
			v += pi;
			w -= wi;
			solution.set(i, true);
		}
		i--;
	}

	if (verbose)
		std::cout << "OPT: " << solution.profit() << std::endl;

	size_t nodes_max = (n + 1) * (c + 1);

	if (pt != NULL) {
		pt->put("Solution.Values", data.nodes);
		pt->put("Solution.ValuesRatio",  (double)data.nodes / (double)nodes_max);
		pt->put("Solution.OPT", solution.profit());
	}

	return solution;
}

/******************************************************************************/

struct Node
{
	ItemIdx i;
	Weight  w;
	Profit  p;
	int state = 0;
	Node* child_0;
	Node* child_1;
	Node* parent;
};

Solution sopt_bellman_1_stack(const Instance& instance,
		boost::property_tree::ptree* pt, bool verbose)
{
	// Initialize memory table
	ItemIdx n = instance.item_number();
	Weight  c = instance.capacity();
	ValIdx values_size = (n+1)*(c+1);
	Profit* values = new Profit[values_size];
	for (ItemIdx i=0; i<values_size; ++i)
		values[i] = -1;

	std::stack<Node*> stack;
	size_t size_max = 0;
	size_t nodes    = 0;

	Node* node = new Node();
	node->i = instance.item_number();
	node->w = instance.capacity();
	node->parent = NULL;
	stack.push(node);

	while (!stack.empty()) {
		if (stack.size() > size_max)
			size_max = stack.size();

		Node* node = stack.top();
		stack.pop();
		ItemIdx i = node->i;
		Weight  w = node->w;
		if (node->state == 0) {
			if (values[INDEX(i,w)] != -1) {
				node->p = values[INDEX(i,w)];
				continue;
			}

			if (i == 0) {
				values[INDEX(i,w)] = 0;
				node->p      = 0;
				continue;
			}

			node->state = 1;
			stack.push(node);

			Node* n0 = new Node();
			node->child_0 = n0;
			n0->i = i-1;
			n0->w = w;
			n0->parent = node;
			stack.push(n0);

			if (w >= instance.weight(i)) {
				Node* n1 = new Node();
				node->child_1 = n1;
				n1->i = i-1;
				n1->w = w - instance.weight(i);
				n1->parent = node;
				stack.push(n1);
			}
		} else if (node->state == 1) {
			Profit p0 = node->child_0->p;
			Profit p1 = (w < instance.weight(i))? 0: node->child_1->p + instance.profit(i);

			nodes++;
			values[INDEX(i,w)] = (p1 > p0)? p1: p0;
			node->p = values[INDEX(i,w)];
			delete node->child_0;
			if (w >= instance.weight(i))
				delete node->child_1;
		}
	}
	delete node;

	Profit opt = values[values_size-1];

	// Retrieve optimal solution
	Solution solution(instance);
	ItemIdx i = n;
	Weight  w = c;
	Profit  v = 0;
	while (v < opt) {
		Weight wi = instance.weight(i);
		Profit pi = instance.profit(i);
		Profit v0 = values[INDEX(i-1,w)];
		Profit v1 = (w < wi)? 0: values[INDEX(i-1,w-wi)] + pi;
		if (v1 > v0) {
			v += pi;
			w -= wi;
			solution.set(i, true);
		}
		i--;
	}

	size_t nodes_max = (n + 1) * (c + 1);

	if (pt != NULL) {
		pt->put("Solution.OPT",          opt);
		pt->put("Solution.StackMaxSize", size_max);
		pt->put("Solution.Values",       nodes);
		pt->put("Solution.ValuesRatio",  (double)nodes / (double)nodes_max);
	}

	if (verbose)
		std::cout << "OPT: " << opt << std::endl;

	delete[] values; // Free memory

	return solution;
}

/******************************************************************************/

struct MapData
{
	MapData(const Instance& instance):
		instance(instance), i(instance.item_number()), w(instance.capacity())
	{
		ItemIdx n = instance.item_number();
		values = new std::map<Weight, Profit>[n + 1];
		for (ItemIdx i=0; i<=n; ++i)
			values[i] = std::map<Weight, Profit>();
	}
	~MapData() { delete[] values; }
	const Instance& instance;
	std::map<Weight, Profit>* values;
	ItemIdx i;
	Weight w;
};

Profit sopt_bellman_1_map_rec(MapData& d)
{
	if (d.values[d.i].find(d.w) != d.values[d.i].end())
		return d.values[d.i][d.w];

	if (d.i == 0) {
		d.values[d.i][d.w] = 0;
		return 0;
	}

	d.i--;
	Profit v0 = sopt_bellman_1_map_rec(d);
	Profit v1 = -1;
	if (d.w >= d.instance.weight(d.i+1)) {
		d.w -= d.instance.weight(d.i+1);
		v1 = d.instance.profit(d.i+1) + sopt_bellman_1_map_rec(d);
		d.w += d.instance.weight(d.i+1);
	}
	d.i++;

	if (v1 > v0) {
		d.values[d.i][d.w] = v1;
		return v1;
	} else {
		d.values[d.i][d.w] = v0;
		return v0;
	}
}

Solution sopt_bellman_1_map(const Instance& instance,
		boost::property_tree::ptree* pt, bool verbose)
{
	// Compute optimal value
	MapData data(instance);
	Profit opt = sopt_bellman_1_map_rec(data);

	// Retrieve optimal solution
	Solution solution(instance);
	ItemIdx i = instance.item_number();
	Weight  w = instance.capacity();
	Profit  v = 0;
	while (v < opt) {
		Profit v0 = data.values[i-1][w];
		Profit v1 = (w < instance.weight(i))? 0:
			data.values[i-1][w - instance.weight(i)] + instance.profit(i);
		if (v1 > v0) {
			v += instance.profit(i);
			w -= instance.weight(i);
			solution.set(i, true);
		}
		i--;
	}

	size_t map_size = 0;
	for (ItemIdx i=0; i<=instance.item_number(); ++i)
		map_size += data.values[i].size();
	size_t map_max_size = (instance.item_number() + 1) * (instance.capacity() + 1);

	if (pt != NULL) {
		pt->put("Solution.OPT", opt);
		pt->put("Solution.MapSize", map_size);
		pt->put("Solution.MapRatio", (double)map_size / (double)map_max_size);
	}

	if (verbose)
		std::cout << "OPT: " << opt << std::endl;

	return solution;
}

/******************************************************************************/

Solution sopt_bellman_2(const Instance& instance,
		boost::property_tree::ptree* pt, bool verbose)
{
	ItemIdx n = instance.item_number();
	Weight  c = instance.capacity();

	Solution solution(instance);

	if (verbose)
		std::cout << std::endl;

	Profit* values = new Profit[c+1]; // Initialize memory table
	ItemIdx iter = 0;
	for (;;) {
		if (verbose)
			std::cout << n << " " << std::flush;
		iter++;
		ItemIdx last_item = 0;
		for (Weight w=c+1; w-->0;)
			values[w] = 0;
		for (ItemIdx i=1; i<=n; ++i) {
			Weight wi = instance.weight(i);
			Profit pi = instance.profit(i);
			if (c >= wi && values[c-wi] + pi > values[c]) {
				values[c] = values[c-wi] + pi;
				last_item = i;
			}
			for (Weight w=c; w-->0;)
				if (w >= wi && values[w-wi] + pi > values[w])
					values[w] = values[w-wi] + pi;
		}

		if (last_item == 0)
			break;

		solution.set(last_item, true);
		c -= instance.weight(last_item);
		n = last_item - 1;
	}
	if (verbose)
		std::cout << std::endl;

	if (verbose)
		std::cout << "OPT: " << solution.profit() << std::endl;

	if (pt != NULL) {
		pt->put("Solution.Iterations", iter);
		pt->put("Solution.OPT", solution.profit());
	}

	delete[] values; // Free memory

	return solution;
}

/******************************************************************************/

#define DBG(x)
//#define DBG(x) x

struct RecData2
{
	RecData2(const Instance& instance):
		instance(instance), n1(1), n2(instance.item_number()),
		c(instance.capacity()), sol_curr(instance)
	{
		values1 = new Profit[2*(c+1)];
		values2 = values1 + c+1;
	}
	~RecData2() { delete[] values1; }
	const Instance& instance;
	ItemIdx n1;
	ItemIdx n2;
	Weight  c;
	Solution sol_curr;
	Profit* values1;
	Profit* values2;
};

void opts_bellman(const Instance& instance, Profit* values, ItemIdx n1, ItemIdx n2, Weight c)
{
	DBG(std::cout << "OPTS n1 " << n1 << " n2 " << n2 << " c " << c << std::endl;)
	for (Weight w=c+1; w-->0;)
		values[w] = 0;
	for (ItemIdx i=n1; i<=n2; ++i) {
		Weight wi = instance.weight(i);
		Profit pi = instance.profit(i);
		for (Weight w=c+1; w-->0;)
			if (w >= wi && values[w-wi] + pi > values[w])
				values[w] = values[w-wi] + pi;
	}
}

void sopt_bellman_3_rec(RecData2& d)
{
	DBG(std::cout << "Rec n1 " << d.n1 << " n2 " << d.n2 << " c " << d.c << std::endl;)
	DBG(std::cout << d.sol_curr << std::endl;)
	ItemIdx k = (d.n1 + d.n2) / 2;
	DBG(std::cout << "k " << k << std::endl;)
	ItemIdx n2 = d.n2;
	opts_bellman(d.instance, d.values1, d.n1, k, d.c);
	opts_bellman(d.instance, d.values2, k+1, d.n2, d.c);

	DBG(std::cout << "Find" << std::endl;)
	Profit z_max  = -1;
	Profit z2_opt = -1;
	Weight c1_opt = 0;
	Weight c2_opt = 0;
	for (Weight c1=0; c1<=d.c; ++c1) {
		Weight c2 = d.c - c1;
		Profit z = d.values1[c1] + d.values2[c2];
		if (z > z_max) {
			z_max = z;
			c1_opt = c1;
			c2_opt = c2;
			z2_opt = d.values2[c2];
		}
	}
	assert(z_max != -1);
	DBG(std::cout << "c1 " << c1_opt << " c2 " << c2_opt << std::endl;)

	DBG(std::cout << "Conquer" << std::endl;)
	if (k == d.n1) {
		DBG(std::cout << "Leaf" << std::endl;)
		if (d.values1[c1_opt] == d.instance.profit(d.n1)) {
			DBG(std::cout << "Set " << d.n1 << std::endl;)
			d.sol_curr.set(d.n1, true);
		}
	} else {
		DBG(std::cout << "..." << std::endl;)
		d.n2 = k;
		d.c  = c1_opt;
		sopt_bellman_3_rec(d);
	}

	if (k+1 == n2) {
		DBG(std::cout << "Leaf" << std::endl;)
		if (z2_opt == d.instance.profit(n2)) {
			DBG(std::cout << "Set " << n2 << std::endl;)
			d.sol_curr.set(n2, true);
		}
	} else {
		DBG(std::cout << "..." << std::endl;)
		d.n1 = k+1;
		d.n2 = n2;
		d.c  = c2_opt;
		sopt_bellman_3_rec(d);
	}
}

Solution sopt_bellman_3(const Instance& instance,
		boost::property_tree::ptree* pt, bool verbose)
{
	if (instance.item_number() == 0) {
		Solution solution(instance);
		if (pt != NULL)
			pt->put("Solution.OPT", 0);
		return solution;
	}

	if (instance.item_number() == 1) {
		Solution solution(instance);
		if (instance.weight(1) <= instance.capacity())
			solution.set(1, true);
		if (pt != NULL)
			pt->put("Solution.OPT", solution.profit());
		return solution;
	}

	RecData2 data(instance);
	sopt_bellman_3_rec(data);

	if (pt != NULL)
		pt->put("Solution.OPT", data.sol_curr.profit());

	if (verbose)
		std::cout << "OPT: " << data.sol_curr.profit() << std::endl;

	DBG(std::cout << data.sol_curr << std::endl;)

	return data.sol_curr;
}

#undef DBG
