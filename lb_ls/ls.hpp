#ifndef LOCAL_SEARCH_HPP_AVFBNDDM
#define LOCAL_SEARCH_HPP_AVFBNDDM

#include "../lib/instance.hpp"
#include "../lib/solution.hpp"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

/**
 * Local search
 * Moves: remove up to 2 items and add up to 2 items
 * First fit
 */

/* Update solution */
Profit sol_ls(Solution& solution, Profit ub = -1,
		boost::property_tree::ptree* pt = NULL, bool verbose = false);
/* Return new solution */
Solution sol_ls(const Instance& instance, Profit ub = -1,
		boost::property_tree::ptree* pt = NULL, bool verbose = false);
/* Return lb */
Profit lb_ls(const Instance& instance, Profit ub = -1);

#endif /* end of include guard: LOCAL_SEARCH_HPP_AVFBNDDM */
