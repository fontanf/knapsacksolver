#include "balknap.hpp"

#include "../lb_ls/ls.hpp"
#include "../lb_greedy/greedy.hpp"
#include "../ub_dantzig/dantzig.hpp"

#include <map>

#define IDX2(k,w,p) rl2*(k) + rl1*(w) + (p)
#define IDX1(  w,p)           rl1*(w) + (p)

#define DBG(x)
//#define DBG(x) x

Profit opt_balknap(const Instance& instance, Profit lb,
		boost::property_tree::ptree* pt, bool verbose)
{
	DBG(std::cout << instance << std::endl;)

	Weight  c = instance.capacity();
	ItemIdx n = instance.item_number();

	// If there is only 1 item
	if (n == 1) {
		Solution solution(instance);
		if (instance.weight(1) <= instance.capacity())
			solution.set(1, true);
		if (pt != NULL)
			pt->put("Solution.OPT", solution.profit());
		if (verbose) {
			std::cout << "OPT " << solution.profit() << std::endl;
			std::cout << "EXP " << instance.optimum() << std::endl;
		}
		return solution.profit();
	}

	// Compute weight max
	ItemIdx j_wmax = instance.max_weight_item();
	if (j_wmax == 0) {
		if (pt != NULL) {
			pt->put("Solution.OPT", 0);
		}
		if (verbose) {
			std::cout << "OPT " << 0 << std::endl;
			std::cout << "EXP " << instance.optimum() << std::endl;
		}
		return 0;
	}
	Weight w_max = instance.weight(j_wmax);
	DBG(std::cout << "w_max " << w_max << std::endl;)

	// Compute break item
	Weight  r = c;
	ItemIdx b = 0;
	Profit  p_bar = 0;
	for (b=1; b<=n; ++b) {
		Weight wi = instance.weight(b);
		if (wi > r)
			break;
		r     -= wi;
		p_bar += instance.profit(b);
	}

	if (b == n+1) { // all items are in the break solution
		if (pt != NULL)
			pt->put("Solution.OPT", p_bar);
		if (verbose) {
			std::cout << "OPT " << p_bar << std::endl;
			std::cout << "EXP " << instance.optimum() << std::endl;
		}
		return p_bar;
	}
	Profit pb    = instance.profit(b);
	Weight wb    = instance.weight(b);
	Weight w_bar = instance.capacity() - r;
	Profit z     = p_bar;
	Profit u     = p_bar + r * pb / wb;
	if (z < lb)
		z = lb;

	DBG(std::cout << "n " << n << " c " << c << std::endl;)
	DBG(std::cout << "b " << b << " pb " << pb << " wb " << wb << std::endl;)
	DBG(std::cout << "p_bar " << p_bar << " w_bar " << w_bar << std::endl;)
	DBG(std::cout << "z " << z << " u " << u << std::endl;)

	if (verbose)
		std::cout << "z " << z << " u " << u << " GAP " << u - z << std::endl;

	if (z == u) { // If UB == LB, then stop
		if (pt != NULL)
			pt->put("Solution.OPT", z);
		if (verbose) {
			std::cout << "OPT " << z << std::endl;
			std::cout << "EXP " << instance.optimum() << std::endl;
		}
		return z;
	}

	// Create memory table
	DBG(std::cout << "Create memory table" << std::endl;)
	Profit rl1 = u - z;
	StateIdx rl2 = rl1 * 2 * w_max;
	DBG(std::cout << "rl1 " << rl1 << " rl2 " << rl2 << std::endl;)
	if (verbose)
		std::cout << "Memory " << (double)((rl2 * 2) * sizeof(ItemIdx)) / 1000000000 << std::endl;
	ItemIdx* s = new ItemIdx[2*rl2];
	ItemIdx* s0 = s;
	ItemIdx* s1 = s0 + rl2;

	// Initialization
	for (Weight mu=c-w_max+1; mu<=c; ++mu) { // s(mu,pi) = 0 for mu <= c
		Weight w = mu + w_max - c - 1;
		Profit x = ((c - mu) * pb) / wb;
		x = (x >= 0)? x: x-1;
		for (Profit pi=z+1-x; pi<=u-x; ++pi) {
			Profit p = pi + x - z - 1;
			s0[IDX1(w,p)] = 0;
		}
	}
	for (Weight mu=c+1; mu<=c+w_max; ++mu) { // s(mu,pi) = 1 for mu > c
		Weight w = mu + w_max - c - 1;
		Profit x = ((c - mu) * pb) / wb;
		if (c < mu)
			x -= 1;
		for (Profit pi=z+1-x; pi<=u-x; ++pi) {
			Profit p = pi + x - z - 1;
			s0[IDX1(w,p)] = 1;
		}
	}
	Profit w_tmp = w_bar + w_max - 1 - c; // s(w_bar,p_bar) = b
	Profit x_tmp = ((c - w_bar) * pb) / wb;
	Profit p_tmp = p_bar + x_tmp - z - 1;
	s0[IDX1(w_tmp,p_tmp)] = b;

	for (ItemIdx t=b; t<=n; ++t) { // Recursion
		Weight wt = instance.weight(t);
		Profit pt = instance.profit(t);
		DBG(std::cout << "t " << t << " pt " << pt << " wt " << wt << std::endl;)
		// Copy previous iteration table
		for (Weight mu=c-w_max+1; mu<=c+w_max; ++mu) {
			Weight w = mu + w_max - c - 1;
			Profit x = ((c - mu) * pb) / wb;
			if (c < mu)
				x -= 1;
			for (Profit pi=z+1-x; pi<=u-x; ++pi) {
				Profit p = pi + x - z - 1;
				s1[IDX1(w,p)] = s0[IDX1(w,p)];
				DBG(if ((mu <= c && s1[IDX1(w,p)] > 0)
						|| (mu > c && s1[IDX1(w,p)] > 1))
					std::cout << "(" << mu << "," << pi << "," << s1[IDX1(w,p)] << ") ";)
			}
		}

		// Add item t
		DBG(std::cout << "+ ";)
		for (Weight mu=c-w_max+1; mu<=c; ++mu) {
			Weight mu_ = mu + wt;
			Weight w   = mu  + w_max - c - 1;
			Weight w_  = mu_ + w_max - c - 1;
			Profit x   = ((c - mu)  * pb) / wb;
			Profit x_  = ((c - mu_) * pb) / wb;
			if (c < mu)
				x -= 1;
			if (c < mu_)
				x_ -= 1;
			Profit inf = z + 1 - ((x <= x_ + pt)? x: x_ + pt);
			Profit sup = u     - ((x >= x_ + pt)? x: x_ + pt);
			assert(w_ < 2 * w_max);
			for (Profit pi=inf; pi<=sup; ++pi) {
				Profit pi_ = pi + pt;
				Profit p  = pi  + x  - z - 1;
				Profit p_ = pi_ + x_ - z - 1;
				if (s1[IDX1(w_,p_)] < s0[IDX1(w,p)])
					s1[IDX1(w_,p_)] = s0[IDX1(w,p)];
				DBG(if ((mu_ <= c && s1[IDX1(w_,p_)] > 0)
						|| (mu_ > c && s1[IDX1(w_,p_)] > 1))
					std::cout << "(" << mu_ << "," << pi_ << "," << s1[IDX1(w_,p_)] << ") ";)
			}
		}

		// Remove previously added items
		DBG(std::cout << "- ";)
		for (Weight mu=c+wt; mu>c; --mu) {
			Weight w = mu + w_max - c - 1;
			Profit x = ((c - mu) * pb) / wb;
			if (c < mu)
				x -= 1;
			for (Profit pi=z+1-x; pi<=u-x; ++pi) {
				Profit p = pi + x - z - 1;
				for (ItemIdx j=s0[IDX1(w,p)]; j<s1[IDX1(w,p)]; ++j) {
					Weight mu_ = mu - instance.weight(j);
					Profit pi_ = pi - instance.profit(j);
					Profit x_  = ((c - mu_) * pb) / wb;
					if (c < mu_)
						x_ -= 1;
					if (pi_ < z + 1 - x_ || pi_ > u - x_)
						continue;
					Weight w_  = mu_ + w_max - 1 - c;
					Profit p_  = pi_ + x_ - z - 1;
					if (s1[IDX1(w_,p_)] < j)
						s1[IDX1(w_,p_)] = j;
					DBG(if ((mu_ <= c && s1[IDX1(w_,p_)] > 0)
							|| (mu_ > c && s1[IDX1(w_,p_)] > 1))
						std::cout << "(" << mu_ << "," << pi_ << "," << s1[IDX1(w_,p_)] << ") ";)
				}
			}
		}
		DBG(std::cout << std::endl;)

		// Swap pointers
		ItemIdx* tmp = s1;
		s1 = s0;
		s0 = tmp;
	}

	// Get optimal value
	Profit opt = z;
	for (Weight mu=c-w_max+1; mu<=c; ++mu) {
		Weight w = mu + w_max - c - 1;
		Profit x = ((c - mu) * pb) / wb;
		if (c < mu)
			x -= 1;
		for (Profit pi=z+1-x; pi<=u-x; ++pi) {
			Profit p = pi + x - z - 1;
			if (s0[IDX1(w,p)] > 0 && pi > opt)
				opt = pi;
		}
	}

	if (pt != NULL) {
		pt->put("Solution.OPT", opt);
	}
	if (verbose) {
		std::cout << "OPT " << opt << std::endl;
		std::cout << "EXP " << instance.optimum() << std::endl;
	}

	delete[] s;

	return opt;
}

