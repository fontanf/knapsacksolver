#ifndef LS_HPP_Z1AD4OFD
#define LS_HPP_Z1AD4OFD

#include "../lib/instance.hpp"
#include "../lib/solution.hpp"

Solution sol_forwardgreedynlogn(const Instance& ins, Info* info = NULL);
Solution sol_backwardgreedynlogn(const Instance& ins, Info* info = NULL);
Solution sol_bestgreedynlogn(const Instance& ins, Info* info = NULL);

#endif /* end of include guard: LS_HPP_Z1AD4OFD */
