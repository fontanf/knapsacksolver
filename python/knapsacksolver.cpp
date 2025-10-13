#include "knapsacksolver/algorithms/dynamic_programming_primal_dual.hpp"
#include "knapsacksolver/instance_builder.hpp"

#include <pybind11/pybind11.h>

namespace py = pybind11;

knapsacksolver::Solution solve(
        knapsacksolver::Instance& instance,
        bool verbosity_level)
{
    knapsacksolver::DynamicProgrammingPrimalDualParameters parameters;
    parameters.verbosity_level = verbosity_level;
    auto output = knapsacksolver::dynamic_programming_primal_dual(
            instance,
            parameters);
    return output.solution;
}

PYBIND11_MODULE(knapsacksolver, m)
{
    py::class_<knapsacksolver::Instance>(m, "Instance");
    py::class_<knapsacksolver::InstanceBuilder>(m, "Instance")
        .def(py::init<>())
        .def("read", &knapsacksolver::InstanceBuilder::read)
        .def("set_capacity", &knapsacksolver::InstanceBuilder::set_capacity)
        .def("add_item", &knapsacksolver::InstanceBuilder::add_item);
    py::class_<knapsacksolver::Solution>(m, "Solution")
        .def("contains", &knapsacksolver::Solution::contains)
        .def("number_of_items", &knapsacksolver::Solution::number_of_items)
        .def("profit", &knapsacksolver::Solution::profit)
        .def("weight", &knapsacksolver::Solution::weight);
    m.def("solve", &solve,
            py::arg("instance"),
            py::arg("verbosity_level") = 1);
}