#undef DBG

/******************************************************************************/

#define DBG(x)
//#define DBG(x) x

Profit sopt_balknap(const Instance& instance, Solution& sol_curr,
		boost::property_tree::ptree* pt, bool verbose)
{
	DBG(std::cout << "sopt_balknap..." << std::endl;);

	Weight  c = instance.capacity();
	ItemIdx n = instance.item_number();

	// If there is only 1 item
	if (n == 1) {
		if (instance.weight(1) <= instance.capacity())
			sol_curr.set(1, true);
		if (pt != NULL)
			pt->put("Solution.OPT", sol_curr.profit());
		if (verbose) {
			std::cout << "OPT " << sol_curr.profit() << std::endl;
			std::cout << "EXP " << instance.optimum() << std::endl;
		}
		return sol_curr.profit();
	}

	// Compute weight max
	ItemIdx j_wmax = instance.max_weight_item();
	if (j_wmax == 0) {
		if (pt != NULL) {
			pt->put("Solution.OPT", 0);
		}
		if (verbose) {
			std::cout << "OPT " << 0 << std::endl;
			std::cout << "EXP " << instance.optimum() << std::endl;
		}
		return 0;
	}
	Weight w_max = instance.weight(j_wmax);

	// Compute break item
	Weight  r = c;
	ItemIdx b = 0;
	Profit  p_bar = 0;
	for (b=1; b<=n; ++b) {
		Weight wi = instance.weight(b);
		if (wi > r)
			break;
		r     -= wi;
		p_bar += instance.profit(b);
	}

	if (b == n+1) { // all items are in the break solution
		for (ItemIdx i=1; i<=n; ++i)
			sol_curr.set(i, true);
		if (pt != NULL)
			pt->put("Solution.OPT", p_bar);
		if (verbose) {
			std::cout << "OPT " << p_bar << std::endl;
			std::cout << "EXP " << instance.optimum() << std::endl;
		}
		return p_bar;
	}
	Profit pb    = instance.profit(b);
	Weight wb    = instance.weight(b);
	Weight w_bar = instance.capacity() - r;
	Profit z     = p_bar;
	Profit u     = p_bar + r * pb / wb;
	if (z < sol_curr.profit())
		z = sol_curr.profit();

	if (verbose)
		std::cout << "z " << z << " u " << u << " GAP " << u - z << std::endl;

	if (z == u) { // If UB == LB, then stop
		if (z != sol_curr.profit()) {
			for (ItemIdx i=1; i<b; ++i)
				sol_curr.set(i, true);
			for (ItemIdx i=b; i<=n; ++i)
				sol_curr.set(i, false);
		}
		if (pt != NULL)
			pt->put("Solution.OPT", z);
		if (verbose) {
			std::cout << "OPT " << z << std::endl;
			std::cout << "EXP " << instance.optimum() << std::endl;
		}
		return z;
	}

	// Create memory table
	DBG(std::cout << "Create memory table..." << std::endl;)
	Profit rl1 = u - z;
	StateIdx rl2 = rl1 * 2 * w_max;
	DBG(std::cout << "rl1 " << rl1 << " rl2 " << rl2 << std::endl;)
	if (verbose)
		std::cout << "Memory " << (double)(((n-b+2) * rl2 * 2) * sizeof(ItemIdx)) / 1000000000 << std::endl;
	ItemIdx* s    = new ItemIdx[(n-b+2)*rl2];
	ItemIdx* pred = new ItemIdx[(n-b+2)*rl2];

	// Initialization
	DBG(std::cout << "Initialization..." << std::endl;)
	for (Weight mu=c-w_max+1; mu<=c; ++mu) { // s(mu,pi) = 0 for mu <= c
		Weight w = mu + w_max - c - 1;
		Profit x = ((c - mu) * pb) / wb;
		x = (x >= 0)? x: x-1;
		for (Profit pi=z+1-x; pi<=u-x; ++pi) {
			Profit p = pi + x - z - 1;
			s[IDX2(0,w,p)] = 0;
		}
	}
	for (Weight mu=c+1; mu<=c+w_max; ++mu) { // s(mu,pi) = 1 for mu > c
		Weight w = mu + w_max - c - 1;
		Profit x = ((c - mu) * pb) / wb;
		if (c < mu)
			x -= 1;
		for (Profit pi=z+1-x; pi<=u-x; ++pi) {
			Profit p = pi + x - z - 1;
			s[IDX2(0,w,p)] = 1;
		}
	}
	Profit w_tmp = w_bar + w_max - 1 - c; // s(w_bar,p_bar) = b
	Profit x_tmp = ((c - w_bar) * pb) / wb;
	Profit p_tmp = p_bar + x_tmp - z - 1;
	s[IDX2(0,w_tmp,p_tmp)] = b;

	DBG(std::cout << "Recursion" << std::endl;)
	for (ItemIdx t=b; t<=n; ++t) { // Recursion
		ItemIdx k = t - b + 1;
		Weight wt = instance.weight(t);
		Profit pt = instance.profit(t);

		// Copy previous iteration table
		for (Weight mu=c-w_max+1; mu<=c+w_max; ++mu) {
			Weight w = mu + w_max - c - 1;
			Profit x = ((c - mu) * pb) / wb;
			if (c < mu)
				x -= 1;
			for (Profit pi=z+1-x; pi<=u-x; ++pi) {
				Profit p = pi + x - z - 1;
				s[IDX2(k,w,p)] = s[IDX2(k-1,w,p)];
				pred[IDX2(k,w,p)] = IDX2(k-1,w,p);
			}
		}

		// Add item t
		for (Weight mu=c-w_max+1; mu<=c; ++mu) {
			Weight mu_ = mu + wt;
			Weight w   = mu  + w_max - c - 1;
			Weight w_  = mu_ + w_max - c - 1;
			Profit x   = ((c - mu)  * pb) / wb;
			Profit x_  = ((c - mu_) * pb) / wb;
			if (c < mu)
				x -= 1;
			if (c < mu_)
				x_ -= 1;
			Profit inf = z + 1 - ((x <= x_ + pt)? x: x_ + pt);
			Profit sup = u     - ((x >= x_ + pt)? x: x_ + pt);
			assert(w_ < 2 * w_max);
			for (Profit pi=inf; pi<=sup; ++pi) {
				Profit pi_ = pi + pt;
				Profit p  = pi  + x  - z - 1;
				Profit p_ = pi_ + x_ - z - 1;
				if (s[IDX2(k,w_,p_)] < s[IDX2(k-1,w,p)]) {
					s[IDX2(k,w_,p_)] = s[IDX2(k-1,w,p)];
					pred[IDX2(k,w_,p_)] = IDX2(k-1,w,p);
				}
			}
		}

		// Remove previously added items
		for (Weight mu=c+wt; mu>c; --mu) {
			Weight w = mu + w_max - c - 1;
			Profit x = ((c - mu) * pb) / wb;
			if (c < mu)
				x -= 1;
			for (Profit pi=z+1-x; pi<=u-x; ++pi) {
				Profit p = pi + x - z - 1;
				for (ItemIdx j=s[IDX2(k-1,w,p)]; j<s[IDX2(k,w,p)]; ++j) {
					Weight mu_ = mu - instance.weight(j);
					Profit pi_ = pi - instance.profit(j);
					Profit x_  = ((c - mu_) * pb) / wb;
					if (c < mu_)
						x_ -= 1;
					if (pi_ < z + 1 - x_ || pi_ > u - x_)
						continue;
					Weight w_  = mu_ + w_max - 1 - c;
					Profit p_  = pi_ + x_ - z - 1;
					if (s[IDX2(k,w_,p_)] < j) {
						s[IDX2(k,w_,p_)] = j;
						pred[IDX2(k,w_,p_)] = IDX2(k,w,p);
					}
				}
			}
		}
	}

	// Get optimal value
	DBG(std::cout << "Get optimal value..." << std::endl;)
	Profit opt = z;
	StateIdx idx_opt = 0;
	for (Weight mu=c-w_max+1; mu<=c; ++mu) {
		Weight w = mu + w_max - c - 1;
		Profit x = ((c - mu) * pb) / wb;
		if (c < mu)
			x -= 1;
		for (Profit pi=z+1-x; pi<=u-x; ++pi) {
			Profit p = pi + x - z - 1;
			if (s[IDX2(n-b+1,w,p)] > 0 && pi > opt) {
				opt = pi;
				idx_opt = IDX2(n-b+1,w,p);
			}
		}
	}
	DBG(std::cout << "OPT " << opt << std::endl;)

	// Retrieve optimal solution
	DBG(std::cout << "Retrieve optimal solution..." << std::endl;)
	if (opt != z) {
		for (ItemIdx i=b; i<=n; ++i)
			sol_curr.set(i, false);
		for (ItemIdx i=1; i<b; ++i)
			sol_curr.set(i, true);

		DBG(std::cout << "Profit curr " << sol_curr.profit() << std::endl;)

		StateIdx idx = idx_opt;
		ItemIdx  k   = idx / rl2;
		ItemIdx  t   = k + b - 1;
		Weight   w   = (idx % rl2) / rl1;
		Weight   mu  = c + 1 + w - w_max;
		Profit   p   = (idx % rl2) % rl1;
		Profit   x   = ((c - mu) * pb) / wb;
		if (c < mu)
			x -= 1;
		Profit pi = p + z + 1 - x;
		DBG(std::cout << "t " << t << " wt " << instance.weight(t) << " pt " << instance.profit(t) << " mu " << mu << " pi " << pi << std::endl;)

		while (!(sol_curr.profit() == opt && sol_curr.remaining_capacity() >= 0)) {
			StateIdx idx_next = pred[idx];
			ItemIdx  k_next   = idx_next / rl2;
			ItemIdx  t_next   = k_next + b - 1;
			Weight   w_next   = (idx_next % rl2) / rl1;
			Weight   mu_next  = c + 1 + w_next - w_max;
			Profit   p_next   = (idx_next % rl2) % rl1;
			Profit   x_next   = ((c - mu_next) * pb) / wb;
			if (c < mu_next)
				x_next -= 1;
			Profit pi_next = p_next + z + 1 - x_next;
			//DBG(std::cout << "t " << t_next << " wt " << instance.weight(t_next) << " pt " << instance.profit(t_next) << " mu " << mu_next << " pi " << pi_next << std::endl;)

			if (k_next < k && pi_next < pi) {
				sol_curr.set(t, true);
				DBG(std::cout << "Add " << t << " profit " << sol_curr.profit() << std::endl;)
				assert(mu_next == mu - instance.weight(t));
				assert(pi_next == pi - instance.profit(t));
			}
			if (k_next == k) {
				sol_curr.set(s[idx], false);
				DBG(std::cout << "Remove " << s[idx] << " profit " << sol_curr.profit() << std::endl;)
			}

			idx = idx_next;
			k   = k_next;
			t   = t_next;
			w   = w_next;
			mu  = mu_next;
			p   = p_next;
			pi  = pi_next;
		}
	}

	DBG(std::cout << "End..." << std::endl;)

	if (pt != NULL) {
		pt->put("Solution.OPT", sol_curr.profit());
	}
	if (verbose) {
		std::cout << "OPT " << sol_curr.profit() << std::endl;
		std::cout << "EXP " << instance.optimum() << std::endl;
	}

	delete[] s;
	delete[] pred;

	return sol_curr.profit();
}

