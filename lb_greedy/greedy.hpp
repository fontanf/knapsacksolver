#ifndef GREEDY_HPP_D4Y7KHIE
#define GREEDY_HPP_D4Y7KHIE

#include "../lib/instance.hpp"
#include "../lib/solution.hpp"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

/**
 * Return break solution.
 */
Solution sol_break(const Instance& ins,
        boost::property_tree::ptree* pt = NULL, bool verbose = false);

/**
 * Greedy algorithm
 */
Solution sol_greedy(const Instance& ins,
        boost::property_tree::ptree* pt = NULL, bool verbose = false);

/**
 * Add the item of maximum profit and then fill the knapsack with the greedy
 * algorithm.
 */
Solution sol_greedymax(const Instance& ins,
        boost::property_tree::ptree* pt = NULL, bool verbose = false);

/**
 * Add the best item to the break solution.
 */
Solution sol_forwardgreedy(const Instance& ins,
        boost::property_tree::ptree* pt = NULL, bool verbose = false);

/**
 * Remove the worst item from the break solution with the break item.
 */
Solution sol_backwardgreedy(const Instance& ins,
        boost::property_tree::ptree* pt = NULL, bool verbose = false);

/**
 * Run all the previous greedy algorithm and return the best solution.
 */
Solution sol_bestgreedy(const Instance& ins,
        boost::property_tree::ptree* pt = NULL, bool verbose = false);

/**
 * Greedy algorithm with items n1..n2
 */
Profit lb_greedy_from_to(const Instance& instance,
        ItemIdx n1, ItemIdx n2, Weight c);

/**
 * Greedy algorithm with items first..i1-1,i2+1..last
 */
Profit lb_greedy_except(const Instance& instance,
        ItemIdx first, ItemIdx i1, ItemIdx i2, ItemIdx last, Weight c);

#endif /* end of include guard: GREEDY_HPP_D4Y7KHIE */
