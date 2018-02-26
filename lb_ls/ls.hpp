#ifndef LS_HPP_Z1AD4OFD
#define LS_HPP_Z1AD4OFD

#include "../lib/instance.hpp"
#include "../lib/solution.hpp"

bool best_exchange(const Solution& sol, Info* info = NULL);

Solution sol_forwardgreedybest(const Instance& ins, Info* info = NULL);
Solution sol_backwardgreedybest(const Instance& ins, Info* info = NULL);
Solution sol_bestgreedyplus(const Instance& ins, Info* info = NULL);

#endif /* end of include guard: LS_HPP_Z1AD4OFD */