#undef DBG

/******************************************************************************/

#define DBG(x)
//#define DBG(x) x

struct State1
{
	Weight mu;
	Profit pi;
	bool operator()(const State1& s1, const State1& s2)
	{
		if (s1.mu != s2.mu)
			return s1.mu < s2.mu;
		if (s1.pi != s2.pi)
			return s1.pi < s2.pi;
		return false;
	}
};

struct StateValue1
{
	ItemIdx a;
	ItemIdx a_prec;
};

std::ostream& operator<<(std::ostream& os, std::map<State1, StateValue1>::iterator s)
{
	os << "(" << s->first.mu << " " << s->first.pi << " " << s->second.a << ")";
	return os;
}

Profit opt_balknap_list(const Instance& instance, Profit lb,
		boost::property_tree::ptree* pt, bool verbose)
{
	DBG(std::cout << instance << std::endl;)

	assert(instance.sort_type() == "efficiency" ||
			instance.sort_type() == "partial_efficiency");

	Weight  c = instance.capacity();
	ItemIdx n = instance.item_number();

	// If there is only 1 item
	if (n == 1) {
		Solution solution(instance);
		if (instance.weight(1) <= instance.capacity())
			solution.set(1, true);
		if (pt != NULL)
			pt->put("Solution.OPT", solution.profit());
		if (verbose) {
			std::cout << "OPT " << solution.profit() << std::endl;
			std::cout << "EXP " << instance.optimum() << std::endl;
		}
		return solution.profit();
	}

	// Compute weight max
	ItemIdx j_wmax = instance.max_weight_item();
	if (j_wmax == 0) {
		if (pt != NULL) {
			pt->put("Solution.OPT", 0);
		}
		if (verbose) {
			std::cout << "OPT " << 0 << std::endl;
			std::cout << "EXP " << instance.optimum() << std::endl;
		}
		return 0;
	}
	Weight w_max = instance.weight(j_wmax);
	if (verbose)
		std::cout
			<< "wmax " << w_max
			<< " c " << c
			<< " wmax/c " << (double)w_max/(double)c << std::endl;

	// Compute break item
	Weight  r = c;
	ItemIdx b = 0;
	Profit  p_bar = 0;
	for (b=1; b<=n; ++b) {
		Weight wi = instance.weight(b);
		if (wi > r)
			break;
		r     -= wi;
		p_bar += instance.profit(b);
	}

	if (b == n+1) { // all items are in the break solution
		if (pt != NULL)
			pt->put("Solution.OPT", p_bar);
		if (verbose) {
			std::cout << "OPT " << p_bar << std::endl;
			std::cout << "EXP " << instance.optimum() << std::endl;
		}
		return p_bar;
	}
	Profit pb    = instance.profit(b);
	Weight wb    = instance.weight(b);
	Weight w_bar = instance.capacity() - r;
	Profit z     = p_bar;
	Profit u     = p_bar + r * pb / wb;
	if (z < lb)
		z = lb;

	DBG(std::cout << "n " << n << " c " << c << std::endl;)
	DBG(std::cout << "b " << b << " pb " << pb << " wb " << wb << std::endl;)
	DBG(std::cout << "p_bar " << p_bar << " w_bar " << w_bar << std::endl;)
	DBG(std::cout << "z " << z << " u " << u << std::endl;)

	if (verbose)
		std::cout << "z " << z << " u " << u << " GAP " << u - z << std::endl;

	if (z == u) { // If UB == LB, then stop
		if (pt != NULL)
			pt->put("Solution.OPT", z);
		if (verbose) {
			std::cout << "OPT " << z << std::endl;
			std::cout << "EXP " << instance.optimum() << std::endl;
		}
		return z;
	}

	// Create memory table
	std::map<State1, StateValue1, State1> map;

	// Initialization
	map.insert({{w_bar,p_bar},{b,b}}); // s(w_bar,p_bar) = b

	DBG(for (auto s = map.begin(); s != map.end(); ++s)
		std::cout << s << " ";
	std::cout << std::endl;)

	for (ItemIdx t=b; t<=n; ++t) { // Recursion
		Weight wt = instance.weight(t);
		Profit pt = instance.profit(t);
		DBG(std::cout << "t " << t << " pt " << pt << " wt " << wt << std::endl;)

		// Add item t
		auto s = map.upper_bound({c+1,0});
		auto hint = s;
		hint--;
		DBG(std::cout << "+ ";)
		while (s != map.begin() && (--s)->first.mu <= c) {
			Weight mu_ = s->first.mu + wt;
			Weight pi_ = s->first.pi + pt;

			// Bounding
			if (instance.sort_type() == "partial_efficiency") {
				Profit x_  = ((c -   mu_) * pb) / wb;
				if (c < mu_)
					x_ -= 1;
				if (pi_ < z + 1 - x_ || pi_ > u - x_)
					continue;
			} else {
				assert(instance.sort_type() == "efficiency");
				Profit ub = pi_;
				if (c < mu_) {
					ub += ub_dantzig_rev_from(instance, s->second.a-1, c-mu_);
				} else {
					ub += ub_dantzig_from(instance, t, c-mu_);
				}
				if (ub <= z || ub > u)
					continue;
			}

			hint = map.insert(hint, {{mu_, pi_}, {s->second.a, 1}});
			hint--;
		}

		// Remove previously added items
		DBG(std::cout << "- ";)
		for (auto s = map.rbegin(); s->first.mu > c; ++s) {
			if (s->first.mu > c + wt)
				continue;

			for (ItemIdx j = s->second.a_prec; j < s->second.a; ++j) {
				Weight mu_ = s->first.mu - instance.weight(j);
				Profit pi_ = s->first.pi - instance.profit(j);

				// Bounding
				if (instance.sort_type() == "partial_efficiency") {
					Profit x_  = ((c - mu_) * pb) / wb;
					if (c < mu_)
						x_ -= 1;
					if (pi_ < z + 1 - x_ || pi_ > u - x_)
						continue;
				} else {
					assert(instance.sort_type() == "efficiency");
					Profit ub = pi_;
					if (c < mu_) {
						ub += ub_dantzig_rev_from(instance, j-1, c-mu_);
					} else {
						ub += ub_dantzig_from(instance, t+1, c-mu_);
					}
					if (ub <= z || ub > u)
						continue;
				}

				auto res = map.insert({{mu_,pi_},{j, 1}});
				if (!res.second)
					if (res.first->second.a < j)
						res.first->second.a = j;
			}
			s->second.a_prec = s->second.a;
		}
		DBG(std::cout << std::endl;)

		DBG(for (auto s = map.begin(); s != map.end(); ++s)
			std::cout << s << " ";
		std::cout << std::endl;)
	}

	// Get optimal value
	Profit opt = z;
	for (auto& s: map)
		if (s.first.mu <= c && s.first.pi > opt)
			opt = s.first.pi;

	if (pt != NULL) {
		pt->put("Solution.OPT", opt);
	}
	if (verbose) {
		std::cout << "OPT " << opt << std::endl;
		std::cout << "EXP " << instance.optimum() << std::endl;
	}

	return opt;
}

