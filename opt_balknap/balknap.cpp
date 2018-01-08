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

/*
Profit sopt_balknap(const Instance& instance, Solution& sol_curr,
		boost::property_tree::ptree* pt, bool verbose)
{
	if (opt != z) {
		for (ItemIdx i=1; i<=n; ++i)
			sol_curr.set(i, false);
		for (ItemIdx i=1; i<s[idx_opt]; ++i)
			sol_curr.set(i, true);

		size_t  idx   = idx_opt;
		ItemIdx k     = idx / row_length;
		ItemIdx t     = k + b - 1;
		Weight  abcum = idx % row_length;
		Weight  w     = 0;
		while (abcum > ab_cumulate_diff[w+1])
			w++;
		//Weight mu = c + 1 + w - w_max;
		Profit p  = (ab_cumulate_diff[w+1] - abcum);
		Profit pi = p + alpha[w];

		while (sol_curr.profit() != opt) {
			//DBG(std::cout
				//<< "t " << t
				////<< " mu " << mu
				//<< " pi " << pi
				//<< " p " << sol_curr.profit()
				//<< std::endl;)

			size_t  idx_next   = pred[idx];
			ItemIdx k_next     = idx_next / row_length;
			ItemIdx t_next     = k_next + b - 1;
			Weight  abcum_next = idx_next % row_length;
			Weight  w_next     = 0;
			while (abcum_next > ab_cumulate_diff[w_next+1])
				w_next++;
			//Weight mu_next = c + 1 + w_next - w_max;
			Profit p_next  = (ab_cumulate_diff[w_next+1] - abcum_next);
			Profit pi_next = p_next + alpha[w_next];

			if (k_next < k && pi_next < pi)
				sol_curr.set(t, true);

			idx = idx_next;
			k   = k_next;
			t   = t_next;
			w   = w_next;
			//mu  = mu_next;
			p   = p_next;
			pi  = pi_next;
		}
	}

	if (pt != NULL) {
		pt->put("Solution.OPT", opt);
	}
	if (verbose) {
		std::cout << "OPT " << opt << std::endl;
		std::cout << "EXP " << instance.optimum() << std::endl;
	}

	//delete[] alpha;
	//delete[] beta;
	delete[] ab_cumulate_diff;
	delete[] s;

	return opt;
}
*/

