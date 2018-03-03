#ifndef SOLVER_HPP_WBKVVBLA
#define SOLVER_HPP_WBKVVBLA

#include "instance.hpp"
#include "solution.hpp"

#include <string>
#include <map>

class Solver
{

public:

    Profit run(
            Instance& instance,
            Solution& sol_best,
            const std::map< std::string, std::string>& param,
            Info* info);

protected:

    virtual Profit run_algo(
            Instance& instance,
            Solution& sol_best,
            Profit ub,
            const std::map<std::string, std::string>& param,
            Info* info) = 0;

private:

};

#endif /* end of include guard: SOLVER_HPP_WBKVVBLA */