#undef DBG

/******************************************************************************/

#define DBG(x)
//#define DBG(x) x

struct State2
{
	Weight mu;
	Profit pi;
	ItemIdx b;
	bool operator()(const State2& s1, const State2& s2)
	{
		if (s1.mu != s2.mu)
			return s1.mu < s2.mu;
		if (s1.pi != s2.pi)
			return s1.pi < s2.pi;
		return false;
	}
};

struct StateValue2
{
	ItemIdx a;
	ItemIdx a_prec; // 'a' from state '(mu,pi,a,b-1)'
	std::map<State2, StateValue2, State2>::iterator pred;
};

std::ostream& operator<<(std::ostream& os, std::map<State2, StateValue2>::iterator s)
{
	auto pred = s->second.pred;
	os << "("
		<< s->first.mu << " " << s->first.pi << " "
		<< s->second.a << " " << s->first.b << " "
		<< s->second.a_prec << " "
		<< "[" << pred->first.mu << " " << pred->first.pi << " "
		<< pred->second.a << " " << pred->first.b << "]"
		<< ")";
	assert(
			   (pred->first.pi <  s->first.pi && pred->second.a == s->second.a && pred->first.b + 1 == s->first.b)
			|| (pred->first.pi == s->first.pi)
			|| (pred->first.pi >  s->first.pi && pred->second.a > s->second.a && pred->first.b     == s->first.b));
	return os;
}

