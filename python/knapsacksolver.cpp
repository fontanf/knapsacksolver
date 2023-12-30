#include "knapsacksolver/knapsack/algorithms/dynamic_programming_primal_dual.hpp"
#include "knapsacksolver/knapsack/instance_builder.hpp"

#include <pybind11/pybind11.h>

namespace py = pybind11;

knapsacksolver::knapsack::Solution solve(
        knapsacksolver::knapsack::Instance& instance,
        bool verbosity_level)
{
    knapsacksolver::knapsack::DynamicProgrammingPrimalDualParameters parameters;
    parameters.verbosity_level = verbosity_level;
    auto output = knapsacksolver::knapsack::dynamic_programming_primal_dual(
            instance,
            parameters);
    return output.solution;
}

PYBIND11_MODULE(knapsacksolver, m)
{
    py::class_<knapsacksolver::knapsack::Instance>(m, "Instance");
    py::class_<knapsacksolver::knapsack::InstanceBuilder>(m, "Instance")
        .def(py::init<>())
        .def("read", &knapsacksolver::knapsack::InstanceBuilder::read)
        .def("set_capacity", &knapsacksolver::knapsack::InstanceBuilder::set_capacity)
        .def("add_item", &knapsacksolver::knapsack::InstanceBuilder::add_item);
    py::class_<knapsacksolver::knapsack::Solution>(m, "Solution")
        .def("contains", &knapsacksolver::knapsack::Solution::contains)
        .def("number_of_items", &knapsacksolver::knapsack::Solution::number_of_items)
        .def("profit", &knapsacksolver::knapsack::Solution::profit)
        .def("weight", &knapsacksolver::knapsack::Solution::weight);
    m.def("solve", &solve,
            py::arg("instance"),
            py::arg("verbosity_level") = 1);
}
