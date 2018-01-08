#include "balknap.hpp"

#include "../lb_ls/ls.hpp"
#include "../lb_greedy/greedy.hpp"

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
	size_t rl2 = rl1 * 2 * w_max;
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
		if (x < 0)
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
			if (x < 0)
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
			if (x < 0)
				x -= 1;
			if (x_ < 0)
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
			if (x < 0)
				x -= 1;
			for (Profit pi=z+1-x; pi<=u-x; ++pi) {
				Profit p = pi + x - z - 1;
				for (ItemIdx j=s0[IDX1(w,p)]; j<s1[IDX1(w,p)]; ++j) {
					Weight mu_ = mu - instance.weight(j);
					Profit pi_ = pi - instance.profit(j);
					Profit x_  = ((c - mu_) * pb) / wb;
					if (x_ < 0)
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
		if (x < 0)
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
	size_t rl2 = rl1 * 2 * w_max;
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
		if (x < 0)
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
			if (x < 0)
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
			if (x < 0)
				x -= 1;
			if (x_ < 0)
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
			if (x < 0)
				x -= 1;
			for (Profit pi=z+1-x; pi<=u-x; ++pi) {
				Profit p = pi + x - z - 1;
				for (ItemIdx j=s[IDX2(k-1,w,p)]; j<s[IDX2(k,w,p)]; ++j) {
					Weight mu_ = mu - instance.weight(j);
					Profit pi_ = pi - instance.profit(j);
					Profit x_  = ((c - mu_) * pb) / wb;
					if (x_ < 0)
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
	size_t idx_opt = 0;
	for (Weight mu=c-w_max+1; mu<=c; ++mu) {
		Weight w = mu + w_max - c - 1;
		Profit x = ((c - mu) * pb) / wb;
		if (x < 0)
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

		size_t  idx = idx_opt;
		ItemIdx k   = idx / rl2;
		ItemIdx t   = k + b - 1;
		Weight  w   = (idx % rl2) / rl1;
		Weight  mu  = c + 1 + w - w_max;
		Profit  p   = (idx % rl2) % rl1;
		Profit  x   = ((c - mu) * pb) / wb;
		if (x < 0)
			x -= 1;
		Profit pi = p + z + 1 - x;
		DBG(std::cout << "t " << t << " wt " << instance.weight(t) << " pt " << instance.profit(t) << " mu " << mu << " pi " << pi << std::endl;)

		while (sol_curr.profit() != opt && sol_curr.remaining_capacity() >= 0) {
			size_t  idx_next = pred[idx];
			ItemIdx k_next   = idx_next / rl2;
			ItemIdx t_next   = k_next + b - 1;
			Weight  w_next   = (idx_next % rl2) / rl1;
			Weight  mu_next  = c + 1 + w_next - w_max;
			Profit  p_next   = (idx_next % rl2) % rl1;
			Profit  x_next   = ((c - mu_next) * pb) / wb;
			if (x_next < 0)
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