Profit sopt_balknap_list(const Instance& instance, Solution& sol_curr,
		boost::property_tree::ptree* pt, bool verbose)
{
	DBG(std::cout << "sopt_balknap..." << std::endl;);

	assert(instance.sort_type() == "efficiency" ||
			instance.sort_type() == "partial_efficiency");

	Weight  c = instance.capacity();
	ItemIdx n = instance.item_number();

	// If there is only 1 item
	if (n == 1) {
		if (instance.weight(1) <= instance.capacity())
			sol_curr.set(1, true);
		if (pt != NULL)
			pt->put("Solution.OPT", sol_curr.profit());
		if (verbose) {
			std::cout << "OPT " << sol_curr.profit() << std::endl;
			std::cout << "EXP " << instance.optimum() << std::endl;
		}
		return sol_curr.profit();
	}

	// Compute weight max
	ItemIdx j_wmax = instance.max_weight_item();
	if (j_wmax == 0) {
		if (pt != NULL) {
			pt->put("Solution.OPT", 0);
		}
		if (verbose) {
			std::cout << "OPT " << 0 << std::endl;
			std::cout << "EXP " << instance.optimum() << std::endl;
		}
		return 0;
	}

	// Compute break item
	Weight  r = c;
	ItemIdx b = 0;
	Profit  p_bar = 0;
	for (b=1; b<=n; ++b) {
		Weight wi = instance.weight(b);
		if (wi > r)
			break;
		r     -= wi;
		p_bar += instance.profit(b);
	}

	if (b == n+1) { // all items are in the break solution
		for (ItemIdx i=1; i<=n; ++i)
			sol_curr.set(i, true);
		if (pt != NULL)
			pt->put("Solution.OPT", p_bar);
		if (verbose) {
			std::cout << "OPT " << p_bar << std::endl;
			std::cout << "EXP " << instance.optimum() << std::endl;
		}
		return p_bar;
	}
	Profit pb    = instance.profit(b);
	Weight wb    = instance.weight(b);
	Weight w_bar = instance.capacity() - r;
	Profit z     = p_bar;
	Profit u     = p_bar + r * pb / wb;
	if (z < sol_curr.profit())
		z = sol_curr.profit();

	if (verbose)
		std::cout << "z " << z << " u " << u << " GAP " << u - z << std::endl;

	DBG(std::cout << "n " << n << " c " << c << std::endl;)
	DBG(std::cout << "b " << b << " pb " << pb << " wb " << wb << std::endl;)
	DBG(std::cout << "p_bar " << p_bar << " w_bar " << w_bar << std::endl;)
	DBG(std::cout << "z " << z << " u " << u << std::endl;)

	if (z == u) { // If UB == LB, then stop
		if (z != sol_curr.profit()) {
			for (ItemIdx i=1; i<b; ++i)
				sol_curr.set(i, true);
			for (ItemIdx i=b; i<=n; ++i)
				sol_curr.set(i, false);
		}
		if (pt != NULL)
			pt->put("Solution.OPT", z);
		if (verbose) {
			std::cout << "OPT " << z << std::endl;
			std::cout << "EXP " << instance.optimum() << std::endl;
		}
		return z;
	}

	// Create memory table
	std::map<State2, StateValue2, State2>* maps
		= new std::map<State2, StateValue2, State2>[n-b+2];
	maps[0] = std::map<State2, StateValue2, State2>();

	// Initialization
	maps[0].insert({{w_bar,p_bar,b-1},{b,1,maps[0].end()}}); // s(w_bar,p_bar) = b
	maps[0].begin()->second.pred = maps[0].begin();

	//DBG(std::cout << "-------- Memory --------" << std::endl;
	//for (ItemIdx kk = 0; kk <= 0; ++kk) {
		//std::cout << "t " << kk + b - 1 << ": " << std::flush;
		//for (auto s = maps[kk].begin(); s != maps[kk].end(); ++s)
			//std::cout << s << " ";
		//std::cout << std::endl;
	//}
	//std::cout << "------------------------" << std::endl;)

	for (ItemIdx t=b; t<=n; ++t) { // Recursion
		Weight wt = instance.weight(t);
		Profit pt = instance.profit(t);
		ItemIdx k = t - b + 1;
		DBG(std::cout << "t " << t << " (" << k << ") pt " << pt << " wt " << wt << std::endl;)

		maps[k] = std::map<State2, StateValue2, State2>();
		for (auto s = maps[k-1].begin(); s != maps[k-1].end(); ++s)
			maps[k].insert({
					{s->first.mu, s->first.pi, t},
					{s->second.a, s->second.a, s}});

		//DBG(std::cout << "-------- Memory --------" << std::endl;
		//for (ItemIdx kk = 0; kk <= k; ++kk) {
			//std::cout << "t " << kk + b - 1 << ": " << std::flush;
			//for (auto s = maps[kk].begin(); s != maps[kk].end(); ++s)
				//std::cout << s << " ";
			//std::cout << std::endl;
		//}
		//std::cout << "------------------------" << std::endl;)

		// Add item t
		auto s = maps[k].upper_bound({c+1,0,0});
		auto hint = s;
		if (s != maps[k].begin())
			hint--;
		DBG(std::cout << "+" << std::endl;)
		while (s != maps[k].begin() && (--s)->first.mu <= c) {
			Weight mu_ = s->first.mu + wt;
			Weight pi_ = s->first.pi + pt;

			// Bounding
			if (instance.sort_type() == "partial_efficiency") {
				Profit x_  = ((c -   mu_) * pb) / wb;
				if (c < mu_)
					x_ -= 1;

				DBG(std::cout << "s " << s << " ";)
				DBG(std::cout << "mu_ " << mu_ << " pi_ " << pi_ << " ";)
				DBG(std::cout << "alpha " << z + 1 - x_ << " beta " << u - x_ << " ";)
				DBG(std::cout << std::endl;)

				if (pi_ < z + 1 - x_ || pi_ > u - x_)
					continue;
			} else {
				assert(instance.sort_type() == "efficiency");
				Profit ub = pi_;
				if (c < mu_) {
					ub += ub_dantzig_rev_from(instance, s->second.a-1, c-mu_);
				} else {
					ub += ub_dantzig_from(instance, t, c-mu_);
				}
				if (ub <= z || ub > u)
					continue;
			}

			hint = maps[k].insert(hint, {{mu_, pi_, t}, {s->second.a, 1, s->second.pred}});
			// If (mu_,pi_,t) existed but should be updated
			if (hint->second.a < s->second.a) {
				hint->second.a    = s->second.a;
				hint->second.pred = s->second.pred;
			}
			hint--;
		}

		//DBG(std::cout << "-------- Memory --------" << std::endl;
		//for (ItemIdx kk = 0; kk <= k; ++kk) {
			//std::cout << "t " << kk + b - 1 << ": " << std::flush;
			//for (auto s = maps[kk].begin(); s != maps[kk].end(); ++s)
				//std::cout << s << " ";
			//std::cout << std::endl;
		//}
		//std::cout << "------------------------" << std::endl;)

		// Remove previously added items
		DBG(std::cout << "- ";)
		for (auto s = --(maps[k].end()); s->first.mu > c; --s) {
			//std::cout << "s " << s << std::endl;
			if (s->first.mu > c + wt)
				continue;
			//std::cout << "OK" << std::endl;

			for (ItemIdx j = s->second.a_prec; j < s->second.a; ++j) {
				//std::cout << "j " << j << std::endl;
				Weight mu_ = s->first.mu - instance.weight(j);
				Profit pi_ = s->first.pi - instance.profit(j);

				// Bounding
				if (instance.sort_type() == "partial_efficiency") {
					Profit x_  = ((c - mu_) * pb) / wb;
					if (c < mu_)
						x_ -= 1;
					//DBG(std::cout << "mu_ " << mu_ << " pi_ " << pi_ << " ";)
					//DBG(std::cout << "alpha " << z + 1 - x_ << " beta " << u - x_ << " ";)
					//DBG(std::cout << std::endl;)
					if (pi_ < z + 1 - x_ || pi_ > u - x_)
						continue;
				} else {
					assert(instance.sort_type() == "efficiency");
					Profit ub = pi_;
					if (c < mu_) {
						ub += ub_dantzig_rev_from(instance, j-1, c-mu_);
					} else {
						ub += ub_dantzig_from(instance, t+1, c-mu_);
					}
					if (ub <= z || ub > u)
						continue;
				}

				auto res = maps[k].insert({{mu_, pi_, t}, {j, 1, s}});
				// If (mu_,pi_,t) existed and should be updated
				if (!res.second && res.first->second.a < j) {
					res.first->second.a    = j;
					res.first->second.pred = s;
				}
			}
			s->second.a_prec = s->second.a;
		}
		DBG(std::cout << std::endl;)

		//DBG(std::cout << "-------- Memory --------" << std::endl;
		//for (ItemIdx kk = 0; kk <= k; ++kk) {
			//std::cout << "t " << kk + b - 1 << ": " << std::flush;
			//for (auto s = maps[kk].begin(); s != maps[kk].end(); ++s)
				//std::cout << s << " ";
			//std::cout << std::endl;
		//}
		//std::cout << "------------------------" << std::endl;)
	}

	StateIdx map_size = 0;
	for (ItemIdx k=0; k<=n-b+1; ++k)
		map_size += maps[k].size();

	// Get optimal value
	Profit opt = z;
	auto s = maps[n-b+1].end();
	for (auto st = maps[n-b+1].begin(); st != maps[n-b+1].end(); ++st) {
		if (st->first.mu <= c && st->first.pi > opt) {
			opt = st->first.pi;
			s = st;
		}
	}

	// Retrieve optimal solution
	DBG(std::cout << "Retrieve optimal solution..." << std::endl;)
	if (opt == p_bar && sol_curr.profit() < p_bar) {
		for (ItemIdx i=b; i<=n; ++i)
			sol_curr.set(i, false);
		for (ItemIdx i=1; i<b; ++i)
			sol_curr.set(i, true);
	} else if (opt != z) {
		DBG(std::cout << instance << std::endl;)
		DBG(std::cout << "w_bar " << w_bar << " p_bar " << p_bar << " b " << b << std::endl;)
		DBG(auto s_tmp = s;
		while (s_tmp != s_tmp->second.pred) {
			std::cout << s_tmp << std::endl;
			s_tmp = s_tmp->second.pred;
		})

		for (ItemIdx i=b; i<=n; ++i)
			sol_curr.set(i, false);
		for (ItemIdx i=1; i<b; ++i)
			sol_curr.set(i, true);

		DBG(std::cout << "Profit curr " << sol_curr.profit() << std::endl;)

		ItemIdx t = n;
		ItemIdx a = s->second.a;
		while (!(sol_curr.profit() == opt && sol_curr.remaining_capacity() >= 0)) {
			auto s_next = s->second.pred;
			DBG(std::cout << "s " << s << " s_next " << s_next << std::endl;)

			if (s_next->first.pi < s->first.pi) {
				while (s->first.mu != s_next->first.mu + instance.weight(t)
						|| s->first.pi != s_next->first.pi + instance.profit(t)) {
					t--;
					DBG(std::cout << "t " << t << " wt " << instance.weight(t) << " pt " << instance.profit(t) << " ";)
					assert(t >= b - 1);
				}
				sol_curr.set(t, true);
				DBG(std::cout << "Add " << t << " profit " << sol_curr.profit() << std::endl;)
				t--;
				DBG(std::cout << "t " << t << " wt " << instance.weight(t) << " pt " << instance.profit(t) << " ";)
				assert(t >= b - 1);
			} else if (s_next->first.pi > s->first.pi) {
				while (s->first.mu + instance.weight(a) != s_next->first.mu
						|| s->first.pi + instance.profit(a) != s_next->first.pi) {
					a++;
					DBG(std::cout << "a " << a << " ";)
					assert(a <= b);
				}
				sol_curr.set(a, false);
				DBG(std::cout << "Remove " << s->second.a << " profit " << sol_curr.profit() << std::endl;)
				a++;
				DBG(std::cout << "a " << a << " ";)
				assert(a <= b);
			}

			s = s_next;
		}
	}

	DBG(std::cout << "End..." << std::endl;)

	if (pt != NULL) {
		pt->put("Solution.OPT", sol_curr.profit());
	}
	if (verbose) {
		std::cout << "OPT " << sol_curr.profit() << std::endl;
		std::cout << "EXP " << instance.optimum() << std::endl;
	}

	delete[] maps;

	assert(instance.optimum() == 0 || sol_curr.profit() == instance.optimum());

	return sol_curr.profit();
}

#undef DBG
