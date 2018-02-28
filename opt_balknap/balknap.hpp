#ifndef BALKNAP_HPP_UHNG2R08
#define BALKNAP_HPP_UHNG2R08

#include "../lib/instance.hpp"
#include "../lib/solution.hpp"

/**
 * Time  O(n*wmax*pmax)
 * Space O(wmax*pmax)
 */
Profit opt_balknap(const Instance& instance,
        Profit lb, std::string ub_type, Info* info = NULL);

/**
 * Time  O(n*wmax*pmax)
 * Space O(n*wmax*pmax)
 */
Solution sopt_balknap(const Instance& instance,
        Profit lb, std::string ub_type, Info* info = NULL);

/**
 * Time  O(n*wmax*pmax*log(wmax*pmax))
 * Space O(wmax*pmax)
 */
Profit opt_balknap_list(const Instance& instance,
        Profit lb, std::string ub_type, Info* info = NULL);

/**
 * Time  O(n*wmax*pmax*log(wmax*pmax))
 * Space O(n*wmax*pmax)
 */
Solution sopt_balknap_list(const Instance& instance,
        Profit lb, std::string ub_type, Info* info = NULL);

#endif /* end of include guard: BALKNAP_HPP_UHNG2R08 */
